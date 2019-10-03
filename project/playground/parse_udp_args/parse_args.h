/* 
 *
 * Citing Sources: 
 *
 * https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html 
 * 
 */

#include <stdio.h>
#include <argp.h>


const char *argp_program_version = "argex 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";


struct arguments
{
  char *args[2];            /* ARG1 and ARG2 */
  int verbose;              /* The -v flag */
  char *infile, *outfile;            /* Argument for -o */
  char *source_ip, *source_port;
  char *dest_ip,   *dest_port;  /* Arguments for -a and -b */
};


static struct argp_option options[] =
{
  {"verbose",        'v',											    0, 0, "Produce verbose output"},
  {"source-ip",      's', "<XX.XX.XX.XX>", 0, "Provide source IP address of you NIC"},
  {"source-port",    'p', "<XXXX>"						 , 0, "Provide source port number > 1025"},
  {"dest-ip",        'd', "<XX.XX.XX.XX>", 0, "Provide Destination IP/hostname"},
  {"dest-port",      'b', "<XXXX>"						 , 0, "Provide destination port number"},
  {"output-file",         'o', "OUTFILE"					 , 0, "Output to OUTFILE instead of to standard output"},
  {"input-file",     'f', "INFILE"					  , 0, "Specify file name to get data from"},
  {0}
};


static char args_doc[] = "ARG1 ARG2";
static char doc[] = "Heading for the --help version\n";


static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'v':
      arguments->verbose = 1;
      break;
    case 's':
      arguments->source_ip = arg;
      break;
    case 'd':
      arguments->dest_ip = arg;
      break;
    case 'p':
      arguments->source_port = arg;
      break;
    case 'b':
      arguments->dest_port = arg;
      break;
    case 'o':
      arguments->outfile = arg;
      break;
    case 'f':
      arguments->infile = arg;
      break;
    case ARGP_KEY_ARG:
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}


static struct argp argp = {options, parse_opt, args_doc, doc};
