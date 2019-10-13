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

#define MAX_NUM_OF_LINES 20

//received_lines_record
typedef struct received_lines_records {
    unsigned int mask : MAX_NUM_OF_LINES;
} received_lines_records_t;


int main (int argc, char **argv)
{
    received_lines_records_t received_lines_record ;
    received_lines_record.mask = 0;
    received_lines_records_t expected_lines_record ;
    expected_lines_record.mask = 0;

    struct server_arguments arguments;
    
    //argument defaults:
    arguments.outfile = NULL;
    arguments.source_ip   = "";
    arguments.source_port = "";
    arguments.verbose = 0;
    arguments.debug   = 0;
    
    //Parse the options:
    argp_parse(&server_argp, argc, argv, 0, 0, &arguments);
    
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
    
    #define MAX_CHARS_PER_LINE 512
    
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
    
    char receiveDgramBuffer[MAX_CHARS_PER_LINE]; // Receive Buffer
    
    //Receieve INIT PAKET begin:

    int txSockLen = sizeof(rx_from_address);
    test = recvfrom( rx_socket_fd,                         \
                     receiveDgramBuffer,                   \
                     sizeof(receiveDgramBuffer),           \
                     0,                                    \
                     (struct sockaddr *) &rx_from_address, \
                     &txSockLen                            );

    //Allocate the memory to store: the application header + the data following the application header + 1 byte for NULL:
    file_x_app_layer_t * app_layer = (file_x_app_layer_t *) malloc(test + 1);
    //Copy the header and the data to the allocated memory:
    memcpy(app_layer, receiveDgramBuffer, test);
    if ( (*app_layer).init == 0 ) { printf("Expected INIT packet but got NON-INIT packet!!!\n"); return -1; }
    //Get the data part of the packet containing the file name::
    char * init_data = (char *)  ( ((char *) app_layer) + sizeof(file_x_app_layer_t) );
    //Terminate the string with null:
    *(init_data + test-sizeof(file_x_app_layer_t) ) = NULL;
    //Point to the destination file's name at the tail of the init packet data:
    char *destination_file_name = init_data + (10*2);
    
    //file_info_t file[10];
    file_info_t file[(*app_layer).total_lines]; // FIXME: create a union of two strucs for app_layer and rename the total_liens to total_files
    
    //Copy the file_ids to each file data structure:
    int current_file;
    for (current_file=0; current_file<10; current_file++){
        file[current_file].file_id = (uint16_t *) *(init_data+(current_file*2));
        printf("file[%d].file_id=0x%04x\n",current_file, file[current_file].file_id);
        printf("file[%d].file_id=%d\n",current_file, file[current_file].file_id);
    }
    
    printf("\nDestination File:%s\n", destination_file_name );

    //printBytes(app_header_n_data, test);
    printf("(*app_layer): 0x%08x\n",(*app_layer));
    printf("(*app_layer).file_id: %d\n",(*app_layer).file_id);
    printf("(*app_layer).current_line: %d\n",(*app_layer).current_line);
    printf("(*app_layer).total_lines: %d\n",(*app_layer).total_lines);
    printf("(*app_layer).init: %d\n",(*app_layer).init);
    printf("(*app_layer).ack: %d\n",(*app_layer).ack);
    printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);


    //################ REPLY TO INIT BEGIN ###################

    (*app_layer).ack = 1;

    //OPEN A SOCKET AND CATCH THE FD:
    int tx_socket_fd = -1;
    tx_socket_fd     = socket(AF_INET,SOCK_DGRAM,0);
    if (tx_socket_fd < 0) { printf("error: failed to open datagram socket\n"); return -1; }
    
    rx_from_address.sin_port        = htons(atoi("7778"));

    //Send the init packet:
    test=sendto( tx_socket_fd,                          \
                 app_layer,                             \
                 sizeof(file_x_app_layer_t),            \
                 0,                                     \
                 (struct sockaddr *) &rx_from_address,  \
                 sizeof(rx_from_address)               );

    if ( test < 0) printf("Failed to send line.\n");

    //################ REPLY TO INIT END ###################

    /*
    char * packet_data [(*app_layer).current_line] ;

    int current_line;
    int total_lines=(*app_layer).total_lines;

    //Each bit in the mask corresponds to received line.
    //If the nth bit is unset, the nth line is not expected to be received.
    //If the nth bit is set, the nth line is expected to be received.
    //For example, if trasnfer is to copy 5 lines, bits 0th thru 4th will be set, and rest unset.


    //Initialize the expected line record based on the data obtained from init packet
    for(current_line=0;current_line<total_lines;current_line++)  expected_lines_record.mask = expected_lines_record.mask | (1<<current_line);

    while (received_lines_record.mask != expected_lines_record.mask)
    {
        //Receive a line of text:
        test = recvfrom( rx_socket_fd,                         \
                         receiveDgramBuffer,                   \
                         sizeof(receiveDgramBuffer),           \
                         0,                                    \
                         (struct sockaddr *) &rx_from_address, \
                         &txSockLen                            );

        if ( test < 0) bail("recvfrom(2)"); else  printf("GOT %d BYTES\n", test);
        printBytes(receiveDgramBuffer, test);

        app_layer = receiveDgramBuffer;
        
        if  ( (received_lines_record.mask & (1 << (*app_layer).current_line)) ) ; // continue;
 
        received_lines_record.mask =  ( received_lines_record.mask | (1 << (*app_layer).current_line) );
        printf("mask=0x%04x\n", received_lines_record.mask);

        //received_lines_record.mask = 0;

        // Terminate the received string with Null (maybe it's a good idea to also check the received string to make sure no nulls are present?!):
        //receiveDgramBuffer[test] = '\n'; //NULL terminate the received string
        
        //Allocate the memory to store the application header and the data following the application header:
        app_layer = (file_x_app_layer_t *) malloc(test + 2);
        //Copy the header and the data to the allocated memory:
        memcpy(app_layer, receiveDgramBuffer, test);
        //Get the data part of the packet containing the file name::
        (packet_data[(*app_layer).current_line]) = (char *)  ( ((char *) app_layer) + sizeof(file_x_app_layer_t) );
        //Append the new line and terminate the string with null:
        *(packet_data[(*app_layer).current_line] + test - sizeof(file_x_app_layer_t)    ) = '\n';
        *(packet_data[(*app_layer).current_line] + test - sizeof(file_x_app_layer_t) + 1) = NULL;


        printf("(*app_layer): 0x%08x\n",(*app_layer));
        printf("(*app_layer).file_id: %d\n",(*app_layer).file_id);
        printf("(*app_layer).current_line: %d\n",(*app_layer).current_line);
        printf("(*app_layer).total_lines: %d\n",(*app_layer).total_lines);
        printf("(*app_layer).init: %d\n",(*app_layer).init);
        printf("(*app_layer).ack: %d\n",(*app_layer).ack);
        printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);

        if (arguments.debug != 0){
            printf("RECEIVED DGRAM:%s", packet_data[(*app_layer).current_line]);
            printf("RECEIVED FROM: %s:%u\n\n", inet_ntoa(rx_from_address.sin_addr), (unsigned) ntohs(rx_from_address.sin_port));
        }
        
    }
    
    if (arguments.verbose != 0)
    for(current_line=0;current_line<total_lines;current_line++)  printf("%s", packet_data[current_line]);
    
    shutdown(rx_socket_fd, SHUT_RDWR);
    
    //##################### OPEN FILE BEGIN: ##########################
    
    FILE *outfile_fd = NULL;
    if ( (outfile_fd=fopen(init_data,"w")) == NULL ) {
        printf(stderr, "Unable to open file %s; Use --input-file option, and make sure the file is present.\n", arguments.outfile); 
        return -1; }
    else  {
        fflush(outfile_fd);
        for(current_line=0;current_line<total_lines;current_line++) fputs(packet_data[current_line], outfile_fd);
        fclose(outfile_fd);
    }  

    //##################### OPEN FILE END: ##########################
 
    for(current_line=0; current_line<total_lines; current_line++)
    {   
       //free(packet_data[current_line]);
    }    
 
    */   
    return 0;
}
  
  
