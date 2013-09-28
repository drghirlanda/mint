#include "str.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MINT_STRLEN 256

struct mint_str {
  size_t len;
  char *data;
};

struct mint_str *mint_str_new( const char *str ) {
  int len = 0;
  struct mint_str *s;
  s = malloc( sizeof(struct mint_str) );
  s->data = malloc( MINT_STRLEN );
  /* we make sure data is null terminated and up to MINT_STRLEN in size */
  while( len < MINT_STRLEN && str[ len ] != '\0' ) {
    s->data[ len ] = str[ len ];
    len++;
  }
  s->data[ len ] = '\0';
  s->len = len;
  return s;
}

void mint_str_del( struct mint_str *str ) {
  free( str->data );
  free( str );
}

struct mint_str *mint_str_dup( const struct mint_str *src ) {
  return mint_str_new( src->data );
}

void mint_str_cpy( struct mint_str *dst, const struct mint_str *src ) {
  dst->data = realloc( dst->data, src->len + 1 );
  memcpy( dst->data, src->data, src->len + 1 );
  dst->len = src->len;
}

struct mint_str *mint_str_load( FILE *f ) {
  long pos;
  int len;
  int c;
  struct mint_str *s;

  /* it's not a MINT string if it starts with a number */
  if( mint_values_waiting( f ) || feof( f ) )
    return 0;

  pos = ftell( f );
  len = 0;
  while( len<MINT_STRLEN && !feof(f) ) {
    c = fgetc( f );
    if( isalnum(c) || c == '-' || c == '.' ) len++;
    else break;
  }
  fseek( f, pos, SEEK_SET );
  if( !len ) return 0;
  s = (struct mint_str *)malloc( sizeof(struct mint_str) );
  s->data = malloc( len + 1 );
  fscanf( f, "%s", s->data );
  s->data[ len ] = '\0';
  s->len = len;
  return s;
}

void mint_str_save( const struct mint_str *s, FILE *f ) {
  if( !s ) return;
  fprintf( f, "%s ", s->data );
}

int mint_str_size( const struct mint_str *s ) {
  if( s ) return s->len;
  else return 0;
}

char *mint_str_char( struct mint_str *s ) {
  if( s && s->len ) return s->data;
  else return 0;
}

void mint_str_append( struct mint_str *str, char *more ) {
  int len;
  len = str->len + strlen(more);
  str->data = realloc( str->data, len );
  str->data = strncat( str->data, more, len );
}

int mint_str_find( struct mint_str *str, char c ) {
  int i;
  for( i=0; i<str->len; i++ ) {
    if( str->data[i] == c )
      return i;
  }
  return -1;
}

struct mint_str *mint_str_substr( struct mint_str *str, int start, 
				  int stop ) {
  struct mint_str *substr;

  mint_check( str, "null string" );
  mint_check( start>=0 && start<str->len, "start out of range" );
  mint_check( stop>=start && stop<=str->len, "stop out of range" );

  substr = malloc( sizeof(struct mint_str) );
  substr->len = stop - start + 1;
  substr->data = malloc( substr->len );
  memcpy( substr->data, str->data + start, substr->len - 1 );
  substr->data[ substr->len - 1 ] = '\0';
  return substr;
}

/* # of digits an integer will print to (adapted from a stackoverflow
   answer). we don't need to handle negative numbers. */
int mint_str_numlen( int n ) {
  if (n < 10) 
    return 1;
  return 1 + mint_str_numlen( n/10 );
}

void mint_str_incr( struct mint_str *str ) {
  int i, j, added_len;

  i = mint_str_find( str, '.' );
  if( i == -1 ) {
    str->data = realloc( str->data, str->len + 2 );
    sprintf( str->data + str->len, ".2" );
    str->len += 2;
  } else {
    sscanf( str->data + i + 1, "%d", &j );
    added_len = mint_str_numlen(j+1) - mint_str_numlen(j);
    if( added_len )
      str->data = realloc( str->data, str->len + added_len );
    sprintf( str->data + i + 1, "%d", j+1 );
  }
}

#undef MINT_STRLEN
