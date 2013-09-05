#include <mint.h>
#include <stdio.h>

int main( void ) {
  FILE *f;
  struct mint_network *net;
  int i, iterations = 1e4;

  f = fopen( "sparse.arc", "r" );
  net = mint_network_load( f );
  fclose( f );

  mint_nodes_set( mint_network_nodes(net,0), 1, 0.5 );

  for( i=0; i<iterations; i++ )
    mint_network_operate( net );

  mint_network_del( net );
  return 0;
}
