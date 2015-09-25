#include "mint.h"
#include <stdio.h>
#include <stdlib.h>

int main( void ) {
  FILE *f; 
  struct mint_network *net;
  mint_nodes input;

  mint_random_seed( 3 );

  f = fopen( "starthere.arc", "r" );
  net = mint_network_load( f );        /* load net from file */
  fclose( f );

  input = mint_network_find_nodes( net, "input" );

  input[1][0] = 1;

  mint_network_operate( net );

  mint_network_save( net, stdout );    /* display the network */
  mint_network_del( net );             /* free memory */
  return EXIT_SUCCESS;
}
