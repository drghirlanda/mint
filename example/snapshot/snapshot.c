#include "mint.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( void ) {
  FILE *f; 

  mint_random_seed( time(0) );

  mint_image_init();

  struct mint_network *net;
  f = fopen( "snapshot.arc", "r" );
  net = mint_network_load( f );        /* load net from file */
  fclose( f );
  mint_network_info( net, stdout );    /* display the network */

  for( ;; )
    mint_network_operate( net );

  mint_network_del( net );             /* free memory */
  return EXIT_SUCCESS;
}
