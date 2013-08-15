#include "network.h"
#include "random.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;
  int t;

  /* init random number generator */
  mint_random_seed( time(0) );

  /* read network architecture from file */
  file = fopen( "clocked.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  for( t=0; t<10; t++ ) {
    mint_network_operate( net ); /* this function updates the network */
    printf( "%d\n", t );
    fflush( stdout );
  }

  mint_network_del( net );
  return 0;
}
