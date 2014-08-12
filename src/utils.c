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
  do {
    c = fgetc( file );
    if( c == '#' ) {
      do c = fgetc( file );
      while( c != '\n' && !feof( file ) );
    }
  } while( isspace(c) && !feof( file ) );
  ungetc( c, file );
  return c;
}

int mint_values_waiting( FILE *file ) {
  char c = mint_skip_space( file );
  return isdigit(c) || c=='.' || c=='-' || c=='+';
}

static const char *keywords[6] = {
  "weights", "nodes", "spread", "network", "from", "to" };

int mint_keyword( char *string ) {
  int i, len1, len2;

  if( ! string )
    return 0;
  
  len1 = strlen(string);

  for( i=0; i<6; i++ ) {
    len2 = strlen( keywords[i] );
    if( len1 == len2 &&
	strncmp( string, keywords[i], len2 ) == 0 )
      return 1;
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

