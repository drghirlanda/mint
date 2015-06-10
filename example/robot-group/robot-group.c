#include "mint.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( void ) {
  FILE *f; 
  int i;
  struct mint_network *net;
  mint_nodes lr, motors;

  mint_random_seed( time(0) );

  f = fopen( "robot-group.arc", "r" );
  net = mint_network_load( f );        /* load net from file */
  fclose( f );
  mint_network_save( net, stdout );    /* display the network */

  f = fopen( "robot-group.dot", "w" );
  mint_network_graph( net, f );
  fclose( f );

  lr = mint_network_nodes( net, 0 );
  motors = mint_network_nodes( net, 1 );

  f = fopen( "output.dat", "w" );
  for( i=0; i<1000; i++ ) {
    mint_network_operate( net );
    fprintf( f, "%d %f %f %f %f\n", i,
	     lr[1][0],
	     lr[1][1],
	     motors[1][0], 
	     motors[1][1] );
  }
  fclose( f );

  mint_network_del( net );             /* free memory */
  return EXIT_SUCCESS;
}
