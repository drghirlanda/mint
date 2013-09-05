#include "nodes.h"
#include "utils.h"
#include "op.h"
#include "str.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct mint_nodes_str {
  unsigned int size;
  unsigned int states;
  struct mint_ops *ops;
  struct mint_str *name;
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
  nstr->name = mint_str_new( "n" );
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
  mint_str_del( nstr->name );
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
  dstr->name = mint_str_dup( sstr->name );
  memcpy( &dst[0][0], &src[0][0], 
	  (2+dstr->states) * dstr->size * sizeof(float) );
}

mint_nodes mint_nodes_load( FILE *file ) {
  unsigned int size, states, read;
  int i, k;
  long pos;
  mint_nodes n;
  struct mint_str *name;
  struct mint_nodes_str *nstr;
  struct mint_op *op;
  struct mint_ops *ops;

  if( ! mint_next_string( file, "nodes", 5 ) )
    mint_check( 0, "cannot find 'nodes' keyword" );
    
  /* attempt to read nodes name. if 'name' turns out to be op or
     keyword, create default name and rewind file */
  pos = ftell( file );
  name = mint_str_load( file );
  if( mint_op_exists( mint_str_char(name) ) ||
      mint_keyword( mint_str_char(name) ) ) {
    mint_str_del( name );
    name = mint_str_new( "n" );
    fseek( file, pos, SEEK_SET );
  }

  ops = mint_ops_load( file );
  
  i = mint_ops_find( ops, "size" );
  mint_check( i>=0, "cannot read nodes size" );
  size = mint_op_get_param( mint_ops_get(ops, i), 0 );

  i = mint_ops_find( ops, "states" );
  if( i == -1 ) {
    states = 0;
    op = mint_op_new( "states" );
    mint_op_set_param( op, 0, states );
    mint_ops_append( ops, op );
    mint_op_del( op );
  } else
    states = mint_op_get_param( mint_ops_get(ops, i), 0 );
				
  n = mint_nodes_new( size, states );

  nstr = _STR(n);
  mint_str_del( nstr->name ); /* default set in mint_nodes_new */
  nstr->name = name;
  nstr->ops = ops;
  
  /* load values if on file */
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

  fprintf( f, "nodes %s\n", mint_str_char( nstr->name ) );
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
  mint_check( i>=0 && i<2+nstr->states, "index out of range" );
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

struct mint_str *mint_nodes_get_name( mint_nodes n ) {
  return _STR(n)->name ;
}

#undef _STR
