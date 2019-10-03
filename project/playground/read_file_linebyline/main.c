/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "readfile.h"

#define MAX_NUM_OF_FILES 10

int main (int argc, char **argv)
{
				
				FILE *fd;
				//fd = fopen("file.txt", "r");
				fd = fopen("file2.txt", "r");

				char ** array_of_text[MAX_NUM_OF_FILES];
				int num_of_lines;

				array_of_text[0]= getData(fd, &num_of_lines);

				int k=0;
				for (k=0; k < num_of_lines; k=k+1)
								printf("This Line: %s", array_of_text[0][k]);

				fclose(fd);

    return 0;
}
