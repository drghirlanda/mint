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
  s->len = len - 1;
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
  char c;
  struct mint_str *s;

  /* it's not a MINT string if it starts with a number */
  if( mint_values_waiting( f ) || feof( f ) )
    return 0;

  pos = ftell( f );
  len = 0;
  while( len<MINT_STRLEN && !feof(f) ) {
    c = fgetc( f );
    if( !isspace(c) ) len++;
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

#undef MINT_STRLEN
