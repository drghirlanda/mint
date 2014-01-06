#include "network.h"
#include "image.h"
#include "camera.h"

#include <stdio.h>

int main( void ) {
  struct mint_network *net;
  struct mint_image *img;
  FILE *file;
  mint_nodes R, G, B;

  mint_image_init();
  mint_camera_init();

  file = fopen( "camera.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  R = mint_network_nodes( net, 0 );
  G = mint_network_nodes( net, 1 );
  B = mint_network_nodes( net, 2 );

  for( ;; ) {
    mint_network_operate( net );
    printf( "." );
    fflush( stdout );
  }

  img = mint_image_nodes( R, G, B, 1 );
  mint_image_save( img, "image.jpg" );
  mint_image_del( img );
  
  mint_network_del( net );

  return 0;
}
