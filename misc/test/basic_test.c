#include <mint/network.h>
#include <mint/random.h>
#include <stdio.h>
#include <time.h>


int main( void ) {
  FILE *file;
  struct mint_network *net;
  int i;

  mint_random_seed( time(0) );

  printf( "loading network... ");
  file = fopen( "input/basic_test.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  printf( "done\n" );

  for( i=0; i<10; i++ )
    mint_network_operate( net );

  printf( "saving network... ");  
  file = fopen( "output/basic_net", "w" );
  mint_network_save( net, file );
  fclose( file );
  printf( "done\n" );

  printf( "deleting network... " );
  mint_network_del( net );
  printf( "done\n" );

  return 0;
}
