#include "nop.h"
#include "random.h"
#include "string.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "fastapprox.h"

struct mint_nodes_str;

#undef SET_VAR
#define SET_VAR( n, x, i ) \
  mint_check( i>=0 && i<2+mint_nodes_states(n), \
	      "parameter value %d out of bounds 0-%d", \
	      (int)i, 2+mint_nodes_states(n) );	       \
  x = n[ (int)i ]

/* logistic function */
void mint_node_logistic( mint_nodes n, int min, int max, float *p ) {
  int i;
  float slope, zero, x, *in, *out;

  mint_check( p[0]>0 && p[0]<1, "parameter 0 must be in ]0,1[, but is %f", 
	      p[0] );
  mint_check( p[1]>0, "parameter 1 must be >0, but is %f", p[1] );

  slope = p[1];
  zero = log( 1/p[0] - 1 ) / slope;
  SET_VAR( n, in, p[2] );
  SET_VAR( n, out, p[3] );

  for( i=min; i<max; i++ ) {
    x = slope*(zero - in[i]);
    if( x < -10 )
      out[i] = 1;
    else if( x > 10 )
      out[i] = 0;
    else
      out[i] = 1.0f / (1.0f + fasterexp( x ) );
  }
}

/* leaky integrator */
void mint_node_integrator( mint_nodes n, int min, int max, float *p ) {
  int i;
  float leak, tstep, *in, *out;

  tstep = p[0];
  leak = p[1];
  SET_VAR( n, in, p[2] );
  SET_VAR( n, out, p[3] );

  for( i=min; i<max; i++ )
    out[i] += ( in[i] - leak * out[i] ) / tstep;
}

void mint_node_izzy( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *in, *out, *u, *v;

  SET_VAR( n, in, p[4] );
  SET_VAR( n, out, p[5] );
  SET_VAR( n, u, p[6] );
  SET_VAR( n, v, p[7] );

  for( i=min; i<max; i++ ) {
    if( v[i] == 30 ) {
      out[i] = 1;
      v[i] = p[2];  /* reset */
      u[i] = p[3];  /* reset */
    } else {
      v[i] += 0.04 * v[i]*v[i] + 5*v[i] + 140 - u[i] + in[i];
      u[i] += p[0] * ( p[1] * v[i] - u[i] );
      if( v[i] > 30 ) v[i] = 30;
      out[i] = 0;
    }
  }
}

void mint_node_noise( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *state;

  SET_VAR( n, state, p[2] );

  for( i=min; i<max; i++ )
    state[i] += mint_random_normal( p[0], p[1] );
}

void mint_node_bounded( mint_nodes n, int min, int max, float *p ) {
  int i;
  float vmin, vmax, *state;

  vmin = p[0];
  vmax = p[1];
  SET_VAR( n, state, p[2] );

  for( i=min; i<max; i++ ) {
    if( state[i] < vmin )
      state[i] = vmin;
    else if( state[i] > vmax )
      state[i] = vmax;
  }
}

void mint_node_counter( mint_nodes n, int min, int max, float *p ) {
  int i;
  float threshold, *state, *counter;

  threshold = p[0];
  SET_VAR( n, state, p[1] );
  SET_VAR( n, counter, p[2] );

  mint_check( p[1] != p[2], "parameters 1 and 2 must differ" );

  for( i=min; i<max; i++ ) {
    if( state[i] >= threshold )
      counter[i] = 0;
    else
      counter[i] += 1;
  }
}

void mint_node_spikes( mint_nodes n, int min, int max, float *p ) {
  int i;
  float *state, probability;

  probability = p[0];
  SET_VAR( n, state, p[1] );

  mint_check( probability >= 0, "parameter 0 must be positive" );

  for( i=min; i<max; i++ )
    state[i] = mint_random() < 0.001 * probability;
}

void mint_node_rows( mint_nodes n, int min, int max, float *p ) {
  int size = mint_nodes_size( n );
  MINT_UNUSED( min );
  MINT_UNUSED( max );
  mint_check( p[0] != -1, "rows argument missing (parameter 0)" );
  mint_check( p[0]>0 && p[0]<size, "rows argument <0 or too large" );
}

void mint_node_states( mint_nodes n, int min, int max, float *p ) {
  MINT_UNUSED( n );
  MINT_UNUSED( min );
  MINT_UNUSED( max );
  mint_check( p[0]>0, "states argument %f is negative", p[0] );
}

void mint_node_size( mint_nodes n, int min, int max, float *p ) {
  MINT_UNUSED( min );
  MINT_UNUSED( max );
  mint_check( p[0]>0, "non positive size for nodes %s", 
	      mint_nodes_get_name(n) );
}

void mint_node_color( mint_nodes n, float *p ) {
  int states;
  states = mint_nodes_states( n );
  mint_check( p[0]>=0 && p[0]<2+states,
	      "parameter 0 out of range 0-%d", 1+states );
}

/*
    Parameters: 0: Time constant
                1: Maximum habituation level, in [0,1]
		2: State variable storing habituation level (default: 2)
		3: State variable that habituates (default: 1)
 */
void mint_node_habituation( mint_nodes n, int min, int max, float *p ) {

  float T, a;
  float *h, *y;
  int i;

  T = p[0];
  mint_check( T>0, "time constant (parameter 0) must be positive" );
  a = p[1];
  mint_check( a>=0 && a<=1, 
	      "max habituation (parameter 1) must be in [0,1]" );
  
  SET_VAR( n, h, p[2] );
  SET_VAR( n, y, p[3] );

  for( i=min; i<max; i++ ) {
    h[i] += ( a*y[i] - h[i] ) / T;
    y[i] -= h[i];
  }
}

void mint_node_identity( mint_nodes n, int min, int max, float *p ) {
  MINT_UNUSED( min );
  MINT_UNUSED( max );
  MINT_UNUSED( p );
  memcpy( n[1], n[0], mint_nodes_size(n) * sizeof(float) );
}


#undef SET_VAR
