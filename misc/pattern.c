#include "utils.h"
#include "random.h"
#include "pattern.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* structure to manage bidimensional patterns */
struct mint_pattern_str {
  int rows;    /* # of rows */
  int cols;    /* # of cols */
  float **values;    /* values */
};

#define _STR(p) ( (struct mint_pattern_str *)p - 1 )

/* size in bytes of a mint_pattern object, see mont_nodes for
   rationale */
static size_t mint_pattern_bytes( int rows, int cols ) {
  size_t b = 0;
  b += rows * cols * sizeof(float); /* values */
  b += rows * sizeof(float *);      /* pointers */
  b += sizeof(struct mint_pattern_str);       
  return b;
}

mint_pattern mint_pattern_new( int rows, int cols ) {
  size_t offset;
  struct mint_pattern_str *pstr;
  mint_pattern p;
  int s = 0;
  pstr = malloc( mint_pattern_bytes(rows, cols) );
  pstr->rows = rows;
  pstr->cols = cols;
  p = (mint_pattern) ++pstr;
  offset = rows*sizeof(float *);
  do {
    p[s] = (float *)( (char *)p + offset + s*cols*sizeof(float) );
  } while( s++ < rows );
  return p;
}

void mint_pattern_del( mint_pattern p ) {
  free( _STR(p) );
}

void mint_pattern_cpy( mint_pattern p1, const mint_pattern p2 ) {
  struct mint_pattern_str *pstr1, *pstr2;
  if( p1 == p2 ) return;
  pstr1 = _STR(p1);
  pstr2 = _STR(p2);
  mint_check( pstr1->rows == pstr2->rows, "different rows" );
  mint_check( pstr2->cols == pstr2->cols, "different cols" );
  memcpy( p1, p2, pstr1->rows * pstr1->cols * sizeof(float) );
}

mint_pattern mint_pattern_dup( const mint_pattern p1 ) {  
  mint_pattern p2;
  p2 = mint_pattern_new( _STR(p1)->rows, _STR(p1)->cols );
  mint_pattern_cpy( p2, p1 );
  return p2;
}

void mint_pattern_save( const mint_pattern p, FILE *dest ) {
  int i, j, count;
  count = fprintf( dest, "# pattern %d %d\n", _STR(p)->rows, 
		   _STR(p)->cols );
  mint_check( count>0, "cannot write pattern" );
  for( i=0; i<_STR(p)->rows; i++ ) {
    for( j=0; j<_STR(p)->cols; j++ ) {
      count = fprintf( dest, "%f ", p[i][j] );
      mint_check( count>0, "cannot write values" );
    }
    fprintf( dest, "\n" );
  }
  fprintf( dest, "\n" );
}

mint_pattern mint_pattern_load( FILE *source ) {
  int i, j, rows, cols, read;
  mint_pattern p;

  read = fscanf( source, " # pattern %d %d", &rows, &cols );
  mint_check( read == 2, "cannot read pattern geometry" );
  mint_check( rows>0 && cols>0, "rows/cols negative or zero size" );
  
  p = mint_pattern_new( rows, cols );
  for( i=0; i<rows; i++ ) for( j=0; j<cols; j++ ) {
    read = fscanf( source, " %f", p[i] + j );
    mint_check( read, "cannot read pattern values" );
  }
  return p;
}

void mint_pattern_paste( const mint_pattern p, mint_nodes n, int nr, 
			 float x, float y ) {
  int pr = _STR(p)->rows;
  int pc = _STR(p)->cols;
#ifndef NDEBUG
  int nc;
  int nsize = mint_nodes_size(n);
  nc = nsize / nr;
#endif
  int i, j;
  int X, Y;
  float xd, yd;

  mint_check( nsize%nr == 0, "nodes row does not divide nodes size" );
  mint_check( x>=0. && y>=0., "x and y offsets must be >= zero" );
  mint_check( pr<nr, "pattern too wide for retina" );
  mint_check( pc< nc, "patter too tall for retina" );
  mint_check( x+pr<=nr && y+pc<=nc, "pattern falls out of retina" );

  xd = x - floor(x);
  yd = y - floor(y);

  for( i=0; i<pr; i++ ) {
    X = (int)x + i;
    for( j=0; j<pc; j++ ) {
      Y = (int)y + j;
      n[1][X+nr*Y] += (1-xd)*(1-yd) * p[i][j];
      n[1][X+1+nr*Y] += xd*(1-yd) * p[i][j];
      n[1][X+1+nr*(Y+1)] += xd*yd * p[i][j];
      n[1][X+nr*(Y+1)] += (1-xd)*yd * p[i][j];
    }
  }
}

void mint_pattern_paste_random( const mint_pattern p, 
				mint_nodes n, int nr ) {
  int pr = _STR(p)->rows;
  int pc = _STR(p)->cols;
  int nc = mint_nodes_size(n)/nr;
  float x, y;
  x = mint_random_uniform( 0, nr-pr );
  y = mint_random_uniform( 0, nc-pc );
  mint_pattern_paste( p, n, nr, x, y );
}

void mint_pattern_paste_center( const mint_pattern p, 
				mint_nodes n, int nr ) {
  int pr = _STR(p)->rows;
  int pc = _STR(p)->cols;
  int nc = mint_nodes_size(n)/nr;
  float x, y;
  x = nr/2 - pr/2;
  y = nc/2 - pc/2;
  mint_pattern_paste( p, n, nr, x, y );
}
