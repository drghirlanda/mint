#include "mint.h"
#include <stdio.h>
#include <stdlib.h>

int main( void ) {
  FILE *f; 
  struct mint_network *net;
  f = fopen( "starthere.arc", "r" );
  net = mint_network_load( f );        /* load net from file */
  fclose( f );
  mint_network_save( net, stdout );    /* display the network */
  mint_network_del( net );             /* free memory */
  return EXIT_SUCCESS;
}
