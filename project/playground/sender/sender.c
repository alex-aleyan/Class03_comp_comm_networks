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
    arguments.dest_ip = "";
    arguments.source_port = "";
    arguments.dest_port = "";
    arguments.verbose = 0;
    arguments.debug   = 0;
  
    // Parse the Arguments:
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
  
    //Set the output stream:
    //FILE *outstream;
    //if (arguments.outfile) outstream = fopen (arguments.outfile, "w");
    //else                   outstream = stdout;
  
    //
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
  
    /* READ THE BINARY DATA FROM A FILE: */
    FILE *fd = NULL;
  
    /* Read the data into dataArray[]. */
    if ((fd=fopen(arguments.infile,"r"))==NULL) {
								fprintf(stderr, "Unable to open file:%s\nUse --input-file option, and make sure the file is present.\n", arguments.infile); 
								return -1;
    } 
  
    // Get number of lines:
    char current_char;
    int line_count=0;
    for (current_char = getc(fd), line_count=0; current_char != EOF; current_char = getc(fd))  if (current_char == '\n') line_count++;
				if (arguments.debug != 0) 	printf("\nNumber of lines (line_count): %d\n\n", line_count);
    char * line_ptr[line_count];
  
  		rewind(fd);
  
    // get number of chars per line:
    int char_count=0;
    int current_line=0;
    for (current_char = getc(fd), current_line=0; current_char != EOF; current_char = getc(fd))
    {
        char_count++;
        if (current_char == '\n')
        {
  								    // allocate just enough memory to hold the characters of current line:
												if (arguments.debug != 0) {
																printf("Number of char per line %d: %d\n", current_line, char_count);
																printf("Allocating: line=%d; bytes=%d: %d\n", current_line, char_count*sizeof(char)+1);
												}
  
            line_ptr[current_line] = malloc(char_count*sizeof(char)+1);
            if (line_ptr[current_line] == NULL) printf("Caught NULL at malloc!!!");
  
            char_count = 0;
            current_line++;
        }
    }
  
  		rewind(fd);
  
    printf("\n");
  
    //copy file content to allocated memory line by line one charachter at the time:
    for (current_char = getc(fd), current_line=0, char_count; current_char != EOF; current_char = getc(fd))
    {
								if (arguments.verbose != 0) {
												if (char_count == 0) printf("Line(%d):", current_line);
								}
  
        *(line_ptr[current_line]+char_count)=current_char;
        char_count++;
        if (current_char == '\n')
        {
            //terminate with NULL:
            *(line_ptr[current_line]+char_count)='\0';
												if (arguments.verbose != 0) printf("%s", line_ptr[current_line]);
            char_count=0;
            current_line++;
        }
    }
  
    printf("\n");
  
				if (arguments.debug != 0) { for(current_line=0; current_line<line_count; current_line++) printf("%s",line_ptr[current_line]); }
  
  		rewind(fd);
  
  
    //##################### UDP SENDER STARTS HERE:##################################
    
  
    struct sockaddr_in dest_address;
    memset(&dest_address, 0, sizeof(dest_address));
  
    dest_address.sin_family      = AF_INET; // Receiving Socket Family
    dest_address.sin_addr.s_addr = inet_addr(arguments.dest_ip);// Receiving Socket IP Address
    dest_address.sin_port        = htons(atoi(arguments.dest_port)); // Receiving Socket Port Number

				if (arguments.debug != 0) {
								printf( "dest_address.sin_family      =AF_INET\n");
								printf( "dest_address.sin_addr.s_addr =0x%08x\n", dest_address.sin_addr.s_addr);
								printf( "dest_address.sin_port        =0x%04x\n\n", dest_address.sin_port);
				}
    
    int tx_socket_fd = -1;
    //OPEN A SOCKET AND CATCH THE FD:
    tx_socket_fd     = socket(AF_INET,SOCK_DGRAM,0); 
    if (tx_socket_fd < 0) {perror("error: failed to open datagram socket\n"); exit(1); }
  
    //for(current_line=0; current_line<line_count; current_line++) printf("%s",line_ptr[current_line]);
  
    //Send data:
    int test;
    for(current_line=0, test=-1; current_line<line_count; current_line++)
    {
								if (arguments.debug != 0) {
												printf("strlen: %d\n", strlen(line_ptr[current_line]) );
								}

        //SEND THE DATA: TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
    				test=sendto( tx_socket_fd, 
                     line_ptr[current_line], \
                     strlen(line_ptr[current_line]), \
                     0, \
                     (struct sockaddr *) &dest_address, \
                     sizeof(dest_address)                   );

    				if ( test < 0) printf("Failed to send line(%d).\n", current_line);
    }
  
    
    for(current_line=0; current_line<line_count; current_line++)
    {
    				free(line_ptr[current_line]);
    }
    
  
    return 0;
}
