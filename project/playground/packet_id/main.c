/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include "./packet_id.h"

int main (void)
{

    printf("packet_id=%d\npacket_id=0x%04x\n",packet_id());
    return 0;
}
