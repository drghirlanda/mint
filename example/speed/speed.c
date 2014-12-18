#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

/* set input */
void receive_input( mint_nodes in ) {
  int i, size;
  size = mint_nodes_size( in );
  for( i=0; i<size; i++ ) 
    in[1][i] = mint_random();
}

int main( void ) {
  FILE *file;
  struct mint_network *net;
  int i;
  float times;
  float t, t1, t2;

  mint_random_seed( time(0) );

  printf( "creating network...\n" );
  fflush( stdout );
  file = fopen( "speed.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  printf( "...done\n" );

  file = fopen( "speed.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );

  mint_network_info( net, stdout );

  times = 2;
  while(1) {
    t1 = clock();
    for( i=0; i<times; i++ ) {
      receive_input( mint_network_get_nodes( net, 0 ) );
      mint_network_operate( net );
    }
    t2 = clock();
    t = (t2 - t1) / CLOCKS_PER_SEC;
    printf( "%f %.0f\n", t, times );
    if( times<1 )
      break;
    else if( t<0.95 )
      times *= 1.25;
    else if( t>1.05 )
      times *= 0.75;
    else 
      break;
  };

  mint_network_del( net );
  return 0;
}
