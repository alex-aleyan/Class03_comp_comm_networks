/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../source/readfile.h"

#define MAX_NUM_OF_FILES 10

int main (int argc, char **argv)
{
				
				FILE *fd;
				//fd = fopen("file.txt", "r");
				fd = fopen("file.txt", "r");
       
   

				char * array_of_text[getLinesPerFile(fd)];
				int num_of_lines;

				getData(fd, &num_of_lines, 1, array_of_text);
        printf("num_of_lines: %d", num_of_lines);
				int k=0;
				for (k=0; k < num_of_lines; k=k+1)
								printf("This Line: %s", array_of_text[k]);

				fclose(fd);

    return 0;
}
