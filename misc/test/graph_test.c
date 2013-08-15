#include <mint.h>
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;

  mint_random_seed( time(0) );

  file = fopen( "input/graph_test.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  file = fopen( "output/graph_test.dot", "w" );
  mint_network_graph_full( net, file );
  fclose( file );

  mint_network_del( net );
  return 0;
}
