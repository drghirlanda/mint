#include <stdlib.h>
#include <string.h>
#include "str.h"

struct mint_str {
  size_t len;
  char *data;
};

struct mint_str *mint_str_new( size_t len ) {
  struct mint_str *s;
  s = malloc( sizeof(struct mint_str) );
  s->data = malloc( len );
  s->len = len;
  return s;
}

void mint_str_del( struct mint_str *str ) {
  free( str->data );
  free( str );
}

struct mint_str *mint_str_dup( const struct mint_str *src ) {
  struct mint_str *dst;
  dst = mint_str_new( src->len );
  mint_str_cpy( dst, src );
  return dst;
}

void mint_str_cpy( struct mint_str *dst, const struct mint_str *src ) {
  if( dst->len != src->len ) {
    dst->data = realloc( dst->data, src->len );
    dst->len = src->len;
  }
  memcpy( dst->data, src->data, dst->len );
}

struct mint_str *mint_str_load( FILE *f ) {
  struct mint_str *s;
  char buf[256], *r;
  r = fgets( buf, 256, f );
  s = mint_str_new( strlen(buf) );
  memcpy( s->data, buf, s->len );
  return s;
}

void mint_str_save( const struct mint_str *s, FILE *f ) {
  char buf[256];
  sprintf( buf, "%%%d", s->len );
  fprintf( f, buf, s->data );
}

int mint_str_size( const struct mint_str *s ) {
  return s->len;
}


