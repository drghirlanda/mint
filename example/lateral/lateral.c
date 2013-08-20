#include "network.h"
#include "image.h"

#include <stdio.h>
#include <FreeImage.h>

int main( void ) {
  struct mint_network *net;
  struct mint_image *img;
  mint_weights w;
  FILE *file;
  int rows;

  file = fopen( "lateral.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  mint_network_operate( net );
  mint_network_save( net, stderr );

  w = mint_network_weights( net, 0 );

  rows = mint_nodes_size( mint_network_nodes(net, 0) );
  img = mint_image_weights( w, rows, 0 );
  mint_image_save( img, "weights.bmp", FIF_BMP );
  mint_image_del( img );
  
  mint_network_del( net );

  return 0;
}
