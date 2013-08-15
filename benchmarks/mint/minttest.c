#include <stdio.h>
#include <mint.h>

int main( void ) {
  struct mint_network *net;
  FILE *file;
  int i;

  file = fopen( "mint.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  for( i=0; i<1000; i++ )
    mint_network_operate( net );

  mint_network_del( net );
  return 0;
}
