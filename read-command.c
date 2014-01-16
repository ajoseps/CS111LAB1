// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

//custom includes
#include <stdio.h>
#include <stdbool.h>
#include "alloc.h"
#include "stack.h"

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


bool check_if_space(char);
bool add_to_buff_filtered(char*, char, int*, int*);
void add_to_buff_unfiltered(char*, char, int*, int*);
bool check_if_nonsimple(char);
bool check_if_open_paren(char);
bool check_if_closed_paren(char);

struct command_stream
{
    // Previous Command
    command_t* prev_command;

    // Current Command
    command_t* curr_command;

    // Next Command
    command_t* next_command;
};

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  printf("MAKING A COMMAND\n");

  // Creates Buffer
  int c, prevC;
  int curr_size = sizeof(char) * 25;
  int op_size = sizeof(char) * 4;
  char *buffer = checked_malloc( curr_size );
  char *op_buff = checked_malloc( op_size ); 
  int index = 0;
  int op_index = 0;
	bool prevEscape, prevQuote;
	bool addedToBuff;

	do {
    // For simple commands
		do {
			c=(*get_next_byte)(get_next_byte_argument);
			if(c=='\"')
	    {
				add_to_buff_unfiltered(buffer,c,&index,&curr_size);
		    c=(*get_next_byte)(get_next_byte_argument);
		    do {
		      prevEscape=false;
		      prevQuote=false;
		      add_to_buff_unfiltered(buffer,c,&index,&curr_size);
		      if(c=='\\')
		      {
			      prevEscape=true;
		      }
		      else if(c=='\"')
		      {
			      prevQuote=true;
		      }
		      c=(*get_next_byte)(get_next_byte_argument);
		    } while(prevEscape || !prevQuote);
	    }
	    addedToBuff=add_to_buff_filtered(buffer,c,&index,&curr_size);
	  } while(addedToBuff);
    // For non-simple commands
    // At this point, c contains a non-simple command or a portion of it
    add_to_buff_unfiltered(op_buff, c, &op_index, &op_size);
    prevC = c;
		c=(*get_next_byte)(get_next_byte_argument);
    if(!check_if_space(c))
    {
      if(prevC == c)
      {
        add_to_buff_unfiltered(op_buff, c, &op_index, &op_size);
      }
      else
      {
        error(1, 0, "invalid non-simple command");
        return 0;
      }
    }

    /* DO STUFF WITH THE BUFFERS HERE */
	} while (c!=EOF);

  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}

// HELPER FUNCTIONS

// Checks if c contains a space 
bool check_if_space(char c)
{
  if(c == ' ')
    return true;
  else
    return false;
}

// Adds c to buffer if c is non-simple else returns false
bool add_to_buff_filtered(char* buff, char c, int* index, int* curr_size)
{
  if(check_if_nonsimple(c))
  {
    return false;
  }
  else
  {
    add_to_buff_unfiltered(buff, c, index, curr_size);
    return true;
  }
}

// Adds c to buffer
void add_to_buff_unfiltered(char* buff, char c, int* index, int* curr_size)
{
  if(*index == *curr_size)
  {
    *curr_size += sizeof(char) * 25;
    buff = checked_realloc(buff, *curr_size);
  }
  buff[*index] = c;
  *index++;
}

// Checks if character is a or part of a non-simple command
bool check_if_nonsimple(char c)
{
  switch(c)
  {
    case '|':
    case '&':
    case ';':
    case '(':
    case ')':
      return true;
    default:
      return false;
  }
}

// Checks if character is an open paranthesis
bool check_if_open_paren(char c)
{
  if(c == '(')
    return true;
  else
    return false;
}

// Checks if character is a closed paranthesis
bool check_if_closed_paren(char c)
{
  if(c == ')')
    return true;
  else
    return false;
}
