#include "mint.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( void ) {
  FILE *f; 
  struct mint_network *net;
  int t;
  mint_nodes brain;

  f = fopen( "recnet.arc", "r" );
  net = mint_network_load( f );        /* load net from file */
  fclose( f );
  mint_network_save( net, stdout );    /* display the network */

  f = fopen( "recnet.dot", "w" );
  mint_network_graph( net, f );
  fclose( f );

  brain = mint_network_nodes( net, 1 ); /* handy shortcut */

  f = fopen( "recnet.dat", "w" );
  for( t=0; t<=100; t++ ) {
    mint_network_operate( net );
    fprintf( f, "%d %f\n", t, brain[1][0] );
  }
  fclose( f );

  mint_network_del( net );             /* free memory */
  return EXIT_SUCCESS;
}
