// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

//custom includes
#include <stdio.h>
#include "alloc.h"

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
/*
struct command_stream
{
    // Previous Command
    command* prev_command;

    // Current Command
    command* curr_command;

    // Next Command
    command* next_command;
};*/

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  printf("MAKING A COMMAND\n");

  // Creates Buffer
  int c,i;
  int size = sizeof(char) * 100;
  int curr_size = size;
  char *buffer = checked_malloc( curr_size );
  int count = 0; 
    do {
    if(count == curr_size)
    {
        curr_size += size;
        buffer = checked_realloc(buffer, curr_size);
    } 
    c=(*get_next_byte)(get_next_byte_argument);
    buffer[count] = c;
    count++;
  } while (c!=EOF);

  //error (1, 0, "command reading not yet implemented");
  printf("CHAR COUNT: %d\n",count);

  for(i = 0; i < count; i++)
  {
    printf("%c", buffer[i]);
  }
  return 0;
}

// Helper Methods
// Creates a buffer containing the input file
/*
int buffer_from_file(int (*get_next_byte) (void *), void *get_next_byte_argument, char &buff, int size)
{
  int c;
  int curr_size = size;
  int count = 0; 
    do {
    if(count == curr_size)
    {
        curr_size += size;
        buffer = checked_realloc(buffer, curr_size);
    } 
    c=(*get_next_byte)(get_next_byte_argument);
    buffer[count] = c;
    count++;
  } while (c!=EOF);
  return count;
} */

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
