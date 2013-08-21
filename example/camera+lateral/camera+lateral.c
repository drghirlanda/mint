#include "network.h"
#include "image.h"
#include "camera.h"

#include <stdio.h>
#include <FreeImage.h>

int main( void ) {
  struct mint_network *net;
  struct mint_image *img;
  FILE *file;
  mint_nodes R, G, B;
  int i;

  mint_camera_init();

  file = fopen( "camera+lateral.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  R = mint_network_nodes( net, 0 );
  G = mint_network_nodes( net, 1 );
  B = mint_network_nodes( net, 2 );

  for( i=0; i<10; i++ )
    mint_network_operate( net );

  img = mint_image_nodes( R, G, B, 0 );
  mint_image_save( img, "in.jpg", FIF_JPEG );
  mint_image_del( img );

  img = mint_image_nodes( R, G, B, 1 );
  mint_image_save( img, "out.jpg", FIF_JPEG );
  mint_image_del( img );
  
  mint_network_del( net );

  return 0;
}
