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
  file = fopen( "servomotor.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  mint_network_save( net, stdout );

  n = mint_network_nodes( net, 0 );

  printf( "centering motor and waiting 5s\n" );
  n[1][0] = 0.5;
  mint_network_operate( net ); 
  sleep( 5 );

  for( i=0; i<10; i++ ) {

    printf( "driving motor counterclockwise for 5s\n" );
    n[1][0] = 0;
    mint_network_operate( net ); 
    sleep( 5 );

    printf( "driving motor clockwise for 5s\n" );
    n[1][0] = 1;
    mint_network_operate( net ); 
    sleep( 5 );

    printf( "returning to center\n" );
    n[1][0] = 0.5;
    mint_network_operate( net ); 
    sleep( 5 );
  }

  mint_network_del( net );
  return 0;
}
