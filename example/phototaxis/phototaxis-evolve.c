#include <mint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "physiology.c"
#include "simulation.c"

long tmax = 1000;
float activity = 1;
float loss = 0.01;
float mutlevel = .05;

float performance( struct mint_network *net ) {
  return simulation( net, tmax, 0 );
}

void mutation( struct mint_network *net ) {
  int i, r, c;
  mint_weights w;
  i = mint_random_int( 0, mint_network_matrices(net) );
  w = mint_network_weights(net, i);
  r = mint_random_int( 0, mint_weights_rows(w) );
  c = mint_random_int( 0, mint_weights_cols(w) );
  w[0][r][c] += mint_random_normal( 0, mutlevel );
}

int main( void ) {
  struct mint_network *net;
  FILE *f;
  float score;

  mint_random_seed( time(0) );

  mint_update_nadd( "tonic", tonic, 0, 1, &activity );
  mint_update_nadd( "light_sensor", light_sensor, 0, 0, 0 );
  mint_update_nadd( "metabolism", metabolism, 1, 1, &loss );

  f = fopen("phototaxis-evolve.arc","r");
  net = mint_network_load( f );
  fclose( f );

  printf( "initial score: %f\n", performance(net) );
  mint_random_search( net, performance, mutation, 1e6, 10000 );
  printf( "final score: %f\n", performance(net) );

  f = fopen("output/phototaxis-evolve.dot","w");
  mint_network_graph_full( net, f );
  fclose( f );

  f = fopen("output/phototaxis-evolve.dat","w");
  simulation( net, tmax, f );
  fclose( f );

  mint_network_del( net );
  return 0;
}
