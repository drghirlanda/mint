#include "network.h"
#include "pi.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;

  mint_pi_init();

  /* read network architecture from file */
  file = fopen( "pi.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  mint_network_save( net, stdout );

  mint_network_operate( net ); 
  sleep( 5 );

  mint_network_del( net );
  return 0;
}
