/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include <arpa/inet.h>  //inet_aton
#include <netinet/in.h> //sockaddr_in
#include "../source/parse_args.h"
//#include "../source/func.h"
#include "../source/headers.h"
#include "../source/readfile.h"
#include "../source/packet_id.h"


#define MAX_NUM_OF_FILES 10
#define MAX_CHARS_PER_LINE 512

#define member_size(type, member) sizeof(((type *)0)->member)

int main (int argc, char **argv)
{
  

    //##################### OPARSE ARGS STARTS HERE:##################################
  
    struct arguments arguments;
  
    /* Set argument defaults */
    arguments.outfile = NULL;
    arguments.dest_ip = "";
    arguments.dest_port = "";
    arguments.verbose = 0;
    arguments.debug   = 0;
    int arg_i = 0 ; for (arg_i=0; arg_i<10; arg_i++)  arguments.args[arg_i]=NULL;

  
    // Parse the Arguments:
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
  
  
    if (arguments.verbose != 0){
        printf ( "\n");
        printf ( "These arguments received:\n");
        printf ( "  --dest-ip:    \"%s\"\n",   arguments.dest_ip);
        printf ( "  --dest-port:  \"%s\"\n", arguments.dest_port);
        printf ( "  --output-file: \"%s\"\n\n", arguments.outfile);
        //int arg_i = 0 ;
        for (arg_i=0; arg_i<10; arg_i++)  printf ( "arguments.args[%d]:%s\n", arg_i, arguments.args[arg_i]);
    }

    //Make sure dest ip and dest port are provided via options:
    if (arguments.dest_ip == "" || arguments.dest_port == "" || arguments.args[0] == NULL || arguments.outfile == NULL) {
        fprintf(stderr, "Make sure to provide: \n\t--dest-ip\n\t--dest-port\n\t--input-file\n\t--output-file\n\nUse --help for more information.\n"); 
        return -1;
    } 
  
    struct sockaddr_in adr_inet;
  
    //Check the IP Addresses to be 4 ascii octets ("127.0.0.1"):
    if ( !inet_aton(arguments.dest_ip, &adr_inet.sin_addr) ) { printf("Bad IP Address %s\n", arguments.dest_ip); return -1;}
    if ( !inet_aton(arguments.dest_port, &adr_inet.sin_port) ) { printf("Bad Port: %s\n", arguments.dest_port); return -1;}

    if (arguments.debug != 0) {
        printf( "The arguments.dest_ip=%s\n",   arguments.dest_ip);
        printf( "The arguments.dest_port=%d\n\n", atoi(arguments.dest_port) );
    }
  
    //##################### OPEN FILE STARTS HERE:##################################
    
    uint16_t current_file;
    uint16_t current_line=0;
    char ** text_line;
    file_info_t file[10];


    for (current_file=0; current_file<10; current_file++){ 

        /*
        int NUM_OF_LINES;

        // Create file descriptor:
        file[current_file].file_id = packet_id();
        FILE *fd = NULL;
      
        if ((fd=fopen(arguments.args[current_file],"r"))==NULL) {
            fprintf(stderr, "Unable to open file:%s\nUse --input-file option, and make sure the file is present.\n", arguments.args[0]); 
            return -1; 
        } 
     
    
        file[current_file].number_of_lines_in_file = getLinesPerFile(fd);
    
        //getData(fd, &NUM_OF_LINES, 1, text_line);
        getData(fd, &NUM_OF_LINES, 1, file[current_file].text_line);
        file[current_file].number_of_lines_in_file = NUM_OF_LINES;
    
        if (arguments.debug != 0) {
            printf("file[%d].file_id:                 0x%04x\n", current_file, file[current_file].file_id);
            printf("file[%d].number_of_lines_in_file: 0x%04x\n", current_file, file[current_file].number_of_lines_in_file);
            for(current_line=0; current_line<file[current_file].number_of_lines_in_file; current_line++){
                printf("file[%d].text_line[%d]:            %s", current_file, current_line, file[current_file].text_line[current_line] );
            }
        }
        
        */

        getFileInfo( &file[current_file], arguments.args[current_file], current_file, 1);

    }

    /*
    for (current_file=0; current_file<10; current_file++){ 
        printf("file[%d].file_id:                 0x%04x\n", current_file, file[current_file].file_id);
        printf("file[%d].number_of_lines_in_file: 0x%04x\n", current_file, file[current_file].number_of_lines_in_file);
        for(current_line=0; current_line<file[current_file].number_of_lines_in_file; current_line++){
            printf("file[%d].text_line[%d]:            %s", current_file, current_line, file[current_file].text_line[current_line]);
            }

    }
    */
    
    //##################### UDP SENDER STARTS HERE:##################################

    struct sockaddr_in tx_to_address;

    memset(&tx_to_address, 0, sizeof(tx_to_address));
  
    tx_to_address.sin_family      = AF_INET; // Receiving Socket Family
    tx_to_address.sin_addr.s_addr = inet_addr(arguments.dest_ip);// Receiving Socket IP Address
    tx_to_address.sin_port        = htons(atoi(arguments.dest_port)); // Receiving Socket Port Number

    if (arguments.debug != 0) {
        printf( "tx_to_address.sin_family      = AF_INET\n");
        printf( "tx_to_address.sin_addr.s_addr = 0x%08x\n", tx_to_address.sin_addr.s_addr);
        printf( "tx_to_address.sin_port        = 0x%04x\n\n", tx_to_address.sin_port);
    }

    //OPEN A SOCKET AND CATCH THE FD:
    int tx_socket_fd = -1;
    tx_socket_fd     = socket(AF_INET,SOCK_DGRAM,0); 
    if (tx_socket_fd < 0) {perror("error: failed to open datagram socket\n"); exit(1); }
  
    //for(current_line=0; current_line<file[0].number_of_lines_in_file; current_line++) printf("%s",file[0].text_line[current_line]);
    for (current_file=0; current_file<10; current_file++){ 
        printf("file[%d].file_id:                 0x%04x\n", current_file, file[current_file].file_id);
        printf("file[%d].number_of_lines_in_file: 0x%04x\n", current_file, file[current_file].number_of_lines_in_file);
        for(current_line=0; current_line<file[current_file].number_of_lines_in_file; current_line++){
            printf("file[%d].text_line[%d]:            %s", current_file, current_line, file[current_file].text_line[current_line]);
            }

    }

    file_x_app_layer_t * app_layer;
    app_layer = malloc( sizeof(file_x_app_layer_t) );
    (*app_layer).file_id = packet_id();
    (*app_layer).current_line = file[0].number_of_lines_in_file;
    (*app_layer).total_lines = strlen(arguments.outfile);
    (*app_layer).ack = 0;
    (*app_layer).reserved = 0;
    (*app_layer).init = 1;


    //##################### SEND INIT PACKET BEGIN:##################################

    int test;


    //RX SOCKET:
    struct sockaddr_in rx_local_address; // AF_INET
    memset(&rx_local_address, 0, sizeof(rx_local_address));
    rx_local_address.sin_family      = AF_INET;
    //rx_local_address.sin_addr.s_addr = tx_to_address.sin_addr.s_addr;
    //rx_local_address.sin_port        = tx_to_address.sin_port;
    rx_local_address.sin_addr.s_addr = inet_addr(arguments.source_ip);
    rx_local_address.sin_port        = htons(atoi(arguments.source_port));
    if (rx_local_address.sin_addr.s_addr == INADDR_NONE) bail("bad source address");
    //OPEN A SOCKET AND CATCH THE FD:
    int rx_socket_fd = -1; 
    rx_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
    if (rx_socket_fd == -1) bail("Failed to create a socket; see line: rx_socket_fd=socket(AF_INET,SOCKET_DGRAM,0);");
    test = bind( rx_socket_fd,                          \
                 (struct sockaddr *) &rx_local_address, \
                 sizeof(rx_local_address)               );
    if (test == -1) bail("bind()");



    //(*app_layer).total_lines = strlen(arguments.outfile);
    (*app_layer).file_id = 0;
    (*app_layer).current_line = 0;
    (*app_layer).total_lines = 10;
    (*app_layer).ack = 0;
    (*app_layer).init = 1;
    (*app_layer).fin = 0;
    (*app_layer).reserved = 0;


    //print the applicatin header:
    printf("\n\napp_layer: 0x%08x\n",(*app_layer));
    printf("app_layer.file_id: %d\n",(*app_layer).file_id);
    printf("(*app_layer).current_line: %d\n",(*app_layer).current_line);
    printf("(*app_layer).total_lines: %d\n",(*app_layer).total_lines);
    printf("(*app_layer).init: %d\n",(*app_layer).init);
    printf("(*app_layer).ack: %d\n",(*app_layer).ack);
    printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);



    //Allocate the memory to store the 10 file ids with the destination file's name appended at the end:
    char * init_packet_payload = malloc( (10*2) + strlen(arguments.outfile));

    //put all file_ids at the head of the allocated memory:
    for (current_file=0; current_file<10; current_file++)
        *(init_packet_payload+(current_file*2)) = file[current_file].file_id;

    //place the destination file's name at the tail of the allocated memory:   
    memcpy( (init_packet_payload+(10*2)) , arguments.outfile , strlen(arguments.outfile) );
    
    //Concatinate the Application Header with the Payload:
    char * init_payload = concat_bytes(app_layer, sizeof(file_x_app_layer_t), init_packet_payload, (10*2)+strlen(arguments.outfile) );

    printBytes(init_payload, sizeof(file_x_app_layer_t) + strlen(arguments.outfile) + (2*10) );
    //printf("NULL: %02x %02x \n", NULL, '\0');
    
    //Send the init packet:
    test=sendto( tx_socket_fd,                                                \
                 init_payload,                                                \
                 sizeof(file_x_app_layer_t)+strlen(arguments.outfile)+(10*2), \
                 0,                                                           \
                 (struct sockaddr *) &tx_to_address,                          \
                 sizeof(tx_to_address)                                        );

    if ( test < 0) printf("Failed to send line.\n");

    //##################### SEND INIT PACKET END:##################################



    //##################### RECEIVE ACK PACKET END:##################################
    

    char receiveDgramBuffer[MAX_CHARS_PER_LINE]; // Receive Buffer

    struct sockaddr_in rx_from_address; // AF_INET
    int rxSockLen = sizeof(rx_from_address);

    test = recvfrom( rx_socket_fd,                         \
                     receiveDgramBuffer,                   \
                     sizeof(receiveDgramBuffer),           \
                     0,                                    \
                     (struct sockaddr *) &rx_from_address, \
                     &rxSockLen                            );

    app_layer = (file_x_app_layer_t *) receiveDgramBuffer;

    if ( (*app_layer).init == 0 || (*app_layer).ack == 0 ) { printf("Expected INIT and ACK field to be set!!!\n"); return -1; }

    printf("(*app_layer).init: %d\n",(*app_layer).init);
    printf("(*app_layer).ack: %d\n",(*app_layer).ack);

    //##################### RECEIVE ACK PACKET END:##################################


    //Send data:
//    int test;
    char *app_header_n_data=NULL;
//    for(current_line=0, test=-1; current_line<number_of_lines_in_file[0]; current_line++)

    for(current_file=0; current_file<10; current_file++)
    {

        //for(current_line=file[current_file].number_of_lines_in_file-1, test=-1; current_line>=0; current_line--)
        for(current_line=0, test=-1; current_line<file[current_file].number_of_lines_in_file; current_line++)
        {
    
            (*app_layer).file_id = file[current_file].file_id ;
            (*app_layer).current_line = current_line;
            (*app_layer).total_lines = file[current_file].number_of_lines_in_file;
            (*app_layer).init = 0;
            (*app_layer).ack = 0;
            (*app_layer).fin = 0;
            (*app_layer).reserved = 0;
    
            printf("\n\napp_layer: 0x%08x\n",(*app_layer));
            printf("(*app_layer).file_id: %d\n",(*app_layer).file_id);
            printf("(*app_layer).current_line: %d\n",(*app_layer).current_line);
            printf("(*app_layer).total_lines: %d\n",(*app_layer).total_lines);
            printf("(*app_layer).init: %d\n",(*app_layer).init);
            printf("(*app_layer).ack: %d\n",(*app_layer).ack);
            printf("(*app_layer).fin: %d\n",(*app_layer).fin);
            printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);
            
    
            //app_header_n_data=malloc( sizeof(file_x_app_layer_t) + strlen(file[0].text_line[current_line]) );
            //memcpy(app_header_n_data, app_layer , sizeof(file_x_app_layer_t) );
            //memcpy(app_header_n_data+(sizeof(file_x_app_layer_t)), file[0].text_line[current_line], strlen(file[0].text_line[current_line])-1 );
            app_header_n_data = concat_bytes(app_layer, sizeof(file_x_app_layer_t), 
                                             file[current_file].text_line[current_line], strlen(file[current_file].text_line[current_line])-1 );
            
            printBytes(app_header_n_data, sizeof(file_x_app_layer_t) + strlen(file[current_file].text_line[current_line]) -1);
    
            //SEND THE DATA: TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
            test=sendto( tx_socket_fd, 
                         app_header_n_data,                                             \
                         strlen(file[current_file].text_line[current_line])+sizeof(file_x_app_layer_t)-1,    \
                         0,                                                             \
                         (struct sockaddr *) &tx_to_address,                            \
                         sizeof(tx_to_address)                                          );
    
            if ( test < 0) printf("Failed to send line(%d).\n", current_line);
        }
    }    

    /*
    for(current_line=0; current_line<number_of_lines_in_file[0]; current_line++)
    {
        free(file[0].text_line[current_line]);
    }*/
  
    return 0;
}
