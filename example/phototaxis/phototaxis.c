#include <mint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

float step = .1;
float tmax = 1000;
float loss = 0.1;
float activity = 1;

void metabolism( mint_nodes n, int min, int max ) {
  int i;
  float loss;
  struct mint_update *u;
  u = mint_nodes_get_update(n);
  loss = mint_update_get_param( u, 0 );
  for( i=min; i<max; i++ ) {
    if( n[0][i]>0.80 ) 
      n[2][i] += 1.5*loss;
    else
      n[2][i] -= loss;
    if( n[2][i] > 1. )
      n[2][i] = 1.;
    else if( n[2][i] < 0. ) 
      n[2][i] = 0.;
    n[1][i] = n[2][i];
  }
}

void tonic( mint_nodes n, int min, int max ) {
  int i;
  float activity;
  struct mint_update *u;
  u = mint_nodes_get_update(n);
  activity = mint_update_get_param( u, 0 );
  for( i=min; i<max; i++ )
    n[1][i] = activity;
}

void add_noise( struct mint_network *net ) {
  mint_nodes n;
  int i, j, k;
  for( i=0; i<mint_network_groups(net); i++ ) {
    n = mint_network_nodes(net,i);
    for( j=0; j<mint_nodes_size(n); j++ ) {
      for( k=0; k<2+mint_nodes_states(n); k++ )
	n[k][j] += mint_random_normal(0,0.025);
    }
  }
}

int main( void ) {
  struct mint_network *net;
  FILE *f;
  float light_pos, my_pos;
  int t;
  mint_nodes sensor, energy, posphoto, negphoto, output; 

  mint_random_seed( time(0) );

  mint_update_nadd( "metabolism", metabolism, 1, 1, &loss );
  mint_update_nadd( "tonic", tonic, 0, 1, &activity );

  f = fopen("phototaxis.arc","r");
  net = mint_network_load( f );
  fclose( f );

  sensor = mint_network_nodes(net,0);
  posphoto = mint_network_nodes(net,1);
  negphoto = mint_network_nodes(net,2);
  energy = mint_network_nodes(net,3);
  output = mint_network_nodes(net,4);

  f = fopen("output/phototaxis.dot","w");
  mint_network_graph_full( net, f );
  fclose( f );

  light_pos = 1;
  my_pos = mint_random();
  mint_nodes_set( energy, 2, .2 + .8*mint_random() );

  f = fopen("output/phototaxis.dat","w");
  for( t=0; t<tmax; t++ ) {
    mint_nodes_set( sensor, 1, 1.1 - fabs(my_pos-light_pos) );
    /* add_noise( net ); */
    mint_network_nupdate( net );
    my_pos += step*( output[1][0] - 0.5 );
    if( my_pos<0 ) my_pos = 0;
    if( my_pos>1 ) my_pos = 1;
    fprintf( f, "%d %f %f %f %f\n", t, energy[1][0], my_pos,
	     posphoto[1][0], negphoto[1][0] );
  }
  fclose( f );

  mint_network_del( net );
  return 0;
}
