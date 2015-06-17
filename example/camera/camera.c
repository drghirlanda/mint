#include "network.h"
#include "image.h"
#include "camera.h"

#include <stdio.h>
#include <unistd.h>

int main( void ) {
  struct mint_network *net;
  FILE *file;
  int i=0;

  mint_image_init();
  mint_camera_init();

  printf( "creating network... " );
  fflush(stdout);
  file = fopen( "camera.arc", "r" );
  net = mint_network_load( file );
  fclose( file );
  printf( "done\n" );
  fflush(stdout);

  printf( "running network... " );
  fflush(stdout);
  for( i=0; i<1000; i++ ) {
    mint_network_operate( net );
  }
  printf( "done\n" );
  fflush(stdout);

  printf( "deleting network... " );
  fflush(stdout);
  mint_network_del( net );
  printf( "done\n" );
  fflush(stdout);

  return 0;
}
