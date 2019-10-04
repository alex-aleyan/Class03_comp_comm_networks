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
    char waters[] = "VERBOSEVERBOSEVERBOSEVERBOSE \n";

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

#define MAX_NUM_OF_TEXT_LINES_PER_FILE 10

    int testToBail;
    struct sockaddr_in rx_local_address; // AF_INET
    struct sockaddr_in rx_from_address; // AF_INET
    int rxSocket;
    char receiveDgramBuffer[MAX_NUM_OF_TEXT_LINES_PER_FILE][512];	// Receive Buffer
    
    //OPEN A SOCKET AND CATCH THE FD:
    rxSocket = socket(AF_INET,SOCK_DGRAM,0);
    if (rxSocket == -1) bail("Failed to create a socket; see line: rxSocket=socket(AF_INET,SOCKET_DGRAM,0);");
    
    memset(&rx_local_address, 0, sizeof(rx_local_address));
    rx_local_address.sin_family = AF_INET;
    rx_local_address.sin_addr.s_addr = inet_addr(arguments.source_ip);  //rx_local_address.sin_addr.s_addr = inet_addr(SERVER_SOCKET_IP);

    if (rx_local_address.sin_addr.s_addr == INADDR_NONE) bail("bad source address");
    rx_local_address.sin_port = htons(atoi(arguments.source_port));
    
    //inetSocketLength = sizeof(rx_local_address);
    printf("FD returned by socket(): %d\n", rxSocket);
    
    //BIND THE SOCKET TO A GIVEN IP/PORT:
    testToBail = bind(rxSocket, (struct sockaddr *) &rx_local_address, sizeof(rx_local_address));
    if (testToBail == -1) bail("bind()");
    
    //system("netstat -neopa | grep Recv-Q ");
    //system("netstat -neopa | grep dgram ");
    
    int txSockLen;
    txSockLen = sizeof(rx_from_address);
    

    int current_line;
    for(current_line=0;current_line<MAX_NUM_OF_TEXT_LINES_PER_FILE;current_line++)
    {
      //RECEIVE THE DATA:
      //testToBail = recvfrom(rxSocket, receiveDgramBuffer, sizeof(receiveDgramBuffer), 0, (struct sockaddr *) &rx_from_address, &txSockLen);
      testToBail = read(rxSocket, receiveDgramBuffer[current_line], sizeof(receiveDgramBuffer) );
      if ( testToBail < 0) bail("recvfrom(2)");
      receiveDgramBuffer[current_line][testToBail] = '\0'; //NULL terminate the received string

      printf("RECEIVED DGRAM:\n%s", receiveDgramBuffer[current_line]);
      printf("RECEIVED FROM: %s:%u\n\n", inet_ntoa(rx_from_address.sin_addr), (unsigned) ntohs(rx_from_address.sin_port));
    }

    for(current_line=0;current_line<MAX_NUM_OF_TEXT_LINES_PER_FILE;current_line++)	printf("%s", receiveDgramBuffer[current_line]);

    shutdown(rxSocket, SHUT_RDWR);

    //##################### OPEN FILE STARTS HERE: ##########################

    FILE *fd = NULL;
    if ( (fd=fopen(arguments.outfile,"w")) == NULL ) {
								printf(stderr, "Unable to open file %s; Use --input-file option, and make sure the file is present.\n", arguments.outfile); 
								return -1; }
				else  {
      fflush(fd);
      for(current_line=0;current_line<MAX_NUM_OF_TEXT_LINES_PER_FILE;current_line++)
        fputs(receiveDgramBuffer[current_line], fd);
      fclose(fd);
    }  
    
    
    return 0;
}
  
  
