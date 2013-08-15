#include "network.h"

#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  struct mint_network *net1, *net2;

  file = fopen( "network1.arc", "r" );
  net1 = mint_network_load( file );
  fclose( file );

  file = fopen( "network2.arc", "r" );
  net2 = mint_network_load( file );
  fclose( file );

  mint_network_add( net1, 1, net2, 0, 0, 0 );

  mint_network_save( net1, stdout );

  mint_network_del( net1 );
  mint_network_del( net2 );

  return 0;
}
