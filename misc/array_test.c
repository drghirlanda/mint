#include "array.h"
#include "image.h"

#include <stdio.h>
#include <math.h>

int main( void ) {
  int i, j, k, count;
  FILE *f;

  size_t size1[1] = {10};
  float *a1;

  size_t size2[2] = {4, 9};
  float **a2;

  size_t size3[3] = {2, 3, 4};
  float ***a3;

  /* test 1: fill a 1-dim array and save it */
  a1 = mint_array_new( 1, size1 );
  count = 0;
  for( i=0; i<mint_array_size(a1,0); i++ ) {
    a1[i] = count++;
  }
  f = fopen("dat/a1", "w");
  mint_array_save( a1, f );
  fclose( f );
  mint_array_del( a1 );

  /* reload */
  f = fopen("dat/a1", "r" );
  a1 = mint_array_load( f );
  fclose( f );
  mint_array_save( a1, stdout );
  
  /* test 2: fill a 2-dim array and save it */
  a2 = mint_array_new( 2, size2 );
  count = 0;
  for( i=0; i<mint_array_size(a2,0); i++ ) {
    for( j=0; j<mint_array_size(a2,1); j++ ) {
      a2[i][j] =count++;
    }
  }
  f = fopen("dat/a2", "w");
  mint_array_save( a2, f );
  mint_array_del( a2 );
  fclose( f );

  /* reload */
  f = fopen("dat/a2", "r" );
  a2 = mint_array_load( f );
  fclose( f );
  mint_array_save( a2, stdout );

  /* test 3: fill a 3-dim array and save it to file */
  a3 = mint_array_new( 3, size3 );
  count = 0;
  for( k=0; k<mint_array_size(a3,0); k++ ) {
    for( i=0; i<mint_array_size(a3,1); i++ ) {
      for( j=0; j<mint_array_size(a3,2); j++ ) {
	a3[k][i][j] = count++;
      }
    }
  }
  f = fopen("dat/a3", "w");
  mint_array_save( a3, f );
  fclose( f );
  mint_array_del( a3 );

  /* reload */
  f = fopen("dat/a3", "r" );
  a3 = mint_array_load( f );
  fclose( f );
  mint_array_save( a3, stdout );

  return 0;
}
