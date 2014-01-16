// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

//custom includes
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "alloc.h"
#include "stack.h"

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


void print_cstring(char*);
int precedence_value(char*);
bool precedence(char*, char*);
bool check_if_space(char);
bool add_to_buff_filtered(char*, char, int*, int*);
void add_to_buff_unfiltered(char*, char, int*, int*);
bool check_if_nonsimple(char);
bool check_if_open_paren(char);
bool check_if_closed_paren(char);
bool infix_to_postfix(char*, char*);
void add_to_postfix(char*);
void add_to_array(char**, char*, int*);

// Globals
char *postfix[100];
int pfIndex=0;

stackT opStack;


//Command Stream

struct command_stream
{
    command_t* prev_command;
    command_t* curr_command;
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

  //TESTING VARS
  int i;

  // Initializes Stack
  StackInit(&opStack, 100);

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
        printf("%c\n", prevC);
        printf("%c\n", c);

        error(1, 0, "invalid non-simple command");
        return 0;
      }
    }
    
    // Add simple command into postfix array
    add_to_postfix(buffer);

    // Push operator into op buffer (if empty)
    if(StackIsEmpty(&opStack))
    {
      StackPush(&opStack, op_buff);
    }
    // If stack is not empty, pop operators according to precedence
    else
    {
      char* lastStackElement = StackPop(&opStack);
      if(check_if_closed_paren(*lastStackElement))
      {
        while(!check_if_open_paren(*lastStackElement))
        {
          add_to_postfix(lastStackElement);
          lastStackElement = StackPop(&opStack);
        }
      }
      else if(precedence(lastStackElement, op_buff))
      {
        while(precedence(lastStackElement, op_buff))
        {
          add_to_postfix(lastStackElement);
          lastStackElement = StackPop(&opStack);
        }
      }
      else
      {
        StackPush(&opStack, lastStackElement);
      }
      StackPush(&opStack, op_buff);
    }
	} while (c!=EOF);

  for( i = 0; i <= pfIndex; i++)
  {
    print_cstring(postfix[i]);
  }

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

// Prints out cstring to stdout
void print_cstring(char* cstring)
{
  char c = *cstring;
  while(c != '\0')
  {
    printf("%c", c);
  }
}

// Compares two non-simple commands to see if ns1 has greater than or equal precedence to ns2
bool precedence(char* ns1, char* ns2)
{
  int nsVal1 = precedence_value(ns1);
  int nsVal2 = precedence_value(ns2);

  if(nsVal1 >= nsVal2)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Assigns a precedence value to nonsimple commands
// MIGHT NOT WORK DUE TO STRING COMPARISON
int precedence_value(char* nonsimple)
{
  if(strcmp(nonsimple, "&&") == 0 || strcmp(nonsimple, "||") == 0)
  {
    return 2;
  }
  else if(strcmp(nonsimple, "|") == 0 || strcmp(nonsimple, ";") == 0)
  {
    return 1;
  }
  else if(strcmp(nonsimple, ">") == 0 || strcmp(nonsimple, "<") == 0 || 
          strcmp(nonsimple, "(") == 0 || strcmp(nonsimple, ")") == 0)
  {
    return 0;
  }

  return -1;
}

// Adds a cstring element to the specified array
void add_to_array(char** arr, char* element, int* index)
{
  int i;
  char* tmp = NULL;
  int arrSize = sizeof(arr);
  if(*index==arrSize)
  {
    char** newArr = checked_malloc( arrSize + 50 );
    for(i = 0; i < arrSize; i++)
    {
      strcpy(tmp, arr[i]);
      newArr[i] = tmp;
    }
    arr = newArr;
  }
  
  arr[*index] = element;
  *index++; 
}

// Adds specified cstring element to the postfix array
void add_to_postfix(char* element)
{
  add_to_array(postfix, element, &pfIndex);
}

// Converts infix to postfix 
bool infix_to_postfix(char* simple, char* op)
{
  add_to_postfix(simple);

  if(StackIsEmpty(&opStack))
  {
    // Operator pushed into opStack successfully
    StackPush(&opStack, op);
    return true;
  }
  // opStack was not empty, so operator not pushed
  return false;
}

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
    case '>':
    case '<':
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
