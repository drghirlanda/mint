#include "nodes.h"
#include "utils.h"
#include "op.h"
#include "string.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct mint_nodes_str {
  int size;
  int states;
  struct mint_ops *ops;
  struct mint_str *name;
};

#define _STR(n) ( (struct mint_nodes_str *)n - 1 )

/* size in bytes of a mint_nodes object, includes input and output
   values, node states and a struct mint_nodes_str */
int mint_nodes_bytes( int size, int states ) {
  int b = 0;
  b += size * (2 + states) * sizeof(float); /* values + states */
  b += (2+states) * sizeof(float *);        /* pointers */
  b += sizeof(struct mint_nodes_str);       
  return b;
}

mint_nodes mint_nodes_new( int size, int states ) {
  int offset;
  struct mint_nodes_str *nstr;
  mint_nodes n;
  int s, i;
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

  /* set everything to zero initially */
  for( s = 0; s < 2 + states; s++ ) {
    for( i = 0; i < size; i++ )
      n[s][i] = 0;
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
  int size, states, read;
  int i, k;
  long pos;
  mint_nodes n;
  struct mint_str *name;
  struct mint_nodes_str *nstr;
  struct mint_op *op;
  struct mint_ops *ops;

  if( ! mint_next_string( file, "nodes", 5 ) )
    return 0;
    
  /* attempt to read nodes name. if 'name' turns out to be op or
     keyword, create default name and rewind file */
  pos = ftell( file );
  name = mint_str_load( file );
  if( mint_op_exists( mint_str_char(name), mint_op_nodes_any) ||
      mint_keyword( mint_str_char(name) ) ) {
    mint_str_del( name );
    name = mint_str_new( "n" );
    fseek( file, pos, SEEK_SET );
  }

  ops = mint_ops_load( file, mint_op_nodes_any );
  
  i = mint_ops_find( ops, "size", mint_op_nodes_init );
  mint_check( i>=0, "cannot read nodes size" );
  size = mint_op_get_param( mint_ops_get(ops, i), 0 );

  i = mint_ops_find( ops, "states", mint_op_nodes_init );
  if( i == -1 ) {
    states = 0;
    op = mint_op_new( "states", mint_op_nodes_init );
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
  int k;
  struct mint_nodes_str *nstr = _STR( n );

  mint_nodes_info( n, f );
  for( k=0; k<2+nstr->states; k++ )
    mint_nodes_save_var( n, k, f );
}

void mint_nodes_info( const mint_nodes n, FILE *f ) {
  struct mint_nodes_str *nstr = _STR( n );
  fprintf( f, "nodes %s\n", mint_str_char( nstr->name ) );
  mint_ops_save( nstr->ops, f );
}

int mint_nodes_size( const mint_nodes n ) {
  return _STR( n )->size;
}

int mint_nodes_states( const mint_nodes  n ) {
  return _STR( n )->states;
}

struct mint_ops *mint_nodes_get_ops( const mint_nodes n ) {
  struct mint_nodes_str *nstr = _STR(n); 
  return nstr->ops;
}

void mint_nodes_set( mint_nodes n, int i, float x ) {
  int j;
  struct mint_nodes_str *nstr;
  nstr = _STR(n);
  mint_check( i>=0 && i<2+nstr->states, "index out of range" );
  for( j=0; j<nstr->size; j++ )
    n[i][j] = x;
}

mint_nodes mint_nodes_resize( mint_nodes n, int s2 ) {
  int s1, to_copy;
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

int mint_nodes_get_property( mint_nodes n, const char *prop, int i,
			 float *value ) {
  struct mint_ops *ops;
  int k;

  ops = mint_nodes_get_ops( n );
  k = mint_ops_find( ops, prop, mint_op_nodes_any );

  if( k == -1 ) 
    return 0;
  else if( value )
    *value = mint_op_get_param( mint_ops_get(ops, k), i );

  return 1;
}

int mint_nodes_set_property( mint_nodes n, const char *prop, int i,
			 float value ) {
  struct mint_ops *ops;
  int k;

  ops = mint_nodes_get_ops( n );
  k = mint_ops_find( ops, prop, mint_op_nodes_any );

  if( k == -1 ) 
    return 0;

  mint_op_set_param( mint_ops_get(ops, k), i, value );
  return 1;
}

void mint_nodes_index2coord( mint_nodes n, int i, int *x, int *y ) {
  static mint_nodes nlast = 0;
  static int rows = 0;
  static int size = 0;
  float frows;

  if( n != nlast ) { /* set rows, cols, and size */
    size = mint_nodes_size( n );
    if( mint_nodes_get_property( n, "rows", 0, &frows ) )
      rows = frows;
    else
      rows = 0;
    nlast = n;
  }

  *x = rows ? 0 : i/rows;
  *y = rows ? i : i % rows;
}

int mint_nodes_coord2index( mint_nodes n, int x, int y ) {
  static mint_nodes nlast = 0;
  static int rows = 0;
  static int size = 0;
  float frows;

  if( n != nlast ) { /* set rows, cols, and size */
    size = mint_nodes_size( n );
    if( mint_nodes_get_property( n, "rows", 0, &frows ) )
      rows = frows;
    else
      rows = 0;
    nlast = n;
  }
  return y + rows * x;
}

#undef _STR
