/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include "../parse_udp_args/parse_args.h"
#include <arpa/inet.h>  //inet_aton
#include <netinet/in.h> //sockaddr_in
//#include "../source/func.h"
#include "../read_file_linebyline/readfile.h"

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
				}

				//Make sure dest ip and dest port are provided via options:
    if (arguments.dest_ip == "" || arguments.dest_port == "") {
								fprintf(stderr, "Make sure to provide --dest-ip and --dest-port\nUse --help for more information.\n"); 
								return -1;
    } 
  
    struct sockaddr_in adr_inet;
  
    //Check the IP Addresses to be 4 ascii octets ("127.0.0.1"):
    if ( !inet_aton(arguments.dest_ip, &adr_inet.sin_addr) ) { printf("BAD ADDRESS %s\n", arguments.dest_ip); return -1;}
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
  
    //Send data:
    int test;
    for(current_line=0, test=-1; current_line<line_count; current_line++)
    {
								if (arguments.debug != 0) {
												printf("strlen: %d\n", strlen(text_line[current_line]) );
								}

        //SEND THE DATA: TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
    				test=sendto( tx_socket_fd, 
                     text_line[current_line],             \
                     strlen(text_line[current_line]),     \
                     0,                                  \
                     (struct sockaddr *) &tx_to_address, \
                     sizeof(tx_to_address)                   );

    				if ( test < 0) printf("Failed to send line(%d).\n", current_line);
    }
  
    
    for(current_line=0; current_line<line_count; current_line++)
    {
    				free(text_line[current_line]);
    }
    
  
    return 0;
}
