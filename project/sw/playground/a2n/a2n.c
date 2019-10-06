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

#define NUMBER_OF_STRINGS 4
#define BYTES_PER_STRING 16

int main (int argc, char **argv)
{
  struct sockaddr_in adr_inet;
  //const char *ip_address_ascii="255.240.15.255";
  //const char *ip_address_ascii="240.255.15.0";
  char ip_address_ascii[NUMBER_OF_STRINGS][BYTES_PER_STRING]= { "240.255.15.0",
																												     "255.240.15.255",
																												     "255.255.255.255",
																												     "0.255.0.255"				   };
  uint16_t port_ascii[NUMBER_OF_STRINGS]= { 900,
																												     1025,
																												     1026,
																												     255};
		int i;	

  printf("\n\n");
  for (i=0; i < NUMBER_OF_STRINGS; i++)
  { 	
				if ( !inet_aton(ip_address_ascii[i],&adr_inet.sin_addr) )  puts ("BAD ADDRESS");
				printf("The %s=0x%08x\n", ip_address_ascii[i], adr_inet.sin_addr);
  }
  printf("\n\n");

  printf("\n\n");
  for (i=0; i < NUMBER_OF_STRINGS; i++)
  { 	
				printf("The port[%d]=%d\n", i, port_ascii[i] );
				printf("The port[%d]=0x%04x\n", i, htons(port_ascii[i]) );
  }
  printf("\n\n");

  return 0;
}
