#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( void ) {
  FILE *file;
  struct mint_network *net, *net2;

  printf( "testing feedforward network operation\n" );
  file = fopen( "feedforward.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  mint_network_operate( net );
  mint_network_save( net, stdout );
  file = fopen( "feedforward.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );
  file = fopen( "feedforward_full.dot", "w" );
  mint_network_graph_full( net, file );
  fclose( file );
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

  /* testing duplication */
  net2 = mint_network_dup( net );
  mint_network_save( net2, stdout );
  mint_network_del( net2 );

  mint_network_del( net );
  return 0;
}
