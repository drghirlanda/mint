#include <mint.h>

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

void light_sensor( mint_nodes n, int min, int max ) {
  int i;
  for( i=min; i<max; i++ )
    n[1][i] = 1.1 - n[0][i];
}
