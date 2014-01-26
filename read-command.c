// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

//CUSTOM INCLUDES
#include "alloc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#define NULL_TERMINATOR '\0'
#define SPACE ' '
#define QUOTE '\"'
#define AMPER '&'
#define GREATER '>'
#define LESSER '<'
#define PIPE '|'
#define SEMICOLON ';'
#define COLON ':'
#define POUND '#'
#define OPEN_P '('
#define CLOSE_P ')'
#define NEWLINE '\n'

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

enum token_type {
  AND_T,         // A && B
  SEQUENCE_T,    // A ; B
  OR_T,          // A || B
  PIPE_T,        // A | B
  REGULAR_T,      // A token that is not a special symbol
  OPEN_PARENS_T,  // (
  CLOSE_PARENS_T, // )
  NULL_T,        // \0
  GREATER_T,     // >
  LESS_T,       // <
  QUOTE_T,      // "
  COMMENT_T     // #
};

typedef struct{
  char* buffer;
  int size;
  enum token_type type;
} token_t;

token_t
get_token(int (*get_next_byte) (void *),
        void *get_next_byte_argument);

void 
print_token(token_t token);

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  token_t token;
  do{
    token = get_token(get_next_byte, get_next_byte_argument);
    print_token(token);
  }while(*token.buffer != EOF);
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}


bool 
is_special_char(char c);

token_t 
get_token(int (*get_next_byte) (void *),
        void *get_next_byte_argument)
{
  int curr_size=sizeof(char)*50;
  int index=0;
  char* buffer = checked_malloc( curr_size );
  int c;
  // FLAGS
  bool quotes = false;
  bool comments = false;

  do {
    c=(*get_next_byte)(get_next_byte_argument);
    
    // RESIZING
    if(index == curr_size)
    {
      curr_size += sizeof(char) * 25;
      buffer = checked_realloc(buffer, curr_size);
    }

    // IF A SPACE IS FOUND
    if(c == SPACE && quotes == false && comments == false)
    {
      break;
    }

    // IF A SPECIAL CHARACTER IS FOUND
    if(is_special_char(c) == true && quotes == false && comments == false)
    {
      get_next_byte_argument--;  
      break;
    }

    // START OF QUOTE
    if(c == QUOTE && quotes == false && comments == false)
    {
      quotes = true;
    }
    // END OF QUOTE
    else if(c == QUOTE && quotes == true) 
    {
      buffer[index] = c;
      index++;
      break;
    }

    // START OF COMMENT
    if(c == POUND && comments == false)
    {
      comments = true;
    }
    // END OF COMMENT
    else if(c == NEWLINE && comments == true)
    {
      break;
    }

    buffer[index] = c;
    index++;
  } while(c!=NULL_TERMINATOR && c!= EOF);

  token_t token;
  token.size = index + 1;
  token.type = REGULAR_T; 
  token.buffer = checked_malloc( token.size );
  memcpy(token.buffer, buffer, (size_t)token.size);

  return token;
}

// Prints out the contents of the token's buffer
void 
print_token(token_t token)
{
  char *buffer = token.buffer;
  int c = *buffer;
  int i;
  for(i = 0; i < token.size - 1; i++)
  {
    printf("%c", c);
    c= *++buffer;
  }
  printf("\n");
}

// Returns true if c contains a special character, false otherwise
// DOES NOT CONTAIN QUOTE AND SPACE AND POUND
bool 
is_special_char(char c)
{
  switch(c)
  {
    case OPEN_P:
    case CLOSE_P:
    case SEMICOLON:
    case COLON:
    case PIPE:
    case AMPER:
    case LESSER:
    case GREATER:
      return true;
    default:
      return false;
  }
}

/*
void
set_token_type(token_t &token)
{

}*/
