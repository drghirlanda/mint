#include "random.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* BEGIN simple PRNG code adapted from

   http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf

*/

static unsigned int x=123456789, y=987654321, z=345678912, w=456789123, c=0;

float mint_random( void ) {
  float r;
  int t;
  y ^= (y<<5); 
  y ^= (y>>7); 
  y ^= (y<<22); 
  t = z + w + c; 
  z = w; 
  c = t < 0; 
  w = t & 2147483647;
  x += 1411392427;
  r = x + y + w;
  return r / UINT_MAX;
}

void mint_random_seed( unsigned long int seed ) {
  int i;
  x = y = z = seed;
  for (i=0; i<20; ++i)
    mint_random();
}

/* END Bob Jenkins' code */

float mint_random_uniform( float min, float max ) {
   mint_check( min<=max, "min is larger than max" );
   return (max-min)*mint_random() + min;
}

float mint_random_normal( float mean, float stdev )
{
  float x1, x2, w;
  mint_check( stdev>=0., "stdev must be >= 0." );
  if( stdev==0 )
    return mean;
  do {
    x1 = 2. * mint_random() - 1.;
    x2 = 2. * mint_random() - 1.;
    w = x1 * x1 + x2 * x2;
  } while ( w >= 1. );          
  w = sqrt( (-2. * log( w ) ) / w );
  return (x1 * w + mean) * stdev;
}

int mint_random_int( int min, int max )
{
  return (int)( mint_random_uniform( min, max ) );
}

void mint_random_permutation( int *perm, int min, int max ) {
  int i, j, m, n = max - min;
  mint_check( min<max, "min is larger than max" );
  for( i=0; i<n; i++ ) 
    perm[i] = min + i;
  for( i=n-1; i>0; i-- ) {
    j = mint_random_int( 0, i+1 );
    m = perm[j];
    perm[j] = perm[i];
    perm[i] = m;
  }
}

int mint_random_binomial( float p, int n ) {
  int k = 0;
  mint_check( n>=0, "number of trials must be >= 0" );
  while( n-- ) if( mint_random()<p ) k++;
  return k;
}

void mint_random_multinomial( float *p, int *r, int len, int n ) {
  int i;
  float *pr, sumpr, scale;
  
  /* set pr array and calculate sumpr for later */  
  pr = malloc( len*sizeof(int) );

  if( p==0 ) {
    for( i=0; i<len; i++ )
      pr[i] = 1./len;
    sumpr = 1.;
  } else {
    sumpr = 0;
    for( i=0; i<len; i++ ) {
      pr[i] = p[i];
      sumpr += p[i];
    }
  }

  scale  = 1;
  for( i=0; i<n; i++ ) {
    r[i] = mint_random_binomial( pr[i]/(sumpr*scale), n );
    n -= r[i];
    scale -= pr[i]/sumpr; 
  }

}

int mint_intcmp( const void *x, const void *y ) {
  int a = *(int *)x;
  int b = *(int *)y;
  return a - b;
}

int mint_uintcmp( const void *x, const void *y ) {
  unsigned int a = *(unsigned int *)x;
  unsigned int b = *(unsigned int *)y;
  return a - b;
}

void mint_random_subset( unsigned int *subset, unsigned int n, 
			 unsigned int min, unsigned int max, 
			 int sort ) {
  int *perm;
  perm = malloc( (max-min)*sizeof(int) );
  mint_random_permutation( perm, min, max );
  memcpy( subset, perm, n*sizeof(int) );
  if( sort==1 ) qsort( subset, n, sizeof(int), mint_intcmp );
  free( perm );
}

