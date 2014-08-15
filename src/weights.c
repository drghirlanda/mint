#include "weights.h"
#include "utils.h"
#include "random.h"
#include "network.h"
#include "op.h"
#include "string.h"
#include "wop.h"

#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include <time.h>

struct mint_weights_str {
  int rows;
  int cols;
  int states;
  int from;
  int to;
  mint_string name;
  int target; /* which 'to' variable results are stored in */
  struct mint_ops *ops;

  /* these are for sparse matrices */
  int **cind; /* cind[r][i] = col index of value i in row r */
  int *rlen;  /* rlen[r] = # elements in row r */
};

#define _STR( w ) ( (struct mint_weights_str *)w - 1 )

/* calculate the number of bytes needed to store a dense or sparse
   matrix of given size. */ 
int mint_weights_bytes( int rows, int cols, 
			int states, int sparse ) {
  int b = 0;
  b += sizeof(struct mint_weights_str);
  b += (1+states) * sizeof(float **); /* w[s] */
  b += (1+states) * rows * sizeof(float *); /* w[s][r] */
  if( !sparse )
    b += rows * cols * (1+states) * sizeof(float); /* w[s][r][c] */
  return b;
}

/* this internal function handles memory allocation and basic setup
   for dense and sparse matrices */
static mint_weights mint_weights_alloc( int rows,
					int cols,
					int states,
					int sparse ) {
  mint_weights w;
  int s, r, c;
  char *start1, *start2;
  struct mint_weights_str *wstr;

  mint_check( rows>0, "number of rows is negative or zero" );
  mint_check( cols>0, "number of columns is negative or zero" );
  mint_check( states>=0, "number of states is negative" );
  wstr = malloc( mint_weights_bytes(rows, cols, states, sparse) );
  mint_check( wstr!=0, "out of memory" );
  wstr->rows = rows;
  wstr->cols = cols;
  wstr->states = states;
  wstr->from = -1;
  wstr->to = -1;
  wstr->name = mint_string_new( "w" ); /* default name */
  wstr->target = 0;
  wstr->ops = mint_ops_new();

  w = (mint_weights) (wstr + 1);

  /* the following loops set up w[s] and w[s][r] pointers so that they
     point to appropriate places in the memory block allocated by
     weights_alloc(). for sparse matrices, w[s][r] pointers don't make
     sense until the matrix is filled, and are set to 0 here */

  /* w[s] pointers stored starting from here: */
  start1 = (char *)w + (1+states)*sizeof(float **);
  /* w[s][r] pointers stored starting from here: */
  start2 = start1 + (1+states)*rows*sizeof(float *);
  for( s=0; s<1+states; s++ ) {
    /* each w[s] needs 'rows' places for w[s][r] pointers */
    w[s] = (float **)( start1 + s*rows*sizeof(float *) );
    if( sparse ) {
      for( r=0; r<rows; r++ ) 
	w[s][r] = 0;
    } else {
      for( r=0; r<rows; r++ )
	w[s][r] = (float *)( start2 + (s*rows*cols + r*cols)*sizeof(float) );
    }
  }

  if( !sparse ) { /* set everything to zero */
    for( s=0; s<1+states; s++ ) {
      for( r=0; r<rows; r++ ) {
	for( c=0; c<cols; c++ )
	  w[s][r][c] = 0;
      }
    }
  }

  return w;
}

mint_weights mint_weights_new( int rows, int cols, 
			       int states ) {
  mint_weights w;
  struct mint_weights_str *wstr;
  w = mint_weights_alloc( rows, cols, states, 0 );
  wstr = _STR(w);
  wstr->rlen = 0;
  wstr->cind = 0;
  return w;
}

mint_weights mint_weights_sparse_new( int rows, int cols, 
				      int states ) {
  mint_weights w;
  struct mint_weights_str *wstr;
  int r;
  w = mint_weights_alloc( rows, cols, states, 1 );
  wstr = _STR(w);
  wstr->rlen = malloc( rows*sizeof(int) );
  wstr->cind = malloc( rows*sizeof(int *) );
  for( r=0; r<rows; r++ ) {
    wstr->rlen[r] = 0;
    wstr->cind[r] = 0;
  }
  return w;
}

void mint_weights_del( mint_weights w ) { 
  int r;
  mint_check( w!=0, "null 1st arg" );
  struct mint_weights_str *wstr = _STR( w );
  mint_ops_del( wstr->ops );
  if( wstr->rlen ) {
    for( r=0; r<wstr->rows; r++ )
      free( wstr->cind[r] );
    free( wstr->cind );
    free( wstr->rlen );
  }
  mint_string_del( wstr->name );
  free( wstr );
}

mint_weights mint_weights_dup( const mint_weights src ) {
  struct mint_weights_str *w = _STR(src);
  mint_weights dst;
  if( mint_weights_is_sparse(src) ) 
    dst = mint_weights_sparse_new( w->rows, w->cols, w->states );
  else 
    dst = mint_weights_new( w->rows, w->cols, w->states );
  mint_weights_cpy( dst, src );
  return dst;
}

void mint_weights_cpy( mint_weights dst, const mint_weights src ) {
  int r, s, rows, cols, states;
  struct mint_weights_str *dstr, *sstr; 

  if( dst == src ) return;

  sstr = _STR( src );
  dstr = _STR( dst );

  rows = dstr->rows;
  cols = dstr->cols;
  states = dstr->states;
  mint_check( rows == sstr->rows, "different number of rows" );
  mint_check( cols == sstr->cols, "different number of columns" );
  mint_check( states == sstr->states, "different number of states" );

  dstr->from = sstr->from;
  dstr->to = sstr->to;
  mint_string_del( dstr->name );
  dstr->name = mint_string_dup( sstr->name );
  dstr->target = sstr->target;
  mint_ops_del( dstr->ops );
  dstr->ops = mint_ops_dup( sstr->ops );

  if( mint_weights_is_sparse(src) ) {
    for( s=0; s<1+states; s++ ) {
      for( r=0; r<rows; r++ )
	mint_weights_set_row( dst, r, sstr->rlen[r], src[s][r], sstr->cind[r], s );
    }
  } else
    memcpy( &dst[0][0][0], &src[0][0][0], (1+states)*rows*cols*sizeof(float) );
}

void mint_weights_load_values( mint_weights w, FILE *f ) {
  struct mint_weights_str *wstr;
  int i, r, c, s, nval = 0;

  if( !mint_values_waiting(f) )
    return;

  wstr = _STR(w);

  for( r=0; r<wstr->rows; r++ ) {

    if( mint_weights_is_sparse(w) ) { 

      i = fscanf( f, " %d", wstr->rlen + r );
      mint_check( i==1, "cannot read row length" );
      for( s=0; s<1+wstr->states; s++)
	w[s][r] = malloc( nval*sizeof(float) );
      wstr->cind[r] = malloc( wstr->rlen[r] * sizeof(int) );
      for( c=0; c<wstr->rlen[r]; c++ ) {
	i = fscanf( f, " %d", wstr->cind[r]+c );
	mint_check( i==1, "cannot read column indices" );
      }
      nval = wstr->rlen[r];

    } else
      nval = wstr->cols;

    for( s=0; s<1+wstr->states; s++ ) {
      for( c=0; c<nval; c++ ) {
	i = fscanf( f, " %f", &w[s][r][c] );
	mint_check( i==1, "cannot read values" );
      }
    }

  }

}

mint_weights mint_weights_load( FILE *file, struct mint_network *net ) {
  mint_weights w;
  mint_nodes n;
  struct mint_weights_str *wstr;
  struct mint_op *op;
  struct mint_ops *ops;
  mint_string fromname, toname, name;
  int rows, cols, states, sparse;
  int from, to, i, j;
  long pos;

  if( ! mint_next_string( file, "weights", 7 ) )
    return 0;

  /* sensible initial values (used for error checking) */
  fromname = toname = name = 0;
  sparse = rows = cols = states = 0;
  from = to = -1;

  /* attempt to read weights name. if 'name' turns out to be op or
     keyword, create default name and rewind file */
  pos = ftell( file );
  name = mint_string_load( file );
  if( mint_op_exists( name, mint_op_weights_any ) ||
      mint_keyword( name ) ) {
    mint_string_del( name );
    name = mint_string_new( "w" );
    fseek( file, pos, SEEK_SET );
  }
  
  /* parse name to see whether it contains from-to info */
  i = mint_string_find(name, '-');
  if( i > -1 ) {
    fromname = mint_string_substr( name, 0, i );
    j = mint_string_find( name, '.' );
    if( j == -1 )
      j = mint_string_size( name );
    toname = mint_string_substr( name, i+1, j );
    from = mint_network_nodes_find( net, fromname );
    to = mint_network_nodes_find( net, toname );
    mint_check( from != -1, "cannot find 'from' nodes: %s", fromname );
    mint_check( to != -1, "cannot find 'to' nodes: %s", toname );
  }

  if( mint_next_string( file, "from", 4 ) ) {
    mint_check( !fromname, "'from' given twice for matrix %s", name );
    fromname = mint_string_load( file );
    mint_check( fromname, "cannot read 'from' name for matrix %s", 
		name );
  }

  if( mint_next_string( file, "to", 2 ) ) {
    mint_check( !toname, "'to' given twice for matrix %s", name );
    toname = mint_string_load( file );
    mint_check( toname, "cannot read 'to' name for matrix %s", 
		name );
  }

  ops = mint_ops_load( file, mint_op_weights_any );

  /* have been asked for a sparse matrix? */
  if( mint_ops_find( ops, "sparse", mint_op_weights_init ) != -1 )
    sparse = 1;

  /* check whether rows, cols, and states are specified in ops: */
  i = mint_ops_find( ops, "rows", mint_op_weights_init );
  if( i > -1 ) {
    op = mint_ops_get( ops, i );
    rows = mint_op_get_param( op, 0 );
  }
  i = mint_ops_find( ops, "cols", mint_op_weights_init );
  if( i > -1 ) {
    op = mint_ops_get( ops, i );
    cols = mint_op_get_param( op, 0 );
  }
  i = mint_ops_find( ops, "states", mint_op_weights_init );
  if( i > -1 ) {
    op = mint_ops_get( ops, i );
    states = mint_op_get_param( op, 0 );
  } else {
    states = 0;
    op = mint_op_new( "states", mint_op_weights_init );
    mint_op_set_param( op, 0, states );
    mint_ops_append( ops, op );
    mint_op_del( op );
  }

  /* now try to determine matrix columns, either thorugh the cols op
     or through the fromname info, if net is provided */
  n = 0;
  if( net ) {
    from = mint_network_nodes_find( net, fromname );
    mint_check( from != -1, "cannot find 'from' nodes %s in network", 
		fromname );
    n = mint_network_nodes( net, from );
    if( cols ) {
      mint_check( cols == mint_nodes_size( n ),
		  "'cols' and 'from' give different sizes!" );
    }
    cols = mint_nodes_size( n );
  }
  mint_check( cols != 0, "cannot determine matrix columns" );

  /* now try to determine matrix rows, either thorugh the rows op
     or through the toname info, if net is provided */
  n = 0;
  if( net ) {
    to = mint_network_nodes_find( net, toname );
    mint_check( to != -1, "cannot find 'to' nodes %s in network", 
		toname );
    n = mint_network_nodes( net, to );
    if( rows ) {
      mint_check( rows == mint_nodes_size( n ),
		  "'rows' and 'from' give different sizes!" );
    }
    rows = mint_nodes_size( n );
  }
  mint_check( rows != 0, "cannot determine matrix rows" );

  /* now we add cols and rows op for completeness, if not already
     present -- this is important if the matrix is later saved
     separate from the network */
  i = mint_ops_find( ops, "cols", mint_op_weights_init );
  if( i == -1 ) {
    op = mint_op_new( "cols", mint_op_weights_init );
    mint_op_set_param( op, 0, cols );
    mint_ops_append( ops, op );
    mint_op_del( op );
  }
  i = mint_ops_find( ops, "rows", mint_op_weights_init );
  if( i == -1 ) {
    op = mint_op_new( "rows", mint_op_weights_init );
    mint_op_set_param( op, 0, rows );
    mint_ops_append( ops, op );
    mint_op_del( op );
  }

  /* now we can finally create the weight matrix and set everything we
     know about it... */

  if( sparse )
    w = mint_weights_sparse_new( rows, cols, states );    
  else
    w = mint_weights_new( rows, cols, states );

  wstr = _STR(w);

  mint_string_del( wstr->name ); /* default set in mint_weights_alloc */
  wstr->name = name;
  wstr->from = from;
  wstr->to = to;
  wstr->ops = ops;

  /* set default operate if not on file */
  if( !mint_ops_count(wstr->ops, mint_op_weights_operate) ) {
    op = mint_op_new( "mult", mint_op_weights_operate );
    mint_ops_append( wstr->ops, op );
    mint_op_del( op );
  }

  mint_weights_init( w, 0, wstr->rows );

  mint_weights_load_values( w, file );

  mint_string_del( fromname );
  mint_string_del( toname );

  return w;
}

void mint_weights_save_values( const mint_weights w, FILE *f ) {
  int r, c, s, nval;
  struct mint_weights_str *wstr;
  wstr = _STR(w);
  for( r=0; r<wstr->rows; r++ ) {
    if( mint_weights_is_sparse(w) ) {
      fprintf( f, "%d ", wstr->rlen[r] );
      for( c=0; c<wstr->rlen[r]; c++ )
	fprintf( f, "%d ", wstr->cind[r][c] );
      nval = wstr->rlen[r];
    } else /* full matrix */
      nval = wstr->cols;
    for( s=0; s<1+wstr->states; s++ ) {
      for( c=0; c<nval; c++ ) 
	fprintf( f, "%g ", w[s][r][c] );
    }
    fprintf( f, "\n" );
  }
}

void mint_weights_info( const mint_weights w, FILE *f,
			struct mint_network *net ) {
  struct mint_weights_str *wstr = _STR( w );
  mint_nodes n;
  mint_string name;

  fprintf( f, "weights %s", wstr->name );

  /* save 'to' and 'from' if available from the network */
  if( mint_string_find( wstr->name, '-' ) == -1 && net ) {
    if( wstr->from > -1 ) {
      n = mint_network_nodes( net, wstr->from );
      name = mint_nodes_get_name( n );
      fprintf( f, " from %s",  name );
    }
    if( wstr->to > -1 ) {
      n = mint_network_nodes( net, wstr->to );
      name = mint_nodes_get_name( n );
      fprintf( f, " to %s", name );
    }
  }

  fprintf( f, "\n" );
  mint_ops_save( wstr->ops, f );
}

void mint_weights_save( const mint_weights w, FILE *f,
			struct mint_network *net ) {
  mint_weights_info( w, f, net );
  mint_weights_save_values( w ,f );
}

int mint_weights_rows( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->rows;
}

int mint_weights_cols( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->cols;
}

int mint_weights_states( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->states;
}

int mint_weights_get_from( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->from;
}

mint_string mint_weights_get_name( const mint_weights w ) {
  return _STR(w)->name;
}

int mint_weights_get_to( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->to;
}

int mint_weights_get_target( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->target;
}

void mint_weights_set_from( mint_weights w, int i ) {
  struct mint_weights_str *wstr = _STR(w);
  wstr->from = i;
}

void mint_weights_set_to( mint_weights w, int i ) {
  struct mint_weights_str *wstr = _STR(w);
  wstr->to = i;
}

void mint_weights_set_target( mint_weights w, int i ) {
  struct mint_weights_str *wstr = _STR(w);
  wstr->target = i;
}

struct mint_ops *mint_weights_get_ops( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->ops;
}

int mint_weights_is_sparse( const mint_weights w ) {
  struct mint_weights_str *wstr;
  wstr = _STR(w);
  return wstr->rlen ? 1 : 0;
}

int mint_weights_nonzero( const mint_weights w ) {
  int r, c, count;
  struct mint_weights_str *wstr;
  wstr = _STR(w);

  count = 0;

  if( mint_weights_is_sparse(w) ) {
    for( r=0; r<mint_weights_rows(w); r++ )
      count += wstr->rlen[r];
  } else {
    for( r=0; r<mint_weights_rows(w); r++ ) {
      for( c=0; c<mint_weights_cols(w); c++ ) {
	if( w[0][r][c] != 0 )
	  count++;
      }
    }
  }

  return count;
}

void mint_weights_set_row( mint_weights w, int r, int newlen,
			   float *newval, int *newind,
			   int var ) {
  int i, states;
  int *ind;
  struct mint_weights_str *wstr = _STR(w);

  mint_check( newlen >= 0, "newlen is negative" );

  states = mint_weights_states( w );

  if( newind ) 
    ind = newind;
  else {
    ind = malloc( newlen*sizeof(int) );
    for( i=0; i<newlen; )
      ind[i] = i;
  }

  if( mint_weights_is_sparse(w) ) {

     /* reallocate memory if needed */
    if( wstr->rlen[r] != newlen ) {
      wstr->cind[r] = realloc( wstr->cind[r], newlen*sizeof(int) );
      for( i=0; i<1+states; i++ )
	w[i][r] = realloc( w[i][r], newlen*sizeof(float) );
    }
    /* copy new values */
    if( newlen ) {
      memcpy( wstr->cind[r], ind, newlen*sizeof(int) );
      memcpy( w[ var ][r], newval, newlen*sizeof(float) );
    }
    wstr->rlen[r] = newlen;

  } else {

    /* set values to zero */
    for( i=0; i<wstr->cols; i++ )
      w[ var ][r][i] = 0;
    /* set new values */
    if( newlen ) {
      if( !newind )
	memcpy( w[ var ][r], newval, newlen );
      else {
	for( i=0; i<newlen; i++ ) {
	  mint_check( ind[i] < wstr->cols, "index too large" );
	  w[ var ][r][ ind[i] ] = newval[i];
	}
      }
    }

  }

  if( !newind )
    free( ind );
}


int *mint_weights_colind( mint_weights w, int r ) {
  struct mint_weights_str *wstr = _STR(w);
  mint_check( r>=0 && r<wstr->rows, "row index out of range" );
  return mint_weights_is_sparse(w) ? wstr->cind[r] : 0;
}

int mint_weights_rowlen( mint_weights w, int r ) {
  struct mint_weights_str *wstr = _STR(w);
  mint_check( r>=0 && r<wstr->rows, "row index out of range" );
  return mint_weights_is_sparse(w) ? wstr->rlen[r] : wstr->cols;
}

void mint_weights_set( mint_weights w, int s, int r, int c, float x ) {
  int i, j, rlen;
  struct mint_weights_str *wstr;
  wstr = _STR(w);
  
  if( !mint_weights_is_sparse(w) ) {
    w[s][r][c] = x;
  } else {
    rlen = wstr->rlen[r];
    i = 0;
    while( i < rlen && wstr->cind[r][i] < c )
      i++;
    if( i < rlen ) { /* place already exists */
      w[s][r][i] = x;
    } else {
      /* copy values and states */
      for( j=0; j < 1 + wstr->states; j++ ) {
	w[j][r] = realloc( w[j][r], (rlen+1)*sizeof(float) );
	memmove( w[j][r]+i+1, w[j][r]+i, (rlen-i)*sizeof(float) );
	w[j][r][i] = 0;
      }
      w[s][r][i] = x;

      /* copy indices */
      wstr->cind[r] = realloc( wstr->cind[r], 
			       (rlen+1)*sizeof(int) );
      memmove( wstr->cind[r]+i+1, wstr->cind[r]+i, 
	       (rlen-i)*sizeof(int) );
      wstr->cind[r][i] = c;

      wstr->rlen[r]++; /* now we have one more value */
    }
  }
}


mint_weights mint_weights_prune( mint_weights src, float cutoff, int sparse ) {
  mint_weights dst;
  struct mint_weights_str *sstr, *dstr;
  int rows, cols, states, r, c, s, i;
  float *val;
  int *ind;

  rows = mint_weights_rows( src );
  cols = mint_weights_cols( src );
  states = mint_weights_states( src );

  if( sparse )
    dst = mint_weights_sparse_new( rows, cols, states );
  else
    dst = mint_weights_new( rows, cols, states );

  for( r=0; r<rows; r++ ) {

    /* allocate val and ind to largest possible size */
    i = mint_weights_rowlen( src, r );
    val = malloc( i * sizeof(float) );
    ind = malloc( i * sizeof(int) );
    
    i = 0; /* # values that pass cutoff */
    for( c=0; c<cols; c++ ) {
      if( fabs( src[0][r][c] ) > cutoff ) {
	val[ i ] = src[0][r][c];
	ind[ i++ ] = c;
      }
    }

    /* set values */ 
    mint_weights_set_row( dst, r, i, val, ind, 0 );
    
    /* copy states */
    for( s=1; s<states+1; s++ ) {
      for( c=0; c<i; c++ )
	val[c] = src[s][r][ ind[c] ];
      mint_weights_set_row( dst, r, i, val, ind, s );
    }

    free( val );
    free( ind );
  }
  
  dstr = _STR(dst);
  sstr = _STR(src);

  dstr->ops = mint_ops_dup( sstr->ops );

  return dst;
}

/** Freezing and unfreezing weights is achieved by adding or removing
    the "frozen" op from the ops list. This op does nothing in itself,
    but its presence is checked by mint_weights_update. If the frozen
    op is present, no update is performed. */
void mint_weights_freeze( mint_weights w, int f ) {
  struct mint_op *frz;
  struct mint_ops *ops = mint_weights_get_ops( w );

  if( f ) {
    if( mint_ops_find( ops, "frozen", mint_op_weights_update ) >= 0 )
      return; /* don't add if op already there */
    frz = mint_op_new( "frozen", mint_op_weights_update );
    mint_ops_append( ops, frz );
    mint_op_del( frz );
  } else
    mint_ops_del_name( ops, "frozen" );
}

int mint_weights_frozen( mint_weights w ) {
  struct mint_ops *ops = mint_weights_get_ops( w );
  return mint_ops_find( ops, "frozen", mint_op_weights_init ) >= 0;
}


mint_weights mint_weights_optimize( mint_weights w, mint_nodes pre, 
				    mint_nodes post, float cutoff ) {

  clock_t tdense, tsparse;
  int i, rows;
  int N = 10; /* number of operations */
  mint_weights wdense, wsparse;

  rows = mint_weights_rows( w );

  wsparse = mint_weights_prune( w, 1, cutoff );
  wdense = mint_weights_prune( w, 0, cutoff );

  /* before measuring operation speed, we run operate once in case
     there are some initializations still to be performed */ 
  mint_weights_operate( wdense, pre, post, 0, rows );
  mint_weights_operate( wsparse, pre, post, 0, rows );

  tdense = clock();
  for( i=0; i<N; i++ ) 
    mint_weights_operate( wdense, pre, post, 0, rows );
  tdense = clock() - tdense;

  tsparse = clock();
  for( i=0; i<N; i++ ) 
    mint_weights_operate( wsparse, pre, post, 0, rows );
  tsparse = clock() - tsparse;

  if( tdense < tsparse ) {
    mint_weights_del( wsparse );
    return wdense;
  } else {
    mint_weights_del( wdense );
    return wsparse;
  }
}

int mint_weights_get_property( mint_weights w, const char *prop, int i,
			       float *value ) {
  struct mint_ops *ops;
  int k;

  ops = mint_weights_get_ops( w );
  k = mint_ops_find( ops, prop, mint_op_weights_any );

  if( k == -1 ) 
    return 0;
  else if( value ) 
    *value = mint_op_get_param( mint_ops_get(ops, k), i );

  return 1;
}

int mint_weights_set_property( mint_weights w, const char *prop, int i,
			       float value ) {
  struct mint_ops *ops;
  int k;

  ops = mint_weights_get_ops( w );
  k = mint_ops_find( ops, prop, mint_op_weights_any );

  if( k == -1 ) 
    return 0;

  mint_op_set_param( mint_ops_get(ops, k), i, value );
  return -1;
}

void mint_weights_operate( mint_weights w, mint_nodes pre,
			   mint_nodes post, int rmin, int rmax ) {
  int i, nops;
  struct mint_ops *ops;
  struct mint_op *op;
  ops = mint_weights_get_ops( w );
  nops = mint_ops_size( ops );
  for( i=0; i<nops; i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_type( op ) == mint_op_weights_operate )
      mint_op_run( op , w, pre, post, rmin, rmax, 
		   mint_op_get_params( op ) );
  }
}

void mint_weights_update( mint_weights w, mint_nodes pre, mint_nodes post,
			  int rmin, int rmax ) {
  int i, nops;
  struct mint_ops *ops;
  struct mint_op *op;

  ops = mint_weights_get_ops( w );

  if( mint_ops_find( ops, "frozen", mint_op_weights_update ) >= 0 )
    return;

  nops = mint_ops_size( ops );
  for( i=0; i<nops; i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_type( op ) == mint_op_weights_update )
      mint_op_run( op, w, pre, post, rmin, rmax, 
		   mint_op_get_params( op ) );
  }
} 


void mint_weights_connect( mint_weights w, mint_nodes pre, 
			   mint_nodes post,
			   int rmin, int rmax ) {
  int i, nops;
  struct mint_ops *ops;
  struct mint_op *op;

  ops = mint_weights_get_ops( w );
  nops = mint_ops_size( ops );

  for( i=0; i<nops; i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_type( op ) == mint_op_weights_connect )
      mint_op_run( op, w, pre, post, rmin, rmax, 
		   mint_op_get_params( op ) );
  }
} 

void mint_weights_init( mint_weights w, int rmin, int rmax ) {
  int i, nops;
  struct mint_ops *ops;
  struct mint_op *op;

  ops = mint_weights_get_ops( w );
  nops = mint_ops_size( ops );

  for( i=0; i<nops; i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_type( op ) == mint_op_weights_init )
      mint_op_run( op, w, rmin, rmax, mint_op_get_params( op )  );
  }
}

#undef _STR
