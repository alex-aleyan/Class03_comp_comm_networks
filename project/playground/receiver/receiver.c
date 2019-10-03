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
#define SERVER_SOCKET_PORT 9090


int main (int argc, char **argv)
{
  struct arguments arguments;
  FILE *outstream;
  char waters[] = "VERBOSEVERBOSEVERBOSEVERBOSE \n";

  /* Set argument defaults */
  arguments.outfile = NULL;
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


  //##################### UDP RECEIVER STARTS HERE:

  int testToBail;
  struct sockaddr_in rxAddress; // AF_INET
  struct sockaddr_in txAddress; // AF_INET
  int rxSocket;
  char receiveDgramBuffer[512];	// Receive Buffer
  
  //OPEN A SOCKET AND CATCH THE FD:
  rxSocket = socket(AF_INET,SOCK_DGRAM,0);
  if (rxSocket == -1) bail("Failed to create a socket; see line: rxSocket=socket(AF_INET,SOCKET_DGRAM,0);");
  
  memset(&rxAddress, 0, sizeof(rxAddress));
  rxAddress.sin_family = AF_INET;
  //rxAddress.sin_addr.s_addr = inet_addr(SERVER_SOCKET_IP);
  rxAddress.sin_addr.s_addr = inet_addr(arguments.source_ip);
  if (rxAddress.sin_addr.s_addr == INADDR_NONE) bail("bad source address");
  rxAddress.sin_port = htons(atoi(arguments.source_port));
  
  //inetSocketLength = sizeof(rxAddress);
  printf("FD returned by socket(): %d\n", rxSocket);
  
  //BIND THE SOCKET TO A GIVEN IP/PORT:
  testToBail = bind(rxSocket, (struct sockaddr *) &rxAddress, sizeof(rxAddress));
  if (testToBail == -1) bail("bind()");
  
  //system("netstat -neopa | grep Recv-Q ");
  //system("netstat -neopa | grep dgram ");
  
  int txSockLen;
  txSockLen = sizeof(txAddress);
  
  for(;;)
  {
    //RECEIVE THE DATA:
    //testToBail = recvfrom(rxSocket, receiveDgramBuffer, sizeof(receiveDgramBuffer), 0, (struct sockaddr *) &txAddress, &txSockLen);
    testToBail = read(rxSocket, receiveDgramBuffer, sizeof(receiveDgramBuffer) );
    if ( testToBail < 0) bail("recvfrom(2)");
    receiveDgramBuffer[testToBail] = 0; //NULL terminate the received string
    printf("RECEIVED DGRAM:\n%s", receiveDgramBuffer);
    printf("RECEIVED FROM: %s:%u\n\n", inet_ntoa(txAddress.sin_addr), (unsigned) ntohs(txAddress.sin_port));
  #ifdef BYTES
    printBytes( (unsigned char *) &txAddress, 16);
  #endif
  }
  
   //testToBail = sendto(rxSocket, sendDgramBuffer, strlen(sendDgramBuffer), 0, (struct sockaddr *) &txAddress, inetSocketLength );
  
  shutdown(rxSocket, SHUT_RDWR);
  
  return 0;
}
  
  
