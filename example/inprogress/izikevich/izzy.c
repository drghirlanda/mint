#include "mint.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  struct mint_network *net;
  mint_nodes exc;
  struct mint_op *exc_noise;
  int i, size;
  int t;

  mint_random_seed( time(0) );

  file = fopen("izzy.arc", "r");
  net = mint_network_load( file );
  fclose( file );

  /* some handy aliases */
  exc = mint_network_nodes(net,0);
  i = mint_ops_find( mint_nodes_get_ops( exc ), "noise" );
  exc_noise = mint_ops_get( mint_nodes_get_ops( exc ), i );

  /* initialize izzy nodes at resting potential */
  mint_nodes_set( mint_network_nodes(net,0), 2, -65 );
  mint_nodes_set( mint_network_nodes(net,1), 2, -65 );

  file = fopen( "output/izzyArc.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );

  file = fopen( "output/izzy.dat", "w" );
  for( t=0; t<1000; t++ ) {
    if( t==400 )
      mint_op_set_param( exc_noise, 1, 0.5 );
    else if( t==600 )
      mint_op_set_param( exc_noise, 1, 0 );      
    mint_network_operate( net ); 
    fprintf( file, "%d ", t );
    mint_nodes_save_var( mint_network_nodes(net,1), 2, file );
    fprintf( file, "\n" );
  }

  file = fopen( "output/netwrok.arc", "w" );
  mint_network_save( net, file );
  fclose( file );

  return 0;
}
