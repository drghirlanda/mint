#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <FreeImage.h>

void deltarule_auto( mint_weights w, mint_nodes pre, 
		     mint_nodes post ) {
  int r, c, i, j;
  struct mint_update *u;
  float lrate;
  u = mint_weights_get_update(w);
  lrate = mint_update_get_param( u, 0 );
  r = mint_weights_rows( w );
  c = mint_weights_cols( w );
  for( i=0; i<r; i++ ) {
    for( j=0; j<c; j++ )
      w[0][i][j] += lrate * ( post[2][i] - post[1][i] ) * pre[1][j];
  }
}

/* set input and return desired output */
float receive_input( struct mint_network *net ) {
  int i, size;
  float x1, x2, reward;
  mint_nodes in, val;
  in = mint_network_nodes( net, 0 );
  val = mint_network_nodes( net, 2 );
  size = mint_nodes_size( in );
  if( mint_random()<0.5 ) {
    x1 = 1;
    x2 = 0;
    reward = 1;
  } else {
    x1 = 0;
    x2 = 1;
    reward = 0;
  }
  val[1][0] = reward;
  for( i=0; i<size/2; i++ ) 
    in[1][i] = x1;
  for( ; i<size; i++ ) 
    in[1][i] = x2;
  return reward;
}

int main( void ) {
  struct mint_image *image;
  FILE *file, *file2;
  char filenamebuf[256];
  float reward;
  struct mint_update *upd;
  struct mint_network *net;
  mint_nodes 
    in,  /* sense organ */
    out; /* effectors */
  int t, i, size;

  /* add deltarule_auto to mint */
  reward = 0.1;
  mint_update_wadd( "delta_auto", deltarule_auto, 0, 1, &reward );

  mint_random_seed( time(0) );
  file = fopen( "deltarule.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  in = mint_network_nodes(net,0);
  out = mint_network_nodes(net,1);
  upd = mint_weights_get_update( mint_network_weights(net,0) );
  
  file = fopen( "output/deltaruleArc.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );

  file = fopen( "output/deltarule.dat", "w" );
  for( t=0; t<150; t++ ) {
    reward = receive_input( net );
    mint_network_nupdate( net );
    mint_network_wupdate( net );
    fprintf( file, "%d %f %f\n", t, reward, out[1][0] );
  }
  fclose( file );

  file = fopen( "output/deltaruleDetail.arc", "w" );
  mint_network_save( net, file );
  fclose( file );

  image = mint_image_weights( mint_network_weights(net,0), 0, 0 );
  mint_image_scale( image, 20 );
  mint_image_save( image, "output/weights.jpg", FIF_JPEG );

  mint_network_del( net );

  return 0;
}
