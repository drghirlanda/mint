#include "array.h"
#include "utils.h"
#include "random.h"

#include <string.h>
#include <stdlib.h>

struct mint_array_str {
  size_t ndim;  /* how many dimensions */
  size_t *size; /* size of each dimension */
  void *extra;  /* pointer to extra info (allocated and deallocated
		   externally) */
};

#define _STR( a ) ( (struct mint_array_str *)a - 1 )

/* calculate size in bytes of array with given dimensions */
size_t mint_array_bytes( size_t ndim, size_t *size ) {
  size_t b;
  b = sizeof(struct mint_array_str);
  if( ndim==1 ) {
    b += size[0] * sizeof(float);
  } else if( ndim==2 ) {
    b += size[0] * sizeof(float *);
    b += size[0] * size[1] * sizeof(float);
  } else if( ndim==3 ) {
    b += size[0] * sizeof(float **);
    b += size[0] * size[1] * sizeof(float *);
    b += size[0] * size[1] * size[2] * sizeof(float);
  }
  return b;
}

/* how many values in this array (product of array dimensions) */
size_t mint_array_value_count( mint_array a ) {
  size_t count, s;
  struct mint_array_str *astr = _STR(a);
  count = 1;
  for( s=0; s<astr->ndim; s++ ) {
    count *= astr->size[s];
  }
  return count;
}

mint_array mint_array_new( size_t ndim, size_t *size ) {
  size_t s0, s1;
  char *p0, *p1;
  struct mint_array_str *astr;
  float *a1, **a2, ***a3;
  mint_array a = 0;

  mint_check( ndim>0, "dimensions must be >0" );
  mint_check( ndim<4, "only up to 3 dimensions supported" );

  astr = malloc( mint_array_bytes( ndim, size ) );
  mint_check( astr!=0, "out of memory" );
  astr->ndim = ndim;
  astr->size = malloc( ndim*sizeof(size_t) );
  mint_check( astr->size!=0, "out of memory" );
  for( s0=0; s0<ndim; s0++ ) 
    astr->size[s0] = size[s0];
  astr->extra = 0;

  /* the following sets up pointers in the memory just allocated so
     that the return value a behaves like an ndim-dimensional array:
     ndim==1 -> a[i], ndim==2 -> a[i][j], ndim==3 -> a[i][j][k] */
  if( ndim==1 ) {
    a1 = (float *) (astr+1);
    a = (mint_array)a1;
  } else if( ndim==2 ) {
    a2 = (float **) (astr+1);
    /* bytes from a to (a+p0) contain float * pointers, the following
       bytes contain floats that are accessed through such pointers */
    p0 = (char *)a2 + size[0]*sizeof(float *);
    for( s0=0; s0<size[0]; s0++ ) {
      a2[s0] = (float *)( p0 + s0*size[1]*sizeof(float) );
    }
    a = (mint_array)a2;
  } else if( ndim==3 ) {
    a3 = (float ***) (astr+1);
    /* bytes from a to (a+p0) contain float ** pointers which point to
       bytes between (a+p0) and (a+p1), which contain float * pointers
       that point bytes past (a+p1), which contain floats :) */
    p0 = (char *)a3 + size[0]*sizeof(float **);
    for( s0=0; s0<size[0]; s0++ ) {
      a3[s0] = (float **)( p0 + s0*size[1]*sizeof(float *) );
      p1 = p0 + size[0]*size[1]*sizeof(float *);
      for( s1=0; s1<size[1]; s1++ ) {
	a3[s0][s1] = 
	  (float *)( p1 + (s0*size[1]*size[2] + s1*size[2])*sizeof(float) );
      }
    }
    a = (mint_array)a3;
  }
  return a;
}

void mint_array_del( mint_array a ) {
  mint_check( a!=0, "null 1st arg" );
  struct mint_array_str *astr = _STR( a );
  free( astr->size );
  free( astr );
}

mint_array mint_array_dup( const mint_array src ) {
  mint_array dst;
  struct mint_array_str *srcstr;
  
  srcstr = _STR(src);
  dst = mint_array_new( srcstr->ndim, srcstr->size );
  mint_array_cpy( dst, src );
  return dst;
}

void mint_array_cpy( mint_array dst, const mint_array src ) {
  size_t s, count;
  struct mint_array_str *dstr, *sstr;
  if( dst == src ) return;
  sstr = _STR( src );
  dstr = _STR( dst );
  mint_check( sstr->ndim==dstr->ndim, "different number of dimensions" );
  count = 1;
  for( s=0; s<sstr->ndim; s++ ) {
    mint_check( sstr->size[s]==dstr->size[s], "different sizes" );
    count *= sstr->size[s];
  }
  memcpy( dst, src, count*sizeof(float) );
}

mint_array mint_array_load( FILE *f ) {
  mint_array a;
  float *a1, **a2, ***a3;
  size_t ndim, size[3]; 
  size_t s0, s1, s2;
  int i;

  mint_skip_space( f );
  i = fscanf( f, "array %d", &ndim );
  mint_check( i==1, "cannot read array dimensions" );
  mint_check( ndim>0, "read negative dimensions" );
  mint_check( ndim<4, "read dimensions >3" );
  for( s0=0; s0<ndim; s0++ ) {
    i = fscanf( f, "%d", size+s0 );
    mint_check( i==1, "cannot read dimension sizes" );
  }
  a = mint_array_new( ndim, size );

  if( mint_values_waiting(f) ) {
    if( ndim==1 ) {
      a1 = (float *)a;
      for( s0=0; s0<size[0]; s0++ ) {
	i = fscanf( f, " %f", a1+s0 );
	mint_check( i==1, "cannot read values" );
      }
    } else if( ndim==2 ) {
      a2 = (float **)a;
      for( s0=0; s0<size[0]; s0++ ) {
	for( s1=0; s1<size[1]; s1++ ) {
	  i = fscanf( f, " %f", a2[s0]+s1 );
	  mint_check( i==1, "cannot read values" );
	}
      }
    } else if( ndim==3 ) {
      a3 = (float ***)a;
      for( s0=0; s0<size[0]; s0++ ) {
	for( s1=0; s1<size[1]; s1++ ) {
	  for( s2=0; s2<size[2]; s2++ ) {
	    i = fscanf( f, " %f", a3[s0][s1]+s2 );
	    mint_check( i==1, "cannot read values" );
	  }
	}
      }
    }
  }
  return a;
}

void mint_array_save( const mint_array a, FILE *f ) {
  int i, s0, s1, s2;
  float *a1, **a2, ***a3;
  struct mint_array_str *astr = _STR( a );

  fprintf( f, "array %d ", astr->ndim );
  for( s0=0; s0<astr->ndim; s0++ ) {
    i = fprintf( f, "%d ", astr->size[s0] );
    mint_check( i>0, "cannot write to file" );
  }
  fprintf( f, "\n" );
  if( astr->ndim==1 ) {
    a1 = (float *)a;
    for( s0=0; s0<astr->size[0]; s0++ ) {
      i = fprintf( f, "%f ", a1[s0] );
      mint_check( i>0, "cannot write to file" );
    }
  } else if( astr->ndim==2 ) {
    a2 = (float **)a;
    for( s0=0; s0<astr->size[0]; s0++ ) {
      for( s1=0; s1<astr->size[1]; s1++ ) {
	i = fprintf( f, "%f ", a2[s0][s1] );
	mint_check( i>0, "cannot write to file" );
      }
      fprintf( f, "\n" );
    }
  } else if( astr->ndim==3 ) {
    a3 = (float ***)a;
    for( s0=0; s0<astr->size[0]; s0++ ) {
      for( s1=0; s1<astr->size[1]; s1++ ) {
	for( s2=0; s2<astr->size[2]; s2++ ) {
	  i = fprintf( f, "%f ", a3[s0][s1][s2] );
	  mint_check( i>0, "cannot write to file" );
	}
	fprintf( f, "\n" );
      }
    }
  }
}

size_t mint_array_size( mint_array a, size_t i ) {
  struct mint_array_str *astr = _STR(a);
  mint_check( i < astr->ndim, "requested size of dimension >ndim" )
  return astr->size[i];
}

void mint_array_set_slice( mint_array a, float v, int *start, int *end ) {
  size_t s0, s1, s2;
  size_t trueend[3];
  float *a1, **a2, ***a3;
  struct mint_array_str *astr = _STR(a); 

  /* check for ranges and for special value -1 in end */
  for( s0=0; s0<astr->ndim; s0++ ) {
    mint_check( start[s0]>=0, "start index is <0" );
    mint_check( start[s0]<astr->size[s0], "start index too large" );
    if( end[s0]==-1 ) {
      trueend[s0] = astr->size[s0];
    } else {
      trueend[s0] = end[s0];
    }
    mint_check( trueend[s0]>=0, "end index is <0" );
    mint_check( trueend[s0]<=astr->size[s0], "end index too large" );
  }

  if( astr->ndim==1 ) {
    a1 = (float *)a;
    for( s0=start[0]; s0<trueend[0]; s0++ )
      a1[s0] = v;
  } else if( astr->ndim==2 ) {
    a2 = (float **)a;
    for( s0=start[0]; s0<trueend[0]; s0++ ) {
      for( s1=start[1]; s1<trueend[1]; s1++ )
	a2[s0][s1] = v;
    }
  } else if( astr->ndim==3 ) {
    a3 = (float ***)a;
    for( s0=start[0]; s0<trueend[0]; s0++ ) {
      for( s1=start[1]; s1<trueend[1]; s1++ ) {
	for( s2=start[2]; s2<trueend[2]; s2++ )
	  a3[s0][s1][s2] = v;
      }
    }
  }
}


void *mint_array_get_extra( mint_array a ) {
  return _STR( a )->extra;
}

void mint_array_set_extra( mint_array a, void *extra ) {
  _STR( a )->extra = extra;
}

#undef _STR
