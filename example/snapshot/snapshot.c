#include "mint.h"
#include <stdio.h>
#include <stdlib.h>

int main( void ) {
  FILE *f; 
  int i;

  mint_image_init();

  struct mint_network *net;
  f = fopen( "snapshot.arc", "r" );
  net = mint_network_load( f );        /* load net from file */
  fclose( f );
  mint_network_info( net, stdout );    /* display the network */

  for( i=0; i<20; i++ )
    mint_network_operate( net );

  mint_network_del( net );             /* free memory */
  return EXIT_SUCCESS;
}
