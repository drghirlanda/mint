#include <mint.h>
#include <stdio.h>
#include <time.h>

float perf( struct mint_network *net ) {
  mint_nodes n = mint_network_nodes( net, 0 );
  mint_nodes_set( n, 1, .1 );
  mint_network_operate( net );
  return mint_network_nodes( net, 1 )[1][0];
}

void mut( struct mint_network *net ) {
  mint_weights w = mint_network_weights( net, 0 );
  int c = mint_weights_cols( w );
  w[0][0][ mint_random_int(0,c) ] += mint_random_normal( 0., .1 );
}

void init_network( struct mint_network *net ) {
  int i, j, k, r, c;
  mint_weights w;
  int nw = mint_network_matrices(net);
  for( i=0; i<nw; i++ ) {
    w = mint_network_weights( net, 0 );
    r = mint_weights_rows( w );
    c = mint_weights_cols( w );
    for( j=0; j<r; j++ ) for( k=0; k<c; k++ )
      w[0][j][k] = mint_random_normal( 0, .1 );
  }
}

int main( void ) {
  FILE *file;
  struct mint_network *net1, *net2;
  float p;

  mint_random_seed( time(0) );

  printf( "loading network... ");
  file = fopen( "input/network_test.arc", "r" );
  net1 = mint_network_load( file );
  fclose( file );
  printf( "done\n" );

  printf( "init network... ");  
  init_network( net1 );
  printf( "done\n" );

  printf( "saving network... ");  
  file = fopen( "output/net1", "w" );
  mint_network_save( net1, file );
  fclose( file );
  printf( "done\n" );

  net2 = mint_network_dup( net1 );
  p = mint_random_search( net2, perf, mut, 1., 500000 );
  printf( "final output: %f\n", p );

  printf( "saving trained network... ");  
  file = fopen( "output/net2", "w" );
  mint_network_save( net2, file );
  fclose( file );
  printf( "done\n" );

  printf( "deleting naive network... " );
  mint_network_del( net1 );
  printf( "done\n" );
  fflush( stdout );

  printf( "deleting trained network... " );
  mint_network_del( net2 );
  printf( "done\n" );
  fflush( stdout );

  return 0;
}
