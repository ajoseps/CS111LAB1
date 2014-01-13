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

  int c;
  int count=0;

  int size = sizeof(char) * 100;
  int curr_size = size;

  char *buffer = checked_malloc( curr_size );

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
  printf("%d",count);
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
