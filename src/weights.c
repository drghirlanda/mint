#include "weights.h"
#include "utils.h"
#include "random.h"
#include "op.h"
#include "wop.h"

#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include <time.h>

struct mint_weights_str {
  unsigned int rows;
  unsigned int cols;
  unsigned int states;
  unsigned int from;
  unsigned int to;
  unsigned int target; /* which 'to' variable results are stored in */
  struct mint_ops *ops;

  /* these are for sparse matrices */
  unsigned int **cind; /* cind[r][i] = col index of value i in row r */
  unsigned int *rlen;  /* rlen[r] = # elements in row r */
};

#define _STR( w ) ( (struct mint_weights_str *)w - 1 )

/* calculate the number of bytes needed to store a dense or sparse
   matrix of given size. */ 
size_t mint_weights_bytes( unsigned int rows, unsigned int cols, 
			   unsigned int states, int sparse ) {
  size_t b = 0;
  b += sizeof(struct mint_weights_str);
  b += (1+states) * sizeof(float **); /* w[s] */
  b += (1+states) * rows * sizeof(float *); /* w[s][r] */
  if( !sparse )
    b += rows * cols * (1+states) * sizeof(float); /* w[s][r][c] */
  return b;
}

/* this internal function handles memory allocation and basic setup
   for dense and sparse matrices */
static mint_weights mint_weights_alloc( unsigned int rows,
					unsigned int cols,
					unsigned int states,
					int sparse ) {
  mint_weights w;
  unsigned int s, r;
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
  
  return w;
}

mint_weights mint_weights_new( unsigned int rows, unsigned int cols, 
			       unsigned int states ) {
  mint_weights w;
  struct mint_weights_str *wstr;
  w = mint_weights_alloc( rows, cols, states, 0 );
  wstr = _STR(w);
  wstr->rlen = 0;
  wstr->cind = 0;
  return w;
}

mint_weights mint_weights_sparse_new( unsigned int rows, unsigned int cols, 
				      unsigned int states ) {
  mint_weights w;
  struct mint_weights_str *wstr;
  int r;
  w = mint_weights_alloc( rows, cols, states, 1 );
  wstr = _STR(w);
  wstr->rlen = malloc( rows*sizeof(unsigned int) );
  wstr->cind = malloc( rows*sizeof(unsigned int *) );
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
  unsigned int r, c, s, rows, cols, states;
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
  dstr->target = sstr->target;
  mint_ops_del( dstr->ops );
  dstr->ops = mint_ops_dup( sstr->ops );

  for( r=0; r<rows; r++ ) {
    if( mint_weights_is_sparse(src) )
      mint_weights_set_row( dst, r, sstr->rlen[r], src[0][r], sstr->cind[r] );
    else
      mint_weights_set_row( dst, r, cols, src[0][r], 0 );
    for( s=1; s<1+states; s++ ) {
      for( c=0; c<cols; c++ )
	dst[s][r][c] = src[s][r][c];
    }
  }
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
      wstr->cind[r] = malloc( wstr->rlen[r] * sizeof(unsigned int) );
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

mint_weights mint_weights_load( FILE *f ) {
  mint_weights w;
  struct mint_weights_str *wstr;
  struct mint_op *op;
  unsigned int rows, cols, states, i;
  long pos;
  char buf[7];

  i = fscanf( f, " weights %d %d %d", &rows, &cols, &states );
  mint_check( i==3, "cannot read weights geometry" );

  pos = ftell( f );
  i = fscanf( f, " %6s", buf );
  if( i != 1 || strncmp(buf,"sparse",6)!=0 ) {
    fseek( f, pos, SEEK_SET );
    w = mint_weights_new( rows, cols, states );
  } else {
    w = mint_weights_sparse_new( rows, cols, states );    
  }

  wstr = _STR(w);

  /* load ops, and set default operate if not on file */
  wstr->ops = mint_ops_load( f );
  if( !mint_ops_count(wstr->ops, mint_op_weights_operate) ) {
    if( mint_weights_is_sparse(w) ) {
      op = mint_op_new( "mult_sparse" );
      mint_ops_append( wstr->ops, op );
      mint_op_del( op );
    } else { /* full */
      op = mint_op_new( "mult" );
      mint_ops_append( wstr->ops, op );
      mint_op_del( op );
    }
  }

  if( mint_ops_find( wstr->ops, "from" ) == -1 )
    wstr->from = -1;

  if( mint_ops_find( wstr->ops, "to" ) == -1 )
    wstr->to = -1;

  mint_weights_init( w, 0, wstr->rows );
  mint_weights_load_values( w, f );

  return w;
}

void mint_weights_save_values( const mint_weights w, FILE *f ) {
  unsigned int r, c, s, nval;
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

void mint_weights_save( const mint_weights w, FILE *f ) {
  struct mint_weights_str *wstr = _STR( w );
  fprintf( f, "weights %d %d %d\n", 
	   wstr->rows, wstr->cols, wstr->states );
  if( mint_weights_is_sparse(w) )
    fprintf( f, "sparse " );
  mint_ops_save( wstr->ops, f );
  mint_weights_save_values( w, f );
}

unsigned int mint_weights_rows( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->rows;
}

unsigned int mint_weights_cols( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->cols;
}

unsigned int mint_weights_states( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->states;
}

unsigned int mint_weights_get_from( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->from;
}

unsigned int mint_weights_get_to( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->to;
}

unsigned int mint_weights_get_target( const mint_weights w ) {
  struct mint_weights_str *wstr = _STR(w);
  return wstr->target;
}

void mint_weights_set_from( mint_weights w, unsigned int i ) {
  struct mint_weights_str *wstr = _STR(w);
  wstr->from = i;
}

void mint_weights_set_to( mint_weights w, unsigned int i ) {
  struct mint_weights_str *wstr = _STR(w);
  wstr->to = i;
}

void mint_weights_set_target( mint_weights w, unsigned int i ) {
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
			   float *newval, unsigned int *newind ) {
  int i, j, states;
  unsigned int *ind;
  struct mint_weights_str *wstr = _STR(w);

  mint_check( newlen>0, "newlen is negative" );

  states = mint_weights_states( w );

  if( newind ) 
    ind = newind;
  else {
    ind = malloc( newlen*sizeof(unsigned int) );
    for( i=0; i<newlen; )
      ind[i] = i;
  }

  if( mint_weights_is_sparse(w) ) {

     /* reallocate memory if needed */
    if( wstr->rlen[r] != newlen ) {
      wstr->cind[r] = realloc( wstr->cind[r], newlen*sizeof(unsigned int) );
      for( i=0; i<1+states; i++ )
	w[i][r] = realloc( w[i][r], newlen*sizeof(float) );
    }
    /* copy new values */
    memcpy( wstr->cind[r], ind, newlen*sizeof(unsigned int) );
    memcpy( w[0][r], newval, newlen*sizeof(float) );
    /* set state values to zero */
    for( i=1; i<1+states; i++ ) {
      for( j=0; j<newlen; j++ )
	w[i][r][j] = 0;
    }
    wstr->rlen[r] = newlen;

  } else {

    /* set values and states to zero */
    for( j=0; j<1+states; j++ ) {
      for( i=0; i<wstr->cols; i++ )
	w[j][r][i] = 0;
    }
    /* set new values */
    if( !newind )
      memcpy( w[0][r], newval, newlen );
    else {
      for( i=0; i<newlen; i++ ) {
	mint_check( ind[i] < wstr->cols, "index too large" );
	w[0][r][ ind[i] ] = newval[i];
      }
    }

  }

  if( !newind )
    free( ind );
}


unsigned int *mint_weights_colind( mint_weights w, int r ) {
  struct mint_weights_str *wstr = _STR(w);
  mint_check( r>=0 && r<wstr->rows, "row index out of range" );
  return mint_weights_is_sparse(w) ? wstr->cind[r] : 0;
}

unsigned int mint_weights_rowlen( mint_weights w, int r ) {
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
    if( i == c ) { /* place already exists */
      w[s][r][c] = x;
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
			       (rlen+1)*sizeof(unsigned int) );
      memmove( wstr->cind[r]+i+1, wstr->cind[r]+i, 
	       (rlen-i)*sizeof(unsigned int) );
      wstr->cind[r][i] = c;

      wstr->rlen[r]++; /* now we have one more value */
    }
  }
}


mint_weights mint_weights_prune( mint_weights src, float cutoff, int sparse ) {
  mint_weights dst;
  struct mint_op *op;
  struct mint_weights_str *sstr, *dstr;
  int rows, cols, states, r, c, s, i;
  float *val;
  unsigned int *ind;

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
    ind = malloc( i * sizeof(unsigned int) );
    
    i = 0; /* # values that pass cutoff */
    for( c=0; c<cols; c++ ) {
      if( fabs( src[0][r][c] ) > cutoff ) {
	val[ i ] = src[0][r][c];
	ind[ i++ ] = c;
      }
    }

    /* create row in dst (states allocated, but set to 0) */ 
    mint_weights_set_row( dst, r, i, val, ind );
    
    /* copy states */
    for( c=0; c<i; c++ ) {
      for( s=1; s<states+1; s++ )
	dst[s][r][c] = src[s][r][ ind[c] ];
    }
    
    free( val );
    free( ind );
  }
  
  dstr = _STR(dst);
  sstr = _STR(src);

  dstr->ops = mint_ops_dup( sstr->ops );

  /* replace mult op if appropriate */
  if( sparse ) {
    i = mint_ops_find( dstr->ops, "mult" );
    if( i != -1 ) {
      op = mint_op_new( "mult_sparse" );
      mint_ops_set( dstr->ops, i, op );
      mint_op_del( op );
    }
  } else {
    i = mint_ops_find( dstr->ops, "mult_sparse" );
    if( i != -1 ) {
      op = mint_op_new( "mult" );
      mint_ops_set( dstr->ops, i, op );
      mint_op_del( op );
    }
  }

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
    if( mint_ops_find( ops, "frozen" ) >= 0 )
      return; /* don't add if op already there */
    frz = mint_op_new( "frozen" );
    mint_ops_append( ops, frz );
    mint_op_del( frz );
  } else
    mint_ops_del_name( ops, "frozen" );
}

int mint_weights_frozen( mint_weights w ) {
  struct mint_ops *ops = mint_weights_get_ops( w );
  return mint_ops_find( ops, "frozen" ) >= 0;
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

#undef _STR
