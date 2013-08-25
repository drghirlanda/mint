#include "nodes.h"
#include "utils.h"
#include "op.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct mint_nodes_str {
  unsigned int size;
  unsigned int states;
  struct mint_ops *ops;
};

#define _STR(n) ( (struct mint_nodes_str *)n - 1 )

/* size in bytes of a mint_nodes object, includes input and output
   values, node states and a struct mint_nodes_str */
size_t mint_nodes_bytes( unsigned int size, unsigned int states ) {
  size_t b = 0;
  b += size * (2 + states) * sizeof(float); /* values + states */
  b += (2+states) * sizeof(float *);        /* pointers */
  b += sizeof(struct mint_nodes_str);       
  return b;
}

mint_nodes mint_nodes_new( unsigned int size, unsigned int states ) {
  size_t offset;
  struct mint_nodes_str *nstr;
  mint_nodes n;
  unsigned int s;
  nstr = malloc( mint_nodes_bytes(size, states) );
  mint_check( nstr!=0, "out of memory!" );
  nstr->size = size;
  nstr->states = states;
  nstr->ops = mint_ops_new();
  n = (mint_nodes) ++nstr;
  /* values start at offset, before we store n[s] pointers */
  offset = (2+states)*sizeof(float *);
  for( s=0; s<2+states; s++ ) {
    n[s] = (float *)( (char *)n + offset + s*size*sizeof(float) );
  }
  return n;
}

void mint_nodes_del( mint_nodes n ) { 
  if( !n ) return;
  struct mint_nodes_str *nstr = _STR(n);
  mint_ops_del( nstr->ops );
  free( nstr );
}

mint_nodes mint_nodes_dup( const mint_nodes src ) {
  mint_nodes dst = mint_nodes_new( _STR(src)->size, _STR(src)->states );
  mint_nodes_cpy( dst, src );
  return dst;
}

void mint_nodes_cpy( mint_nodes dst, const mint_nodes src ) {
  struct mint_nodes_str *dstr, *sstr; 
  if( dst == src ) return;
  sstr = _STR( src );
  dstr = _STR( dst );
  mint_check( dstr->size==sstr->size, 
	      "sizes differ: %d != %d",  dstr->size, sstr->size );
  mint_check( dstr->states==sstr->states, 
	      "state variables differ: %d != %d", dstr->size, sstr->size );
  mint_ops_del( dstr->ops );
  dstr->ops = mint_ops_dup( sstr->ops );
  memcpy( &dst[0][0], &src[0][0], 
	  (2+dstr->states) * dstr->size * sizeof(float) );
}

mint_nodes mint_nodes_load( FILE *file ) {
  unsigned int size, states, read, i, k;
  mint_nodes n;
  struct mint_nodes_str *nstr;
  struct mint_op *op;
  mint_skip_space( file );
  read = fscanf( file, "nodes %d %d", &size, &states );
  mint_check( read==2, "cannot read nodes geometry" );
  n = mint_nodes_new( size, states );
  nstr = _STR(n);
  nstr->ops = mint_ops_load( file );

  /* add identity update op is no update op specified */
  if( mint_ops_count( nstr->ops, mint_op_nodes_update ) < 1 ) {
    op = mint_op_new( "identity" );
    mint_ops_append( nstr->ops, op );
  }
  
  if( mint_values_waiting( file ) ) {
    for( k=0; k<2+states; k++ ) for( i=0; i<size; i++ ) {
	read = fscanf( file, " %f", n[k]+i );
	mint_check( read == 1, "cannot read values" );
      }
  }
  return n;
}

void mint_nodes_save_var( const mint_nodes n, int k, FILE *f  ) {
  int i;
  struct mint_nodes_str *nstr = _STR( n );
  mint_check( k>=0 && k<2+nstr->states, "variable index out of bounds" );
  for( i=0; i<nstr->size; i++ )
    fprintf( f, "%g ", n[k][i] );
  fprintf( f, "\n" );
}

void mint_nodes_save( const mint_nodes n, FILE *f ) {
  unsigned int k;
  struct mint_nodes_str *nstr = _STR( n );
  fprintf( f, "nodes %d %d\n", nstr->size, nstr->states );
  mint_ops_save( nstr->ops, f );
  for( k=0; k<2+nstr->states; k++ )
    mint_nodes_save_var( n, k, f );
}

unsigned int mint_nodes_size( const mint_nodes n ) {
  return _STR( n )->size;
}

unsigned int mint_nodes_states( const mint_nodes  n ) {
  return _STR( n )->states;
}

struct mint_ops *mint_nodes_get_ops( const mint_nodes n ) {
  struct mint_nodes_str *nstr = _STR(n); 
  return nstr->ops;
}

void mint_nodes_set( mint_nodes n, unsigned int i, float x ) {
  unsigned int j;
  struct mint_nodes_str *nstr;
  nstr = _STR(n);
  mint_check( i>=0 && i<2 + nstr->states, "index out of range" );
  for( j=0; j<nstr->size; j++ )
    n[i][j] = x;
}

mint_nodes mint_nodes_resize( mint_nodes n, unsigned int s2 ) {
  int s1;
  size_t to_copy;
  mint_nodes n2;

  s1 = _STR(n)->size;
  mint_check( s2>0, "cannot create node group with <1 nodes" );
  n2 = mint_nodes_new( s2, _STR(n)->states );
  if( s2>s1 ) to_copy = s1;
  else to_copy = s2;
  memcpy( &n2[0][0], &n[0][0], (2+_STR(n)->states)*to_copy*sizeof(float) );
  _STR(n2)->ops = mint_ops_dup( _STR(n)->ops );
  mint_nodes_del( n );
  return n2;
}

#undef _STR
