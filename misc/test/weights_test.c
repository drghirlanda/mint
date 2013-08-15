#include <mint.h>
#include <stdio.h>

int main( void ) {
  int i, j, k;
  FILE *f;
  mint_weights n = mint_weights_new( 5, 2, 1 );
  mint_weights n2;

  /* this checks that pointers are set up correctly */
  k = 1;
  for( i=0; i<mint_weights_rows(n); i++ ) {
    for( j=0; j<mint_weights_cols(n); j++ ) {
      n[0][i][j] = k;
      n[1][i][j] = -k++;
    }
  }
  f = fopen( "output/w", "w" );
  mint_weights_save( n, f );
  fclose( f );

  /* this checks weight copying */
  n2 = mint_weights_new( mint_weights_rows(n), mint_weights_cols(n),
			 mint_weights_states(n) );
  mint_weights_cpy( n2, n );
  f = fopen( "output/w2", "w" );
  mint_weights_save( n2, f );
  fclose( f );

  /* now we check weight loading */
  mint_weights_del( n2 );
  f = fopen( "output/w", "r" );
  n2 = mint_weights_load( f );
  fclose( f );
  f = fopen( "output/wrestore", "w" );
  mint_weights_save( n2, f );
  fclose( f );

  mint_weights_del( n );
  mint_weights_del( n2 );
  return 0;
}
