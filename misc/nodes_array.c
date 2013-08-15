#include "nodes.h"
#include "update.h"
#include "utils.h"
#include "array.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct mint_nodes_str {
  int rows;
  struct mint_update *update;
};

mint_nodes mint_nodes_new( int size, int states ) {
  mint_array a;
  struct mint_nodes_str *nstr;
  size_t array_size[2];
  array_size[0] = 2+states;
  array_size[1] = size;
  a = mint_array_new( 2, array_size );
  nstr = malloc( sizeof(struct mint_nodes_str) );
  nstr->rows = 1;
  nstr->update = mint_update_new(0);
  mint_array_set_extra( a, nstr );
  return (mint_nodes)a;
}

void mint_nodes_del( mint_nodes n ) {
  struct mint_nodes_str *nstr;
  nstr = mint_array_get_extra( n );
  mint_update_del( nstr->update );
  free( nstr );
  mint_array_del( n );
}

mint_nodes mint_nodes_dup( const mint_nodes src ) {
  mint_array dst;
  struct mint_nodes_str *nstrsrc, *nstrdst;
  dst = mint_array_dup( src );
  mint_array_set_extra( dst, malloc(sizeof(struct mint_nodes_str)) );
  nstrdst = mint_array_get_extra( dst );
  nstrsrc = mint_array_get_extra( src );
  nstrdst->rows = nstrsrc->rows;
  nstrdst->update = mint_update_dup( nstrsrc->update );
  return dst;
}

void mint_nodes_cpy( mint_nodes dst, const mint_nodes src ) {
  struct mint_nodes_str *nstrdst, *nstrsrc;
  mint_array_cpy( dst, src );
  nstrdst = mint_array_get_extra( dst );
  nstrsrc = mint_array_get_extra( src );
  nstrdst->rows = nstrsrc->rows;
  mint_update_del( nstrdst->update );
  nstrdst->update = mint_update_dup( nstrsrc->update );
}

/* mint_nodes mint_nodes_load_pgm( FILE *file ) { */
/*   int size, read, i, k; */
/*   float max; */
/*   mint_nodes n; */
/*   char c; */

/*   c = fgetc( file ); */
/*   if( c!='P' ) {  */
/*     ungetc( c, file ); */
/*     return 0; /\* non-fatal error, handled by mint_nodes_load *\/ */
/*   } */

/*   c = fgetc( file ); */
/*   mint_check( c=='2', "image file does not start with 'P2'" ); */
/*   read = fscanf( file, "%d %d %f", &i, &k, &max ); */
/*   mint_check( read==3, "cannot read PGM parameters" ); */
/*   size = i*k; */

/*   n = mint_nodes_new( size, 0 ); */
/*   for( i=0; i<size; i++ ) { */
/*     read = fscanf( file, " %f", n[1]+i ); */
/*     mint_check( read==1, "cannot read values" ); */
/*     n[1][i] /= max; */
/*   } */
/*   return n; */
/* } */

/* mint_nodes mint_nodes_load_mint( FILE *file ) { */
/*   int size, states, read, i, k; */
/*   mint_nodes n; */

/*   mint_skip_space( file ); */
/*   read = fscanf( file, "nodes %d %d", &size, &states ); */
/*   mint_check( read==2, "cannot read nodes geometry" ); */
/*   n = mint_nodes_new( size, states ); */
/*   _STR(n)->update = mint_update_load( file ); */

/*   if( mint_values_waiting( file ) ) { */
/*     for( i=0; i<size; i++ ) for( k=0; k<2+states; k++ ) { */
/*  read = fscanf( file, " %f", n[k]+i ); */
/*  mint_check( read == 1, "cannot read values" ); */
/*       } */
/*   } */
/*   return n; */
/* } */

mint_nodes mint_nodes_load( FILE *file ) {
  int size, states, rows, read;
  size_t array_size[3];
  struct mint_nodes_str *nstr;
  mint_nodes a;

  mint_skip_space( file );
  read = fscanf( file, "nodes %d %d %d", &size, &states, &rows );
  mint_check( read==2, "cannot read nodes geometry" );
  if( mint_skip_space( file ) == 'a' ) {
    a = mint_array_load( file );
  } else {
    array_size[0] = 2+states;
    array_size[1] = size;
    a = mint_array_new( 2, array_size );
  }
  nstr = malloc( sizeof(struct mint_nodes_str) );
  nstr->rows = rows;
  nstr->update = mint_update_load( file );
  mint_array_set_extra( a, nstr );
  return a;
}

void mint_nodes_save( const mint_nodes n, FILE *f ) {
  size_t size, states, rows;
  size = mint_array_size(n,1);
  states = mint_array_size(n,0) - 2;
  rows = mint_nodes_rows(n);
  fprintf( f, "nodes %d %d %d\n", size, states, rows );
  mint_array_save( n, f );
  mint_update_save( mint_array_get_extra(n), f );
}

int mint_nodes_size( const mint_nodes n ) {
  return mint_array_size( n, 1 );
}

int mint_nodes_states( const mint_nodes  n ) {
  return mint_array_size( n, 0 ) - 2;
}

int mint_nodes_rows( const mint_nodes  n ) {
  struct mint_nodes_str *nstr;
  nstr = mint_array_get_extra( n );
  return nstr->rows;
}

struct mint_update *mint_nodes_get_update( mint_nodes n ) {
  struct mint_nodes_str *nstr;
  nstr = mint_array_get_extra( n );
  return nstr->update;
}

struct mint_update *mint_nodes_set_update( mint_nodes n, 
					   const char *name ) {
  struct mint_nodes_str *nstr;
  nstr = mint_array_get_extra(n);
  mint_update_del( nstr->update );
  nstr->update = mint_update_new( name );
  return nstr->update;
}

void mint_nodes_set( mint_nodes n, int i, float x ) {
  int start[2] = {i, 0};
  int end[2] = {i, -1};
  mint_array_set_slice( n, x, start, end );
}

