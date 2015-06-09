#include "network.h"
#include "image.h"
#include "camera.h"

#include <stdio.h>
#include <unistd.h>

int main( void ) {
  struct mint_network *net;
  FILE *file;
  
  mint_image_init();
  mint_camera_init();

  file = fopen( "camera.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  for( ;; ) {
    mint_network_operate( net );
  }

  mint_network_del( net );

  return 0;
}
