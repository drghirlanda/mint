#include "spread.h"

#include <stdio.h>

int main( void ) {
  struct mint_spread *s1, *s2;

  s1 = mint_spread_new( 3 );
  mint_spread_save( s1, stderr );

  s2 = mint_spread_dup( s1 );
  mint_spread_save( s2, stderr );

  mint_spread_del( s1 );
  mint_spread_del( s2 );

  return 0;
}
