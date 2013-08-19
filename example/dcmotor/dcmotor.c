#include "network.h"
#include "pi.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;
  mint_nodes n;

  mint_pi_init();

  /* read network architecture from file */
  file = fopen( "pi.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  mint_network_save( net, stdout );

  n = mint_network_nodes( net, 0 );

  printf( "driving motor in one direction for 5s\n" );
  n[0][0] = 1;
  mint_network_operate( net ); 
  sleep( 5 );

  printf( "driving motor in the other direction for 5s\n" );
  n[0][0] = 0;
  mint_network_operate( net ); 
  sleep( 5 );

  mint_network_del( net );
  return 0;
}
