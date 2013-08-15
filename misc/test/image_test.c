#include "image.h"
#include "nodes.h"
#include <stdio.h>
#include <time.h>


int main( void ) {
  struct mint_image *image;
  mint_nodes nred, ngreen, nblue;
  mint_weights w;
  FILE *file;

  nred = mint_nodes_new( 10, 0 );
  ngreen = mint_nodes_dup( nred );
  nblue = mint_nodes_dup( nred ); 

  ngreen[1][4] = 1;
  nred[1][0] = 1;
  nblue[1][9] = 1;

  image = mint_image_nodes( nred, ngreen, nblue, 2, 1 );
  mint_image_scale( image, 30 );
  mint_image_save( image, "output/nodes.jpg", FIF_JPEG );
  mint_image_del( image );

  mint_nodes_del( nred );
  mint_nodes_del( ngreen );
  mint_nodes_del( nblue );

  file = fopen("input/image_weights.arc", "r" );
  w = mint_weights_load( file );
  fclose( file );

  image = mint_image_weights( w, 0, 0 );
  mint_image_scale( image, 30 );
  mint_image_save( image, "output/image_weights.jpg", FIF_JPEG );

  mint_weights_del( w );
  mint_image_del( image );

  return 0;
}
