#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <FreeImage.h>

struct mint_image *fimg, *mimg;

/* set input and return desired output */
float receive_input( mint_nodes nred,
		     mint_nodes ngreen,
		     mint_nodes nblue ) {
  float x1, x2, reward;
  if( mint_random()<0.5 ) {
    mint_image_paste( fimg, nred, ngreen, nblue, 1, 0, 0 );
    reward = 1;
  } else {
    mint_image_paste( mimg, nred, ngreen, nblue, 1, 0, 0 );
    reward = 0;
  }
  return reward;
}

int main( void ) {
  struct mint_image *image;
  FILE *file;
  float reward;
  struct mint_update *ured, *ugreen, *ublue;
  struct mint_network *net;
  mint_nodes 
    nred, ngreen, nblue,  /* sense organ */
    out;                  /* effectors */
  int t, i, size, irows;

  fimg = mint_image_load( "favg-norm-rectmask.jpg" );
  mimg = mint_image_load( "mavg-norm-rectmask.jpg" );
  irows = FreeImage_GetHeight( mint_image_get_FreeImage(fimg) );

  mint_random_seed( time(0) );
  file = fopen( "facediscr.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  nred = mint_network_nodes(net,0);
  ngreen = mint_network_nodes(net,1);
  nblue = mint_network_nodes(net,2);
  out = mint_network_nodes(net,3);

  /* test image -> node copy */
  mint_image_paste( fimg, nred, ngreen, nblue, 1, 0, 0 );
  image = mint_image_nodes( nred, ngreen, nblue, irows, 1 );
  mint_image_save( image, "output/favg-reconstructed.jpg", FIF_JPEG );
  mint_image_del( image );

  ured = mint_weights_get_update( mint_network_weights(net,0) );
  ugreen = mint_weights_get_update( mint_network_weights(net,1) );
  ublue = mint_weights_get_update( mint_network_weights(net,2) );
  
  file = fopen( "output/facediscr.dat", "w" );
  for( t=0; t<1500; t++ ) {
    reward = receive_input( nred, ngreen, nblue );
    mint_network_nupdate( net );
    mint_update_set_param( ured, 1, reward );
    mint_update_set_param( ugreen, 1, reward );
    mint_update_set_param( ublue, 1, reward );
    mint_network_wupdate( net );
    fprintf( file, "%d %f %f\n", t, reward, out[1][0] );
  }
  fclose( file );

  file = fopen( "output/facediscrArc.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );

  file = fopen( "output/facediscrDetail.arc", "w" );
  mint_network_save( net, file );
  fclose( file );

  image = mint_image_weights( mint_network_weights(net,0), irows, 0 );
  mint_image_save( image, "output/wred.jpg", FIF_JPEG );
  mint_image_del( image );

  image = mint_image_weights( mint_network_weights(net,1), irows, 0 );
  mint_image_save( image, "output/wgreen.jpg", FIF_JPEG );
  mint_image_del( image );

  image = mint_image_weights( mint_network_weights(net,2), irows, 0 );
  mint_image_save( image, "output/wblue.jpg", FIF_JPEG );
  mint_image_del( image );

  mint_network_del( net );

  return 0;
}
