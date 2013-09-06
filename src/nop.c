#include "nop.h"
#include "random.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct mint_nodes_str;

void mint_node_identity( mint_nodes n, int min, int max, float *p ) {
  memcpy( n[1]+min, n[0]+min, (max-min)*sizeof(float) );
}

void mint_node_sigmoid( mint_nodes n, int min, int max, float *p ) {
  int i;
  float x, zero, slope;
  float x0 = 6. + 2./3.;
  float *in, *out;
  in = n[0];
  out = n[1];
  zero = p[0];
  slope = p[1];
  for( i=min; i<max; i++ ) {
    x = *(in+i);
    x *= slope;                       /* scaling */
    x -= (.5 - zero) / (.075 + zero); /* translation */
    if( x < -x0 ) 
      *(out+i) = 0.;
    else if( x < x0 ) 
      *(out+i) = .5 + .575 * x / ( 1 + fabs(x) );
    else 
      *(out+i) = 1.;
  }
}

/* leaky integrator */
void mint_node_integrator( mint_nodes n, int min, int max, float *p ) {
  int i;
  float leak, tstep, *in, *out, *state;

  mint_check( p[2]>=2 && p[2]<2+mint_nodes_states(n), 
	      "parameter 2 out of range" );

  in = n[0];
  out = n[1];
  state = n[ (int)p[2] ];
  tstep = p[0];
  leak = p[1];

  for( i=min; i<max; i++ ) {
    *(state+i) += ( *(in+i) - leak * *(state+i) ) / tstep;
    *(out+i) = *(state+i);
  }
}

void mint_node_izzy( mint_nodes n, int min, int max, float *p ) {
  int i;
  float u, v, *in, *out, *vs, *us;

  mint_check( p[4]>=2 && p[4]<2+mint_nodes_states(n), 
	      "parameter 4 out of range" );
  mint_check( p[5]>=2 && p[5]<2+mint_nodes_states(n), 
	      "parameter 5 out of range" );
  mint_check( p[4] != p[5], "parameters 4 and 5 must be different" );

  in = n[0];
  out = n[1];
  vs = n[ (int)p[4] ];
  us = n[ (int)p[5] ];

  for( i=min; i<max; i++ ) {
    v = *(vs+i);
    u = *(us+i);
    if( v==30 ) {
      *(out+i) = 1;
      *(vs+i) = *(p+2);
      *(us+i) += *(p+3);
    } else {
      v += 0.04 * v*v + 5*v + 140 - u + *(in+i);
      u += *p * ( *(p+1) * v - u );
      if( v>30 ) v = 30;
      *(out+i) = 0;
      *(vs+i) = v;
      *(us+i) = u;
    }
  }
}

void mint_node_noise( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *state;
  
  mint_check( p[0]>=0 && p[0]<2+mint_nodes_states(n), 
	      "parameter 0 out of range" );

  state = n[ (int)p[0] ];
  for( i=min; i<max; i++ )
    *(state+i) += mint_random_normal( p[1], p[2] );
}

void mint_node_bounded( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *state;

  mint_check( p[0]>=0 && p[0]<2+mint_nodes_states(n), 
	      "parameter 0 out of range" );

  state = n[ (int)p[0] ];
  for( i=min; i<max; i++ ) {
    if( *(state+i) < p[1] )
      *(state+i) = p[1];
    else if( *(state+i) > p[2] )
      *(state+i) = p[2];
  }
}

void mint_node_counter( mint_nodes n, int min, int max, float *p ) {
  int i;
  float threshold, *state, *counter;

  mint_check( p[0]>=0 && p[0]<2+mint_nodes_states(n), 
	      "parameter 1 out of range" );
  mint_check( p[2]>=0 && p[2]<2+mint_nodes_states(n), 
	      "parameter 1 out of range" );
  mint_check( p[0] != p[2], "parameters 0 and 2 must differ" );

  threshold = p[1];
  state = n[ (int)p[0] ];
  counter = n[ (int)p[2] ];

  for( i=min; i<max; i++ ) {
    if( *(state+i) >= threshold )
      *(counter+i) = 0;
    else
      *(counter+i) += 1;
  }
}

void mint_node_spikes( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *out;

  mint_check( p[0]>=0, "parameter 0 must be positive" );

  out = n[1];
  for( i=min; i<max; i++ )
    *(out+i) = mint_random() < .001 * p[0];
}

void mint_node_rows( mint_nodes n, int min, int max, float *p ) {
  int size = mint_nodes_size( n );
  mint_check( p[0] != -1, "rows argument missing (parameter 0)" );
  mint_check( p[0]>0 && p[0]<size, "rows argument negative or too large" );
}

void mint_node_states( mint_nodes n, int min, int max, float *p ) {
  mint_check( p[0]>0, "states argument negative" );
}

void mint_node_size( mint_nodes n, int min, int max, float *p ) {
  mint_check( p[0]>0, "size argument negative" );
}

void mint_node_color( mint_nodes n, float *p ) {
  int states;
  states = mint_nodes_states( n );
  mint_check( p[0]>=0 && p[0]<2+states,
	      "parameter 0 out of range 0-%d", 1+states );
}
