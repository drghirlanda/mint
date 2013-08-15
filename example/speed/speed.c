#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <FreeImage.h>

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

  file = fopen( "speed.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  times = 2;
  while(1) {
    t1 = clock();
    for( i=0; i<times; i++ ) {
      receive_input( mint_network_nodes( net, 0 ) );
      mint_network_operate( net );
    }
    t2 = clock();
    t = (t2 - t1) / CLOCKS_PER_SEC;
    printf( "%f %.0f\n", t, times );
    if( t<0.95 )
      times *= 1.25;
    else if( t>1.05 )
      times *= 0.75;
    else 
      break;
  };

  mint_network_del( net );
  return 0;
}
