#include "weights.h"
#include "update.h"
#include "image.h"
#include "utils.h"
#include "array.h"

#include <string.h>
#include <stdlib.h>

struct mint_weights_str {
  int from;
  int to;
  struct mint_update *update;
};

mint_weights mint_weights_new( size_t rows, size_t cols, size_t states ) {
  mint_array a;
  struct mint_weights_str *wstr;
  size_t array_size[3] = {1+states, rows, cols};
  a = mint_array_new( 3, array_size );
  wstr = malloc( sizeof(struct mint_weights_str) );
  wstr->from = -1;
  wstr->to = -1;
  wstr->update = mint_update_new( 0 );
  mint_array_set_extra( a, wstr );
  return a;
}

void mint_weights_del( mint_weights w ) { 
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  mint_update_del( wstr->update );
  free( wstr );
  mint_array_del( w );
}

mint_weights mint_weights_dup( const mint_weights src ) {
  mint_array dst;
  struct mint_weights_str *wstrsrc, *wstrdst;
  dst = mint_array_dup( src );
  mint_array_set_extra( dst, malloc( sizeof(struct mint_weights_str) ) );
  wstrdst = mint_array_get_extra( dst );
  wstrsrc = mint_array_get_extra( src );
  wstrdst->from = wstrsrc->from;
  wstrdst->to = wstrsrc->to;
  wstrdst->update = mint_update_dup( wstrsrc->update );
  return dst;
}

void mint_weights_cpy( mint_weights dst, const mint_weights src ) {
  struct mint_weights_str *wstrdst, *wstrsrc; 
  mint_array_cpy( dst, src );
  wstrdst = mint_array_get_extra( dst );
  wstrsrc = mint_array_get_extra( src );
  wstrdst->from = wstrsrc->from;
  wstrdst->to = wstrsrc->to;
  mint_update_cpy( wstrdst->update, wstrsrc->update );
}

mint_weights mint_weights_load( FILE *f ) {
  int i;
  mint_weights w;
  struct mint_weights_str *wstr;
  int rows, cols, states, read;
  size_t array_size[3];

  mint_skip_space( f );
  read = fscanf( f, "weights %d %d %d", &rows, &cols, &states );
  mint_check( read==3, "cannot read weights geometry" );
  if( mint_skip_space( f ) == 'a' ) {
    w = mint_array_load( f );
  } else {
    array_size[0] = 1+states;
    array_size[1] = rows;
    array_size[2] = cols;
    w = mint_array_new( 3, array_size );
  }
  wstr = malloc( sizeof(struct mint_weights_str) );
  i = fscanf( f, "%d", &wstr->from );
  mint_check( i==1, "cannot read 'from' index" );
  i = fscanf( f, "%d", &wstr->to );
  mint_check( i==1, "cannot read 'to' index" );
  wstr->update = mint_update_load( f );
  mint_array_set_extra( w, wstr );
  return w;
}

void mint_weights_save( const mint_weights w, FILE *f ) {
  int i;
  struct mint_weights_str *wstr;
  i = fprintf( f, "weights %d %d %d\n", 
	       mint_array_size(w,0)-1, 
	       mint_array_size(w,1), 
	       mint_array_size(w,2) ); 
  mint_check( i>0, "cannot write weights geometry" );
  mint_array_save( w, f );
  wstr = mint_array_get_extra( w );
  i = fprintf( f, "%d %d\n", wstr->from, wstr->to );
  mint_check( i>0, "cannot write 'from' and 'to' indices" );
  mint_update_save( wstr->update, f );
}

size_t mint_weights_rows( const mint_weights w ) {
  return mint_array_size( w, 1 );
}

size_t mint_weights_cols( const mint_weights w ) {
  return mint_array_size( w, 2 );
}

size_t mint_weights_states( const mint_weights w ) {
  return mint_array_size( w, 0 );
}

int mint_weights_get_from( const mint_weights w ) {
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  return wstr->from;
}

int mint_weights_get_to( const mint_weights w ) {
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  return wstr->to;
}

void mint_weights_set_from( mint_weights w, int i ) {
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  wstr->from = i;
}

void mint_weights_set_to( mint_weights w, int i ) {
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  wstr->to = i;
}

struct mint_update *mint_weights_get_update( const mint_weights w ) {
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  return wstr->update;
}

void mint_weights_set_update( mint_weights w, 
			      const struct mint_update *unew ) {
  struct mint_weights_str *wstr;
  wstr = mint_array_get_extra( w );
  mint_update_cpy( wstr->update, unew );
}

void mint_weights_mult( mint_weights w, mint_nodes from, mint_nodes to ) {
  int i, j, r, c;
  c = mint_array_size( w, 2 );
  r = mint_array_size( w, 1 );
  mint_check( c == mint_nodes_size(from), "dimensions don't match" );
  mint_check( r == mint_nodes_size(to), "dimensions don't match" );
  for( i=0; i<r; i++ ) for( j=0; j<c; j++ ) 
    to[0][i] += w[0][i][j] * from[1][j];
}

void mint_weights_multrow( mint_weights w, mint_nodes from, 
			   mint_nodes to, int i ) {
  int j, r, c;
  c = mint_array_size( w, 2 );
  r = mint_array_size( w, 1 );
  mint_check( c == mint_nodes_size(from), "dimensions don't match" );
  mint_check( r == mint_nodes_size(to), "dimensions don't match" );
  mint_check( i>=0 && i<r, "row index out of range" );
  for( j=0; j<c; j++ ) to[0][i] += w[0][i][j] * from[1][j];
}

