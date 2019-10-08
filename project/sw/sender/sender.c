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
#include "../source/readfile.h"
#include "../source/packet_id.h"
#include "../source/headers.h"

#define MAX_NUM_OF_FILES 10

int main (int argc, char **argv)
{
  

    //##################### OPARSE ARGS STARTS HERE:##################################

  
    struct arguments arguments;
  
    /* Set argument defaults */
    arguments.outfile = NULL;
    arguments.infile = NULL;
    arguments.source_ip = "";
    arguments.source_port = "";
    arguments.dest_ip = "";
    arguments.dest_port = "";
    arguments.verbose = 0;
    arguments.debug   = 0;
  
    // Parse the Arguments:
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
  
    //Set the output stream:
    //FILE *outstream;
    //if (arguments.outfile) outstream = fopen (arguments.outfile, "w");
    //else                   outstream = stdout;
  
    if (arguments.verbose != 0){
        printf ( "\n");
        printf ( "These arguments received:\n");
        printf ( "  --dest-ip:    \"%s\"\n",   arguments.dest_ip);
        printf ( "  --dest-port:  \"%s\"\n", arguments.dest_port);
        printf ( "  --input-file: \"%s\"\n\n", arguments.infile);
        printf ( "  --output-file: \"%s\"\n\n", arguments.outfile);
    }

    //Make sure dest ip and dest port are provided via options:
    if (arguments.dest_ip == "" || arguments.dest_port == "" || arguments.infile == NULL || arguments.outfile == NULL) {
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

    // Create file descriptor:
    FILE *fd = NULL;
  
    if ((fd=fopen(arguments.infile,"r"))==NULL) {
        fprintf(stderr, "Unable to open file:%s\nUse --input-file option, and make sure the file is present.\n", arguments.infile); 
        return -1; 
    } 
 
    int line_count=0;
    int current_line=0;

    char * text_line[getLinesPerFile(fd)];

    getData(fd, &line_count, 1, text_line);

    if (arguments.debug != 0) {
        printf("LINE COUNT:%d\n",line_count);                
        for(current_line=0; current_line<line_count; current_line++){
            printf("%s",text_line[current_line]);
            printf("Line=%d stored at 0x%x\n\n", current_line, text_line[current_line]);
        }
    }
  

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
  
    for(current_line=0; current_line<line_count; current_line++) printf("%s",text_line[current_line]);
  
    file_x_app_layer_t * app_layer;
    app_layer = malloc( sizeof(file_x_app_layer_t) );
    (*app_layer).file_id = packet_id();
    (*app_layer).text_lines = line_count;


    //##### Send init

    //char * dest_file_name_ptr = malloc( sizeof(file_x_app_layer_t) + strlen(arguments.outfile) + 1 );
    //memcpy(dest_file_name_ptr, app_layer , sizeof(file_x_app_layer_t) );
    //memcpy(dest_file_name_ptr+sizeof(file_x_app_layer_t), arguments.outfile, strlen(arguments.outfile)+1);
    //*(dest_file_name_ptr + strlen(arguments.outfile)) = '\0';

    char * dest_file_name_ptr = malloc( sizeof(file_x_app_layer_t) + strlen(arguments.outfile));
    memcpy(dest_file_name_ptr, app_layer , sizeof(file_x_app_layer_t) );
    memcpy(dest_file_name_ptr+sizeof(file_x_app_layer_t), arguments.outfile, strlen(arguments.outfile) );

    (*app_layer).text_lines = line_count;
    (*app_layer).payload_size = strlen(arguments.outfile);
    (*app_layer).tx_burst = 1;
    (*app_layer).server_ack = 0;
    (*app_layer).reserved = 0;

    printf("\n\napp_layer: 0x%08x\n",(*app_layer));
    printf("app_layer.file_id: %d\n",(*app_layer).file_id);
    printf("(*app_layer).text_line: %d\n",(*app_layer).text_lines);
    printf("(*app_layer).payload_size: %d\n",(*app_layer).payload_size);
    printf("(*app_layer).tx_burst: %d\n",(*app_layer).tx_burst);
    printf("(*app_layer).server_ack: %d\n",(*app_layer).server_ack);
    printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);

    printBytes(dest_file_name_ptr, sizeof(file_x_app_layer_t) + strlen(arguments.outfile));

//    printf("\n\nINIT PAYLOAD: %s, strlen=%d\n",dest_file_name_ptr, strlen(arguments.outfile) );
//    printf("NULL: %02x %02x \n", NULL, '\0');

    int test;
    //SEND THE DATA: TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
    test=sendto( tx_socket_fd, \
                 dest_file_name_ptr,             \
                 sizeof(file_x_app_layer_t)+strlen(arguments.outfile),     \
                 0,                                   \
                 (struct sockaddr *) &tx_to_address,  \
                 sizeof(tx_to_address)                );

    if ( test < 0) printf("Failed to send line.\n");


    //#####

    //Send data:
//    int test;
    char *app_header_n_data;
    for(current_line=0, test=-1; current_line<line_count; current_line++)
    {

        (*app_layer).text_lines = current_line;
        (*app_layer).payload_size = strlen(text_line[current_line]);
        (*app_layer).tx_burst = 1;
        (*app_layer).server_ack = 0;
        (*app_layer).reserved = 0;

        printf("\n\napp_layer: 0x%08x\n",(*app_layer));
        printf("app_layer.file_id: %d\n",(*app_layer).file_id);
        printf("(*app_layer).text_line: %d\n",(*app_layer).text_lines);
        printf("(*app_layer).payload_size: %d\n",(*app_layer).payload_size);
        printf("(*app_layer).tx_burst: %d\n",(*app_layer).tx_burst);
        printf("(*app_layer).server_ack: %d\n",(*app_layer).server_ack);
        printf("(*app_layer).reserved: %d\n",(*app_layer).reserved);
        

        app_header_n_data=malloc( sizeof(file_x_app_layer_t) + strlen(text_line[current_line]) );
        memcpy(app_header_n_data, app_layer , sizeof(file_x_app_layer_t) );
        memcpy(app_header_n_data+(sizeof(file_x_app_layer_t)), text_line[current_line], strlen(text_line[current_line])-1 );
        printBytes(app_header_n_data, sizeof(file_x_app_layer_t) + strlen(text_line[current_line]) -1);


        //SEND THE DATA: TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
        test=sendto( tx_socket_fd, 
                     app_header_n_data,                                             \
                     strlen(text_line[current_line])+sizeof(file_x_app_layer_t)-1,    \
                     0,                                                             \
                     (struct sockaddr *) &tx_to_address,                            \
                     sizeof(tx_to_address)                                          );

        if ( test < 0) printf("Failed to send line(%d).\n", current_line);
    }
    
    for(current_line=0; current_line<line_count; current_line++)
    {
        free(text_line[current_line]);
    }
    
  
    return 0;
}
