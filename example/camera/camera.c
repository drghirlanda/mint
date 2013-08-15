#include "camera.h"
#include "image.h"
#include "network.h"
#include "random.h"

#include <FreeImage.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main( void ) {
  struct mint_image *img1, *img2, *img;
  struct mint_network *net;
  int i;
  FILE *arc;

  float d1, d2;

  d1 = 1;
  d2 = 0;

  mint_nodes R, G, B, OUT;

  img1 = mint_camera_image();
  sleep( 2 );
  img2 = mint_camera_image();

  arc = fopen( "network.arc", "r" );
  net = mint_network_load( arc );

  R = mint_network_nodes( net, 0 );
  G = mint_network_nodes( net, 1 );
  B = mint_network_nodes( net, 2 );
  OUT = mint_network_nodes( net, 3 );

  for( i=0; i<10; i++ ) {
    if( mint_random() < 0.5 ) {
      img = img1;
      mint_nodes_set( OUT, 2, d1 );
    } else {
      img = img2;
      mint_nodes_set( OUT, 2, d2 );
    }
    mint_image_paste( img1, R, G, B, 1, 320, 0, 0);
    mint_network_operate( net );
  }

  mint_network_del( net );

  return 0;
}
