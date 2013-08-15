#include <mint.h>
#include <stdio.h>
#include <math.h>

float step = .1;
float low_energy = 0.2;
float high_energy = 0.8;

float noise = 0;

void add_noise( struct mint_network *net, float stdev ) {
  mint_nodes n;
  int i, j, k;
  for( i=0; i<mint_network_groups(net); i++ ) {
    n = mint_network_nodes(net,i);
    for( j=0; j<mint_nodes_size(n); j++ ) {
      for( k=0; k<2+mint_nodes_states(n); k++ )
	n[k][j] += mint_random_normal(0,stdev);
    }
  }
}

float simulation( struct mint_network *net, int tmax, FILE *f ) {
  float light_pos, my_pos, score;
  long t;
  mint_nodes sensor, energy, posphoto, negphoto, output; 

  sensor = mint_network_nodes(net,0);
  posphoto = mint_network_nodes(net,1);
  negphoto = mint_network_nodes(net,2);
  energy = mint_network_nodes(net,3);
  output = mint_network_nodes(net,4);

  light_pos = 1;
  my_pos = 0; /* mint_random(); */
  mint_nodes_set( energy, 1, 0.5 );
  score = 0;

  for( t=0; t<tmax; t++ ) {
    mint_nodes_set( sensor, 0, fabs(my_pos-light_pos) );
    add_noise( net, noise );
    mint_network_nupdate( net );

    my_pos += step*( output[1][0] - 0.5 );
    if( my_pos<0 ) my_pos = 0;
    if( my_pos>1 ) my_pos = 1;

    if( energy[1][0] <= low_energy )
      score -= 10;
    else if( energy[1][0] >= low_energy && my_pos>0.1 )
      score--; 

    if( f ) {
      fprintf( f, "%d %f %f %f %f\n", t, energy[1][0], my_pos,
	       posphoto[1][0], negphoto[1][0] );
    }
  }

  return score;
}
