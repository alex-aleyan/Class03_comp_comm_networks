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




  //##################### PARSE ARGS STARTS HERE:

  struct arguments arguments;
  FILE *outstream;
  char waters[] = "VERBOSEVERBOSEVERBOSEVERBOSE \n";

  /* Set argument defaults */
  arguments.outfile = NULL;
  arguments.infile = NULL;
  arguments.source_ip = "";
  arguments.dest_ip = "";
  arguments.source_port = "";
  arguments.dest_port = "";
  arguments.verbose = 0;

  /* Where the magic happens */
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  /* Where do we send output? */
  //if (arguments.outfile) outstream = fopen (arguments.outfile, "w");
  //else                   outstream = stdout;
  outstream = stdout;

  /* Print argument values */
  printf ( "\n");
  printf ( "source-ip:   %s\n",   arguments.source_ip);
  printf ( "source-port: %s\n\n", arguments.source_port);
  printf ( "dest-ip:     %s\n",   arguments.dest_ip);
  printf ( "dest-port:   %s\n\n", arguments.dest_port);
//  printf ( "ARG1 = %s\n",         arguments.args[0]);
//  printf ( "ARG2 = %s\n\n",       arguments.args[1]);

  /* If in verbose mode, print song stanza */
  if (arguments.verbose) printf ( waters);

  struct sockaddr_in adr_inet;

  if ( !inet_aton(arguments.dest_ip, &adr_inet.sin_addr) )  puts ("BAD ADDRESS");
  printf( "The arguments.dest_ip=%s\n",   arguments.dest_ip);
  printf( "The arguments.dest_ip=0x%08x\n\n", adr_inet.sin_addr);

  printf( "The arguments.dest_port=%d\n", atoi(arguments.dest_port) );
	 printf( "The arguments.dest_port=0x%04x\n", htons(atoi(arguments.dest_port)) );


  //##################### OPEN FILE STARTS HERE:

  /* READ THE BINARY DATA FROM A FILE: */
  FILE *fd = NULL;

  /* Read the data into dataArray[]. */
  //if ((fd=fopen(READFILE,"r"))==NULL) {printf(stderr, "Unable to open file %s", READFILE); return 1;} 
  if ((fd=fopen(arguments.infile,"r"))==NULL) {printf(stderr, "Unable to open file %s", arguments.infile); return 1;} 

  // Get number of lines:
  char current_char;
  int line_count=0;
  for (current_char = getc(fd), line_count=0; current_char != EOF; current_char = getc(fd))  if (current_char == '\n') line_count++;
  printf("\nNumber of lines (line_count): %d\n\n", line_count);
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
          printf("Number of char per line %d: %d\n", current_line, char_count);
          printf("Allocating: line=%d; bytes=%d: %d\n", current_line, char_count*sizeof(char)+1);

          line_ptr[current_line] = malloc(char_count*sizeof(char)+1);
          if (line_ptr[current_line] == NULL) printf("Caught NULL at malloc!!!");

          char_count = 0;
          current_line++;
      }
  }

		rewind(fd);

  printf("\n");

  //copy file content to allocated memory line by line
  for (current_char = getc(fd), current_line=0, char_count; current_char != EOF; current_char = getc(fd))
  {
      if (char_count == 0) printf("Line(%d):", current_line);

      *(line_ptr[current_line]+char_count)=current_char;
      char_count++;
      if (current_char == '\n')
      {
          *(line_ptr[current_line]+char_count)='\0';
          printf("%s", line_ptr[current_line]);
          char_count=0;
          current_line++;
      }
  }

  printf("\n");

  for(current_line=0; current_line<line_count; current_line++) printf("%s",line_ptr[current_line]);

		rewind(fd);


  //##################### UDP SENDER STARTS HERE:
  

  struct sockaddr_in rxAddress;
  memset(&rxAddress, 0, sizeof(rxAddress));

  rxAddress.sin_family      = AF_INET; // Receiving Socket Family
  rxAddress.sin_addr.s_addr = inet_addr(arguments.dest_ip);// Receiving Socket IP Address
  rxAddress.sin_port        = htons(atoi(arguments.dest_port)); // Receiving Socket Port Number
  
  int txSocket = -1;
  //OPEN A SOCKET AND CATCH THE FD:
  txSocket     = socket(AF_INET,SOCK_DGRAM,0); 
  if (txSocket < 0) {perror("error: failed to open datagram socket\n"); exit(1); }

  //for(current_line=0; current_line<line_count; current_line++) printf("%s",line_ptr[current_line]);

  //Send data:
  int test;
  for(current_line=0, test=-1; current_line<line_count; current_line++)
  {
    printf("STRLEN: %d\n", strlen(line_ptr[current_line]) );
    //SEND THE DATA: TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
				test=sendto(txSocket, line_ptr[current_line], strlen(line_ptr[current_line]), 0, (struct sockaddr *) &rxAddress, sizeof(rxAddress) );
				//test=write(txSocket, line_ptr[current_line], strlen(line_ptr[current_line]) );
				if ( test < 0) printf("Failed to send\n");
  }


  
  for(current_line=0; current_line<line_count; current_line++)
  {
				free(line_ptr[current_line]);
  }
  

  return 0;
}
