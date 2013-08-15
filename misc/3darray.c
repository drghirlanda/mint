/* $Id: 3darray.c,v 1.10 2005/03/08 08:24:53 stefano Exp stefano $ */

#include "3darray.h"
#include "utils.h"
#include "random.h"

#include <string.h>
#include <stdlib.h>

struct mint_3darray_str {
  size_t x;
  size_t y;
  size_t z;
};

#define _STR( a ) ( (struct mint_3darray_str *)a - 1 )

size_t mint_3darray_bytes( size_t x, size_t y, size_t z ) {
  size_t b = 0;
  b += sizeof(struct mint_3darray_str);
  b += z * sizeof(float **); /* w[z] */
  b += z * x * sizeof(float *); /* w[z][x] */
  b += x * y * z * sizeof(float); /* w[z][x][y] */
  return b;
}

mint_3darray mint_3darray_new( size_t x, size_t y, size_t z ) {
  size_t s, r;
  char *start1, *start2;
  struct mint_3darray_str *astr;
  mint_3darray a;
  mint_check( x>0, "number of rows is negative or zero" );
  mint_check( y>0, "number of columns is negative or zero" );
  mint_check( z>0, "depth is negative or zero" );
  astr = malloc( mint_3darray_bytes(x, y, z) );
  mint_check( astr!=0, "out of memory" );
  astr->x = x;
  astr->y = y;
  astr->z = z;
  a = (mint_3darray ) ++astr;
  start1 = (char *)a + (1+z)*sizeof(float **);
  s = 0;
  do {
    a[s] = (float **)( start1 + s*x*sizeof(float *) );
    start2 = start1 + z*x*sizeof(float *);
    r = 0;
    do a[s][r] = (float *)( start2 + (s*x*y + r*y)*sizeof(float) );
    while( r++ < x );
  } while( s++ < z );
  return a;
}

void mint_3darray_del( mint_3darray a ) {
  mint_check( a!=0, "null 1st arg" );
  struct mint_3darray_str *astr = _STR( a );
  free( astr );
}

mint_3darray mint_3darray_dup( const mint_3darray src ) {
  mint_3darray dst = mint_3darray_new( _STR(src)->x,
    _STR(src)->y, _STR(src)->z );
  mint_3darray_cpy( dst, src );
  return dst;
}

void mint_3darray_cpy( mint_3darray dst, const mint_3darray src ) {
  int r, c, s;
  struct mint_3darray_str *dstr, *sstr;
  if( dst == src ) return;
  sstr = _STR( src );
  dstr = _STR( dst );
  r = dstr->x;
  c = dstr->y;
  s = dstr->z;
  mint_check( r == sstr->x, "different number of rows" );
  mint_check( c == sstr->y, "different number of columns" );
  mint_check( s == sstr->z, "different number of states" );
  memcpy( dst[0][0], src[0][0], s*r*c*sizeof(float) );
}

mint_3darray mint_3darray_load( FILE *f ) {
  mint_3darray a;
  int x, y, z, r, c, s, i;

  mint_skip_space( f );
  i = fscanf( f, "3darray %d %d %d", &x, &y, &z );
  mint_check( i==3, "cannot read 3darray geometry" );
  a = mint_3darray_new( x, y, z );

  if( mint_values_waiting(f) ) {
    for( s=0; s<z; s++ ) {
      for( r=0; r<x; r++ ) {
	for( c=0; c<y; c++ ) {
          i = fscanf( f, " %f", a[s][r]+c );
          mint_check( i==1, "cannot read values" );
        }
      }
    }
  }

  return a;
}

void mint_3darray_save( const mint_3darray a, FILE *f ) {
  int r, c, s, i;
  struct mint_3darray_str *astr = _STR( a );
  fprintf( f, "3darray %d %d %d\n", astr->x, astr->y, astr->z );
  for( s=0; s<astr->z; s++ ) {
    for( r=0; r<astr->x; r++ ) {
      for( c=0; c<astr->y; c++ ) {
        i = fprintf( f, "%f ", a[s][r][c] );
	mint_check( i>0, "cannot write to file" );
      }
      fprintf( f, "\n" );
    }
    fprintf( f, "\n" );
  }
}

size_t mint_3darray_x( const mint_3darray a ) {
  return _STR( a )->x;
}

size_t mint_3darray_y( const mint_3darray a ) {
  return _STR( a )->y;
}

size_t mint_3darray_z( const mint_3darray a ) {
  return _STR( a )->z;
}

void mint_3darray_set( mint_3darray a, float v ) {
    int ix, iy, iz;
    for( iz=0; iz<_STR(a)->z; iz++ ) {
        for( ix=0; ix<_STR(a)->x; ix++ ) {
            for( iy=0; iy<_STR(a)->y; iy++ )
                a[iz][ix][iy] = v;
        }
    }
}

void mint_3darray_random( mint_3darray a, float min, float max ) {
    int ix, iy, iz;
    for( iz=0; iz<_STR(a)->z; iz++ ) {
        for( ix=0; ix<_STR(a)->x; ix++ ) {
            for( iy=0; iy<_STR(a)->y; iy++ )
                a[iz][ix][iy] = mint_random_uniform( min, max );
        }
    }
}

#undef _STR
