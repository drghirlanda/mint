#include "network.h"
#include "pi.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;
  mint_nodes n;
  int i;

  mint_pi_init();

  /* read network architecture from file */
  file = fopen( "gpiosensor.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  mint_network_info( net, stdout );

  n = mint_network_nodes( net, 0 );

  for( i=0; i<30; i++ ) {
    mint_network_operate( net ); 
    printf( "%f\n", n[1][0] );
    sleep( 5 );
  }

  mint_network_del( net );
  return 0;
}
