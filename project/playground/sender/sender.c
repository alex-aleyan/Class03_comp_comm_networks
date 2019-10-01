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
#include "../source/func.h"


//#define SERVER_SOCKET_IP "192.168.40.185"
//#define SERVER_SOCKET_IP "127.0.0.1"
//#define SERVER_SOCKET_PORT 1026
#define READFILE "./file.txt"

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
  if (arguments.outfile) outstream = fopen (arguments.outfile, "w");
  else                   outstream = stdout;

  /* Print argument values */
  fprintf (outstream, "\n");
  fprintf (outstream, "source-ip:   %s\n",   arguments.source_ip);
  fprintf (outstream, "source-port: %s\n\n", arguments.source_port);
  fprintf (outstream, "dest-ip:     %s\n",   arguments.dest_ip);
  fprintf (outstream, "dest-port:   %s\n\n", arguments.dest_port);
//  fprintf (outstream, "ARG1 = %s\n",         arguments.args[0]);
//  fprintf (outstream, "ARG2 = %s\n\n",       arguments.args[1]);

  /* If in verbose mode, print song stanza */
  if (arguments.verbose) fprintf (outstream, waters);

  struct sockaddr_in adr_inet;

  if ( !inet_aton(arguments.source_ip, &adr_inet.sin_addr) )  puts ("BAD ADDRESS");
  fprintf(outstream, "The arguments.source_ip=%s\n",   arguments.source_ip);
  fprintf(outstream, "The arguments.source_ip=0x%08x\n\n", adr_inet.sin_addr);

  fprintf(outstream, "The arguments.source_port=%d\n", atoi(arguments.source_port) );
	 fprintf(outstream, "The arguments.source_port=0x%04x\n\n", htons(atoi(arguments.source_port)) );


  if ( !inet_aton(arguments.dest_ip, &adr_inet.sin_addr) )  puts ("BAD ADDRESS");
  fprintf(outstream, "The arguments.dest_ip=%s\n",   arguments.dest_ip);
  fprintf(outstream, "The arguments.dest_ip=0x%08x\n\n", adr_inet.sin_addr);

  fprintf(outstream, "The arguments.dest_port=%d\n", atoi(arguments.dest_port) );
	 fprintf(outstream, "The arguments.dest_port=0x%04x\n", htons(atoi(arguments.dest_port)) );


  //##################### OPEN FILE STARTS HERE:

  /* READ THE BINARY DATA FROM A FILE: */
  FILE *fd = NULL;
  /* Read the data into dataArray[]. */
  //if ((fd=fopen(READFILE,"r"))==NULL) {fprintf(stderr, "Unable to open file %s", READFILE); return 1;} 
  if ((fd=fopen(arguments.infile,"r"))==NULL) {fprintf(stderr, "Unable to open file %s", READFILE); return 1;} 
  fseek(fd, 0, SEEK_END); // go to the end of file
  unsigned long fileLen=ftell(fd); //determine the length of file in bytes

  #ifdef SHOW_FD
    printf("fileLen (bytes): %d\n", fileLen);
  #endif
  
  fseek(fd, 0, SEEK_SET);
  unsigned char sendDataBuf[fileLen];
  fread(sendDataBuf, fileLen , 1, fd);
 
  #ifdef SHOW_DATA
    unsigned char * charPtr=(unsigned char*) sendDataBuf;
    printf("The data read from %s: ", READFILE);
    int i; for(i=0; i<fileLen; i++) printf("0x%02x ", *(charPtr+i) );
    printf("\n");
  #endif
  fclose(fd);

  //##################### UDP SENDER STARTS HERE:

  
  
  char txDgramBuffer[512]="Hello World"; //DATA

  struct sockaddr_in rxAddress;
  memset(&rxAddress, 0, sizeof(rxAddress));
  rxAddress.sin_family = AF_INET; // Receiving Socket Family
  //rxAddress.sin_addr.s_addr = inet_addr(SERVER_SOCKET_IP);// Receiving Socket IP Address
  //rxAddress.sin_port = htons(SERVER_SOCKET_PORT); // Receiving Socket Port Number
  rxAddress.sin_addr.s_addr = inet_addr(arguments.dest_ip);// Receiving Socket IP Address
  rxAddress.sin_port = htons(atoi(arguments.dest_port)); // Receiving Socket Port Number
  
  //Establish a socket connection
  int txSocket = -1;
  txSocket = socket(AF_INET,SOCK_DGRAM,0); 

  int test = -1;
  //TX Socket, TX Data, TX Data Size, flags, RX Socket Info (DOMAIN, IP and PORT), size of RX Address Struct)
  /*test=sendto(txSocket, txDgramBuffer, strlen(txDgramBuffer),\ */
  test=sendto(txSocket, sendDataBuf, strlen(sendDataBuf),\
			  0, (struct sockaddr *) &rxAddress, sizeof(rxAddress) );
  if ( test < 0) printf("Failed to send\n");
  
  #ifdef BYTES
  printBytes( (unsigned char *) &rxAddress, 16);
  #endif
  
  return 0;
}
