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
  in = n[0];
  out = n[1];
  state = n[2];
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
  in = n[0];
  out = n[1];
  vs = n[2];
  us = n[3];
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

void mint_node_tonic( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *out;
  out = n[1];
  for( i=min; i<max; i++ ) {
    *(out+i) = mint_random_normal(p[0], p[1]);
    if( *(out+i) < 0  )
      *(out+i) = 0;
  }
}

void mint_node_noise( mint_nodes n, int min, int max, float *p ) {
  int i;
  int s = p[0];
  mint_check( s>=0 && s<2+mint_nodes_states(n), "p[0] out of range" );
  for( i=min; i<max; i++ )
    n[s][i] += mint_random_normal(p[1], p[2]);
}

void mint_node_bounded( mint_nodes n, int min, int max, float *p ) {
  int i;
  int s = p[0];
  mint_check( s>=0 && s<2+mint_nodes_states(n), "p[0] out of range" );
  for( i=min; i<max; i++ ) {
    if( n[s][i] < p[1] )
      n[s][i] = p[1];
    else if( n[s][i] > p[2] )
      n[s][i] = p[2];
  }
}

void mint_node_counter( mint_nodes n, int min, int max, float *p ) {
  int i;
  int threshold = p[0];
  int s = p[1];
  mint_check( s>=0 && s<2+mint_nodes_states(n), "p[1] out of range" );
  for( i=min; i<max; i++ ) {
    if( n[1][i] >= threshold )
      n[s][i] = 0;
    else
      n[s][i] += 1;
  }
}

void mint_node_spikes( mint_nodes n, int min, int max, float *p ) {
  int i;
  mint_check( p[0]>=0, "p[0] must be positive" );
  for( i=min; i<max; i++ )
    n[1][i] = mint_random() < .001 * p[0];
}
