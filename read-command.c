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
#define TAB '\t'

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

// stacks/arrays for postfix evaluation

// ALL STACK FUNCTIONS ARE REPURPOSED FROM THE INCLUDED STACK FILES THAT WERE TAKEN FROM
// Robert I. Pitts <rip@cs.bu.edu>

typedef struct{
  command_t *contents;
  int maxSize;
  int top;
}stackT;

void StackInit(stackT *stackP, int maxSize)
{
  command_t* newContents;

  newContents = (command_t*)checked_malloc(sizeof(command_t) * maxSize);

  if(newContents == NULL)
  {
    fprintf(stderr, "insufficent memory");
    exit(1);
  }
  stackP->contents=newContents;
  stackP->maxSize = maxSize;
  stackP->top = -1;
}

void StackDestroy(stackT *stackP)
{
  free(stackP->contents);
  stackP->contents=NULL;
  stackP->maxSize=0;
  stackP->top=-1;
}

int StackIsEmpty(stackT *stackP)
{
  return stackP->top < 0;
}

int StackIsFull(stackT *stackP)
{
  return stackP->top >= stackP->maxSize -1;
}

void StackPush(stackT *stackP, command_t element)
{
  if(StackIsFull(stackP))
  {
    fprintf(stderr, "Can't push element on stack: stack is full \n");
    exit(1);
  }
  stackP->contents[++stackP->top] = element;
}

command_t StackPop(stackT *stackP)
{
  if(StackIsEmpty(stackP))
  {
    fprintf(stderr, "Can't pop, stack is empty");
    exit(1);
  }
  return stackP->contents[stackP->top--];
}

stackT opStack;
stackT evalStack;
int pfSize=100;
command_t *postfix[100];
int pfIndex=0;

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
  command_stream_t cs_t = checked_malloc(sizeof(struct command_stream));
  cs_t->head = NULL;
  cs_t->tail = NULL;
  cs_t->curr = NULL;
  return cs_t;
}

bool command_stream_add(command_stream_t command_s, command_t command)
{
  command_node_t *cs_node = checked_malloc(sizeof(struct command_node));
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

void
make_command(token_t token, command_t command);

token_t
get_token(int (*get_next_byte) (void *),
        void *get_next_byte_argument);

void 
print_token(token_t token);

void 
concat_buffs(char* dest, char* src);

void
add_to_array(command_t** arr, command_t* element);

void
add_to_postfix(command_t* element);

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  StackInit(&opStack, 500);
  token_t token;

  command_stream_t command_stream = command_stream_init();

  int index = 0;
  do{
    command_t command;
    command = checked_malloc(sizeof(struct command));
    bool lastSimple = false;
    do{
      token = get_token(get_next_byte, get_next_byte_argument);

      char* buffer = token.buffer;
      int c = *buffer;

      if(isgraph(c))
      {
// make_command
        char *buffer = token.buffer;
        int c = *buffer;
        int i;

        command->status = -1;
        if(strcmp(buffer,"&&")==0)
        {
          printf("AND\n"); 
          command->type=AND_COMMAND;
        }
        else if(strcmp(buffer,"||")==0)
        {
          printf("OR\n");
          command->type=OR_COMMAND;
        }
        else if(strcmp(buffer,";")==0)
        {
          printf("SEMICOLON\n");
          command->type=SEQUENCE_COMMAND;
        }
        else if(strcmp(buffer,"|")==0)
        {
          printf("PIPE\n");
          command->type=PIPE_COMMAND;
        }
        else if(strcmp(buffer,"(")==0 || strcmp(buffer,")")==0)
        {
          printf("PARENTHESIS\n");
          command->type=SUBSHELL_COMMAND;
        }
        else if(buffer[0] == '#')
        {
          continue;
        }
        else
        {
          command->type=SIMPLE_COMMAND;
          command->u.word = &buffer;
          //printf("This is u.word in make_command: %s \n", *(command->u.word));
        }
//
        if(command->type==SIMPLE_COMMAND)
        {
          if(lastSimple)
          {
            command_t* tmp = postfix[pfIndex-1];
            char** tmp_buff = (*tmp)->u.word;
            char** cmd_buff =  command->u.word;
            concat_buffs(*tmp_buff,*cmd_buff);

            printf("tmp_buff contains: %s \n", *tmp_buff);
            printf("concated buffs\n");
          }
          else
          {
            lastSimple = true;
            add_to_postfix(&command);
          }          
        }
        else if(command->type==SUBSHELL_COMMAND)
        {
          printf("SIMPLE COMMAND\n");
          lastSimple = false;
        }
        else //normal operator
        {
          printf("NORMAL OPERATOR\n");
          lastSimple = false;

          if(StackIsEmpty(&opStack))
          {
            StackPush(&opStack,command);
          }
          else
          {
            //pop into array until empty or something of higher precedence
            StackPop(&opStack);
          }
        }
      }
      else if(c == NEWLINE)
      {
        break;
      }
    }while(*token.buffer != EOF);
    command_stream_add(command_stream, command);
  }while(*token.buffer != EOF);
  
  return command_stream;
}

command_t
read_command_stream (command_stream_t s)
{
  if(s->head == NULL)
  {
    //there are no commands in stream
    error(1, 0, "command stream is empty");
    return 0;
  }
  else
  {
    command_node_t *tmp = s->head;
    s->head = s->head->next;
    
    command_t currCommand = tmp->command;
    free(tmp);
    return currCommand;
  }

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

void 
concat_buffs(char* dest, char* src)
{
  char* c_buff;
  char* space_c = (char*) checked_malloc(1);
  *space_c = SPACE;
  size_t newSize = strlen(dest) + strlen(src) + 1;
  c_buff = (char*)checked_malloc(newSize);

  strcpy(c_buff, dest);
  strcat(c_buff, space_c);
  strcat(c_buff, src);

  dest = (char*)checked_realloc(dest, newSize);

  memcpy(dest, c_buff,newSize);

  free(c_buff);
  free(space_c);
}

void
add_to_array(command_t** arr, command_t* element)
{
  int i,tmpIndex;
  command_t* tmp = NULL;
  if(pfIndex==pfSize)
  {
    command_t** newArr = checked_malloc( pfSize + 50);
    for(i = 0; i < pfSize; i++)
    {
      printf("%i\n",i);
      memcpy(tmp, arr[i], sizeof(struct command));
      newArr[i] = tmp;
    }
    arr = newArr;
  }
  tmp = checked_malloc( sizeof(struct command) );
  memcpy(tmp, element, sizeof(struct command));
  arr[pfIndex] = tmp;
  pfIndex++;
}

void
add_to_postfix(command_t* element)
{
  add_to_array(postfix, element);
}
