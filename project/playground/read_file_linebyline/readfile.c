/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include <stdlib.h>




int main (int argc, char **argv)
{
				
				FILE *fp;

				fp = fopen("file.txt", "r");




				int line_count = 0;
				int char_count = 0;
				char current_char;

				//count the number of lines in file to determine the size of the pointer array:
				for (current_char = getc(fp); current_char != EOF; current_char = getc(fp))	
								if (current_char == '\n') line_count++;

				printf("\nNumber of lines: %d\n\n", line_count);

				//allocate an array of pointers (1 pointer per line of text in file):
				char * line_ptr[line_count];

				// determine number of chars per each line and allocate just enough memory per each line of text:
				rewind(fp);
				for (current_char = getc(fp), line_count=0; current_char != EOF; current_char = getc(fp))	
				{
								char_count++;
								if (current_char == '\n') 
								{
												// allocate just enough memory to hold the characters of current line:
												line_ptr[line_count] = malloc(char_count*sizeof(char));
												printf("Number of char per line %d: %d\n", line_count, char_count);
												char_count = 0;
												line_count++;
								}
				}

				printf("\n");

				rewind(fp);

				//line_count=0;
				//char current_line [255];
				//while( fgets(current_line, 255, (FILE*) fp) )
				//{
				//				strcpy(line_ptr[line_count], current_line);
				//				printf("%s", line_ptr[line_count]);
				//				line_count++;
			 //}
			 
				//copy file content to allocated memory line by line
				//this is slower than the commented out section above since reading
				//one character at the time but we don't have to allocate additional 255
				//bytes of data
				line_count=0;
				char_count=0;
				for (current_char = getc(fp); current_char != EOF; current_char = getc(fp))	
				{
								*(line_ptr[line_count]+char_count)=current_char;
								char_count++;
								if (current_char == '\n') 
								{
												printf("Line(%d): ", line_count);
												printf("%s", line_ptr[line_count]);
												char_count=0;
												line_count++;
								}
				}

				fclose(fp);

    return 0;
}
