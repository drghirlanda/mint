#include "utils.h"
#include "op.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

int mint_next_string( FILE *file, char *string, int len ) {
  int i;
  long pos;
  char c;
  pos = ftell( file );
  mint_skip_space( file );
  i = 0;
  while( i<len ) {
    c = fgetc( file );
    if( c != string[i] ) { /* at EOF this will return */
      fseek( file, pos, SEEK_SET );
      return 0;
    }
    i++;
  }
  return 1;
}

int mint_skip_space( FILE *file ) {
  char c;
  do c = fgetc( file );
  while( isspace(c) );
  ungetc( c, file );
  return c;
}

int mint_values_waiting( FILE *file ) {
  char c = mint_skip_space( file );
  return isdigit(c) || c=='.' || c=='-' || c=='+';
}


static const char *keywords[4] = {
  "weights", "nodes", "spread", "network" };

int mint_keyword( FILE *file ) {
  int i, len;
  long pos;
  char name[256];

  pos = ftell( file );
  i = fscanf( file, " %255s", name );
  fseek( file, pos, SEEK_SET );

  if( i != 1 ) /* no string on file */
    return 0;

  for( i=0; i<4; i++ ) {
    len = strlen( keywords[i] );
    if( strncmp(name,keywords[i],len) == 0 ) {
      fseek( file, pos, SEEK_SET );
      return 1;
    }
  }

  return 0;
}

int mint_default( const char *name ) {
  static const char *defaults[3] = { "identity", "mult", "synchronous" };
  int i, len;
  for( i=0; i<2; i++ ) {
    len = strlen( defaults[i] );
    if( strncmp(name,defaults[i],len) == 0 )
      return 1;
  }
  return 0;
}

/* strictly speaking, having a library-wide global variable is not
   thread safe, but reading from file is not multithreaded, so it
   should be OK */

static FILE *mint_file = 0;

void mint_set_file( FILE *file ) {
  mint_file = file;
}

FILE *mint_get_file( void ) {
  return mint_file;
}
