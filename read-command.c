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
#include <ctype.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#define NULL_TERMINATOR '\0'
#define EMPTY ''
#define TAB '\t'
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

struct command_stream{
  struct command_node *head;
  struct command_node *tail;
  struct command_node *curr;
};

typedef struct command_node{
  command_t command;
  struct command_node *next;
  struct command_node *prev;
} command_node_t;

// command_stream_t functions
command_stream_t command_stream_init()
{
  command_stream_t cs_t = checked_malloc(sizeof(command_stream_t));
  cs_t->head = NULL;
  cs_t->tail = NULL;
  cs_t->curr = NULL;
  return cs_t;
}

bool command_stream_add(command_stream_t command_s, command_t command)
{
  command_node_t *cs_node = checked_malloc(sizeof(command_node_t));
  cs_node->next = NULL;
  cs_node->prev = NULL;
  cs_node->command = command;

  if(command_s->head == NULL)
  {
    command_s->head = cs_node;
    command_s->tail = cs_node;
    command_s->curr = cs_node;
  }
  else
  {
    ((command_node_t*)(command_s->tail))->next = cs_node;
    cs_node->prev = command_s->tail;
    command_s->tail = cs_node;
    command_s->curr = cs_node;
  }
  return true;
}

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
  command_stream_t command_stream = command_stream_init();

  int index = 0;
  do{
      token = get_token(get_next_byte, get_next_byte_argument);

      char* buffer = token.buffer;
      int c = *buffer;

      if(isgraph(c))
      {
        print_token(token);
      }
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
  bool first = true;
  bool second = false;

  do {
    c=(*get_next_byte)(get_next_byte_argument);

    if(c == NEWLINE)
    {
      break;
    }
    
    // RESIZING
    if(index == curr_size)
    {
      curr_size += sizeof(char) * 25;
      buffer = checked_realloc(buffer, curr_size);
    }
    
    // IF FIRST CHARACTER IS SPECIAL
    if(first == true && is_special_char(c))
    {
      buffer[index] = c;
      index++;
      second = true;
    }
    else if(first == false && second == true)
    {
        if(is_special_char(c))
        {
          buffer[index] = c;
          index++;
          second = false;
          break;
        }
        else
        {
          ungetc(c,get_next_byte_argument);
          second = false;
          break;
        }
    }
    else
    {
      second = false;
      // IF A SPACE IS FOUND
      if(c == SPACE && quotes == false && comments == false)
      {
        break;
      }

      // IF A SPECIAL CHARACTER IS FOUND
      if(is_special_char(c) == true && quotes == false && comments == false)
      {
        ungetc(c,get_next_byte_argument);
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
    }
    first = false;
  } while(c!=NEWLINE && c!=NULL_TERMINATOR && c!= EOF);

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
