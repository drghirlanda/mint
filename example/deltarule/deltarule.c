#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <FreeImage.h>

/* set input and return desired output */
float receive_input( struct mint_network *net ) {
  int i, size;
  float x1, x2, reward;
  mint_nodes in, val;
  in = mint_network_nodes( net, mint_network_nodes_find(net, "receptors") );
  val = mint_network_nodes( net, mint_network_nodes_find( net, "value" ) );
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
  mint_nodes_set( val, 1, reward );
  for( i=0; i<size/2; i++ ) 
    in[1][i] = x1;
  for( ; i<size; i++ ) 
    in[1][i] = x2;
  return reward;
}

int main( void ) {
  struct mint_image *image;
  FILE *file;
  float reward;
  struct mint_network *net;
  mint_nodes 
    in,  /* sense organ */
    out; /* effectors */
  int t;

  mint_random_seed( time(0) );
  file = fopen( "deltarule.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  in = mint_network_nodes( net, mint_network_nodes_find( net, "receptors" ) );
  out = mint_network_nodes( net, mint_network_nodes_find( net, "effectors" ) );
  
  file = fopen( "deltaruleArc.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );

  file = fopen( "deltarule.dat", "w" );
  for( t=0; t<150; t++ ) {
    reward = receive_input( net );
    mint_network_operate( net );
    fprintf( file, "%d %f %f\n", t, reward, out[1][0] );
  }
  fclose( file );

  file = fopen( "deltaruleDetail.arc", "w" );
  mint_network_save( net, file );
  fclose( file );

  image = mint_image_weights( mint_network_weights(net,0), 0, 0 );
  mint_image_scale( image, 20 );
  mint_image_save( image, "weights.jpg" );

  mint_network_del( net );

  return 0;
}
