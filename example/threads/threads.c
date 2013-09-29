#include <mint.h>
#include <stdio.h>
#include <time.h>

int main( void ) {
  struct mint_network *net;
  FILE *file;
  int i;
  int iterations = 1000;

  mint_image_init();

  mint_random_seed( time(0) );

  file = fopen( "threads.arc", "r" );
  net = mint_network_load( file );

  mint_nodes_set( mint_network_nodes(net,0), 1, 0.5 );

  for( i=0; i<iterations; i++ )
    mint_network_operate( net );

  mint_network_del( net );
  return 0;
}
