#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;

  printf( "testing feedforward network operation\n" );
  file = fopen( "feedforward.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  mint_network_operate( net );
  mint_network_save( net, stdout );
  mint_network_del( net );

  printf( "\n\ntesting synchronous network operation\n" );
  file = fopen( "synchronous.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  mint_network_operate( net );
  mint_network_save( net, stdout );
  mint_network_del( net );

  printf( "\n\ntesting asynchronous network operation\n" );
  file = fopen( "asynchronous.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  mint_network_operate( net );
  mint_network_save( net, stdout );
  mint_network_del( net );

  printf( "\n\ntesting custom network operation\n" );
  file = fopen( "custom.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  mint_network_operate( net );
  mint_network_save( net, stdout );
  mint_network_del( net );

  return 0;
}
