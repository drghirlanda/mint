#include "mint.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  float reward;
  struct mint_network *net;
  mint_nodes 
    in,  /* input */
    out; /* output */
  int t;

  mint_random_seed( time(0) );
  file = fopen( "featrev.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  in = mint_network_nodes( net, mint_network_nodes_find( net, "in" ) );
  out = mint_network_nodes( net, mint_network_nodes_find( net, "out" ) );
  
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
