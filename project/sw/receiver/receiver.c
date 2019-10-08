/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include "../source/parse_args.h"
#include <arpa/inet.h>  //inet_aton
#include <netinet/in.h> //sockaddr_in
#include "../source/func.h"
#include "../source/headers.h"


int main (int argc, char **argv)
{
    struct arguments arguments;
    
    /* Set argument defaults */
    arguments.outfile = NULL;
    arguments.infile = NULL;
    arguments.source_ip   = "";
    arguments.source_port = "";
    arguments.dest_ip   = "";
    arguments.dest_port = "";
    arguments.verbose = 0;
    arguments.debug   = 0;
    
    /* Where the magic happens */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    
    //Set the output stream:
    //FILE *outstream;
    //if (arguments.outfile) outstream = fopen (arguments.outfile, "w");
    //else                   outstream = stdout;
    
    if (arguments.verbose != 0){ 
        printf ( "\n");
        printf ( "These arguments received:\n");
        printf ( "  --source-ip:    \"%s\"\n",   arguments.source_ip);
        printf ( "  --source-port:  \"%s\"\n", arguments.source_port);
        printf ( "  --output-file: \"%s\"\n\n", arguments.outfile);
    } 
    
    //Make sure dest ip and dest port are provided via options:
    if (arguments.source_ip == "" || arguments.source_port == "") {
        fprintf(stderr, "Make sure to provide --source-ip and --source-port\nUse --help for more information.\n");
        return -1;
    }  
    
    struct sockaddr_in adr_inet;
    
    //Check the IP Addresses to be 4 ascii octets ("127.0.0.1"):
    if ( !inet_aton(arguments.source_ip, &adr_inet.sin_addr) ) { printf("BAD ADDRESS %s\n", arguments.source_ip); return -1;}
    if ( !inet_aton(arguments.source_port, &adr_inet.sin_port) ) { printf("Bad Port: %s\n", arguments.source_port); return -1;}
    
    if (arguments.debug != 0) {
        printf( "The arguments.source_ip=%s\n",   arguments.source_ip);
        printf( "The arguments.source_port=%d\n\n", atoi(arguments.source_port) );
    }
    
    
    //##################### UDP RECEIVER STARTS HERE:
    
    // NOTE: Retrieve this info from the initial pocket!
    #define MAX_NUM_OF_TEXT_LINES_PER_FILE 10
    
    int test;
    struct sockaddr_in rx_local_address; // AF_INET
    struct sockaddr_in rx_from_address; // AF_INET
    
    memset(&rx_local_address, 0, sizeof(rx_local_address));
    
    rx_local_address.sin_family      = AF_INET;
    rx_local_address.sin_addr.s_addr = inet_addr(arguments.source_ip);  
    rx_local_address.sin_port        = htons(atoi(arguments.source_port));
    if (rx_local_address.sin_addr.s_addr == INADDR_NONE) bail("bad source address");
    
    if (arguments.debug != 0) {
        printf( "rx_local_address.sin_family      = AF_INET\n");
        printf( "rx_local_address.sin_addr.s_addr = 0x%08x\n",   rx_local_address.sin_addr.s_addr);
        printf( "rx_local_address.sin_port        = 0x%04x\n\n", rx_local_address.sin_port);
    }
    
    //OPEN A SOCKET AND CATCH THE FD:
    int rx_socket_fd = -1;
    rx_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
    if (rx_socket_fd == -1) bail("Failed to create a socket; see line: rx_socket_fd=socket(AF_INET,SOCKET_DGRAM,0);");
    
    if (arguments.debug != 0) printf("FD returned by socket(): %d\n", rx_socket_fd);
    
    //BIND THE SOCKET TO A GIVEN IP/PORT:
    test = bind( rx_socket_fd,                          \
                 (struct sockaddr *) &rx_local_address, \
                 sizeof(rx_local_address)               );
    
    if (test == -1) bail("bind()");
    
    //system("netstat -neopa | grep Recv-Q ");
    //system("netstat -neopa | grep dgram ");
    
    char receiveDgramBuffer[MAX_NUM_OF_TEXT_LINES_PER_FILE][512]; // Receive Buffer
    char receive_buffer[512]; // Receive Buffer
    
    //### Receieve INIT PAKET begin:
    //
        int txSockLen = sizeof(rx_from_address);
        test = recvfrom( rx_socket_fd,                         \
                         receiveDgramBuffer[0],     \
                         sizeof(receiveDgramBuffer[0]),           \
                         0,                                    \
                         (struct sockaddr *) &rx_from_address, \
                         &txSockLen                            );

        char * app_header_n_data;
        app_header_n_data=malloc(test );
        char * init_data;
        init_data=malloc( test - sizeof(file_x_app_layer_t) + 1);
        //memcpy(app_header_n_data, receiveDgramBuffer[0] , sizeof(file_x_app_layer_t) );
        memcpy(app_header_n_data, receiveDgramBuffer[0], test );
        memcpy(init_data, receiveDgramBuffer[0]+sizeof(file_x_app_layer_t), test-sizeof(file_x_app_layer_t) );
        *(init_data+test - sizeof(file_x_app_layer_t)) = NULL;
        file_x_app_layer_t * app_layer = (file_x_app_layer_t *) app_header_n_data;

        printf("\nTEXT LINES: %d\n", (*app_layer).text_lines);
        printf("INIT_DATA: %s\n\n", init_data);

        //printBytes(app_header_n_data, test);
        printf("(*app_layer): 0x%08x\n",(*app_layer));
        printf("(*app_layer).file_id: %d\n",(*app_layer).file_id);
        printf("(*app_layer).text_line: %d\n",(*app_layer).text_lines);
        printf("(*app_layer).payload_size: %d\n",(*app_layer).payload_size);
        printf("(*app_layer).tx_burst: %d\n",(*app_layer).tx_burst);
        printf("(*app_layer).server_ack: %d\n",(*app_layer).server_ack);
        printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);

    //### Receieve INIT PAKET end


    int current_line;
    int total_lines=(*app_layer).text_lines;
//    int txSockLen = sizeof(rx_from_address);
    //for(current_line=0;current_line<MAX_NUM_OF_TEXT_LINES_PER_FILE;current_line++)
    //for(current_line=0;current_line<(*app_layer).text_lines;current_line++)
    for(current_line=0;current_line<total_lines;current_line++)
    {
        //Receive a line of text:
        test = recvfrom( rx_socket_fd,                         \
                         receiveDgramBuffer[current_line],     \
                         sizeof(receiveDgramBuffer),           \
                         0,                                    \
                         (struct sockaddr *) &rx_from_address, \
                         &txSockLen                            );

        app_layer = receiveDgramBuffer[current_line];
        //test = read( rx_socket_fd,                     \
        //                          receiveDgramBuffer[current_line], \
        //                          sizeof(receiveDgramBuffer)        );
        
        if ( test < 0) bail("recvfrom(2)"); else  printf("GOT %d BYTES\n", test);
        printBytes(receiveDgramBuffer[current_line], test);
        
        // Terminate the received string with Null (maybe it's a good idea to also check the received string to make sure no nulls are present?!):
        receiveDgramBuffer[current_line][test] = '\n'; //NULL terminate the received string
        
        if (arguments.debug != 0){
            //printf("RECEIVED DGRAM:\"%s\"", receiveDgramBuffer[current_line]);
            //printf("RECEIVED FROM:\"%s\":%u\n\n", inet_ntoa(rx_from_address.sin_addr), (unsigned) ntohs(rx_from_address.sin_port));
            printf("RECEIVED DGRAM:\"%s\"", receiveDgramBuffer[current_line]+sizeof(file_x_app_layer_t));
            printf("RECEIVED FROM:\"%s\":%u\n\n", inet_ntoa(rx_from_address.sin_addr), (unsigned) ntohs(rx_from_address.sin_port));
        }
    }
    
    if (arguments.verbose != 0)
    for(current_line=0;current_line<total_lines;current_line++)  printf("%s", receiveDgramBuffer[current_line]+sizeof(file_x_app_layer_t));
    //for(current_line=0;current_line<MAX_NUM_OF_TEXT_LINES_PER_FILE;current_line++)  printf("%s", receiveDgramBuffer[current_line]);
    
    shutdown(rx_socket_fd, SHUT_RDWR);
    
    //##################### OPEN FILE STARTS HERE: ##########################
    
    FILE *outfile_fd = NULL;
    if ( (outfile_fd=fopen(init_data,"w")) == NULL ) {
        printf(stderr, "Unable to open file %s; Use --input-file option, and make sure the file is present.\n", arguments.outfile); 
        return -1; }
    else  {
        fflush(outfile_fd);
        for(current_line=0;current_line<total_lines;current_line++) 
            fputs(receiveDgramBuffer[current_line]+sizeof(file_x_app_layer_t), outfile_fd);
            
        fclose(outfile_fd);
    }  
    
    
    return 0;
}
  
  
