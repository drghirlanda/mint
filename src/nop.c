#include "op.h"
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

/* fast logistic function */
void mint_node_fastlogistic( mint_nodes n, int min, int max, float *p ) {
  int i;
  float slope, zero, x, *in, *out;

  mint_check( p[0]>0 && p[0]<1, 
	      "parameter 0 must be in ]0,1[, but is %f", p[0] );
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

void mint_node_logistic( mint_nodes n, int min, int max, float *p ) {
  int i;
  float slope, zero, x, *in, *out;

  mint_check( p[0]>0 && p[0]<1, 
	      "parameter 0 must be in ]0,1[, but is %f", p[0] );
  mint_check( p[1]>0, "parameter 1 must be >0, but is %f", p[1] );

  slope = p[1];
  zero = log( 1/p[0] - 1 ) / slope;
  SET_VAR( n, in, p[2] );
  SET_VAR( n, out, p[3] );

  for( i=min; i<max; i++ ) {
    x = slope*(zero - in[i]);
    out[i] = 1.0 / ( 1.0 + exp( x ) );
  }
}

void mint_node_sigmoid( mint_nodes n, int min, int max, float *p ) {
  int i;
  float x, zero, slope, *in, *out;
  float x0 = 6. + 2./3.;
  
  zero = p[0];
  slope = p[1];
  SET_VAR( n, in, p[2] );
  SET_VAR( n, out, p[3] );
  
  for( i=min; i<max; i++ ) {
    x = in[i];
    x *= slope;                       /* scaling */
    x -= (.5 - zero) / (.075 + zero); /* translation */
    if( x < -x0 ) 
      out[i] = 0.;
    else if( x < x0 ) 
      out[i] = .5 + .575 * x / ( 1 + fabs(x) );
    else 
      out[i] = 1.;
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
  float vmin, vmax, *from, *to;

  vmin = p[0];
  vmax = p[1];
  SET_VAR( n, from, p[2] );
  SET_VAR( n, to,   p[3] );

  for( i=min; i<max; i++ ) {
    if( from[i] < vmin )
      to[i] = vmin;
    else if( from[i] > vmax )
      to[i] = vmax;
    else
      to[i] = from[i];
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
  mint_check( p[3]>=0 && p[3]<2+states,
	      "parameter 1 out of range 0-%d", 1+states );
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
  
  SET_VAR( n, h, p[2] ); /* habituation level is n[ p[2] ] */
  SET_VAR( n, y, p[3] ); /* habituating variable is n[ p[3] ] */

  for( i=min; i<max; i++ ) {
    h[i] += ( a*y[i] - h[i] ) / T;
    y[i] -= h[i];
  }
}

void mint_node_identity( mint_nodes n, int min, int max, float *p ) {
  MINT_UNUSED( min );
  MINT_UNUSED( max );
  MINT_UNUSED( p );
  memcpy( n[ (int)p[1] ], n[ (int)p[0] ], 
	  mint_nodes_size(n) * sizeof(float) );
}


#include <float.h>

/* the algorithm for numerical differentiation is derived from
   en.wikipedia.org/wiki/Numerical_differentiation, second two-point
   method. */

void mint_node_gradient( mint_nodes n, int min, int max, float *p  ) {
  int i, j, nops, s, running;
  float *old, *x, *y, *g;
  float y1, y2, step;
  struct mint_op *op;
  struct mint_ops *ops;

  SET_VAR( n, y, p[0] );
  SET_VAR( n, x, p[1] );
  SET_VAR( n, g, p[2] );

  step = p[3];
  running = 4;

  /* 'running' is an undocumented parameter to used to check whether a
     gradient op is running. if it is set it means that we are being
     called as part of a node update used in a gradient calculation,
     so we should not be doing such a calculation ourselves.  */
  if( p[running] ) 
    return;

  /* if we need to run, disable all further gradient calculations but
     setting p[3] to 1 in all gradient ops */
  ops = mint_nodes_get_ops( n );
  nops = mint_ops_size( ops );
  for( i=0; i<nops; i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_name_is( op, "gradient" ) )
      mint_op_set_param( op, running, 1 );
  }

  /* 'old' stores the current node state. it has to be reinstated
     during the calculation and at the end. */
  s = 2 + mint_nodes_states( n );
  old = malloc( s * sizeof(float) );

  for( i=min; i<max; i++ ) {

    for( j=0; j<s; j++ ) old[j] = n[j][i]; /* record node state */

    x[i] = old[ (int)p[1] ] - step; /* calculate (x-h,y) */
    mint_nodes_update( n, i, i+1 );
    y1 = y[i];

    for( j=0; j<s; j++ ) n[j][i] = old[j]; /* restore node state */

    x[i] = old[ (int)p[1] ] + step; /* calculate (x+h,y) */
    mint_nodes_update( n, i, i+1 );
    y2 = y[i];
  
    for( j=0; j<s; j++ ) n[j][i] = old[j]; /* restore node state */

    g[i] = ( y2 - y1 ) / (2*step); /* store gradient */
  }

  free( old );

  /* once we are done we reset p[3] for all gradient ops */
  for( i=0; i<nops; i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_name_is( op, "gradient" ) )
      mint_op_set_param( op, running, 0 );
  }
}

void mint_node_softmax( mint_nodes n, int min, int max, float *p ) {
  int i, *out_int;
  float slope, *in, *out, *prob, min_in;

  mint_check( p[0]>0 && p[0]<mint_nodes_states(n), 
	      "parameter 0 out of range; must be a node state index, but is %f", p[0] );

  mint_check( p[1]>0 && p[1]<mint_nodes_states(n), 
	      "parameter 1 out of range; must be a node state index, but is %f", p[1] );

  mint_check( p[2]>0 && p[2]<mint_nodes_states(n), 
	      "parameter 2 out of range; must be a node state index, but is %f", p[2] );

  mint_check( p[0] != p[1],
	      "parameters 0 and 1 cannot be the same" );

  mint_check( p[3]>0, "parameter 3 must be >0, but is %f", p[3] );

  slope = p[3];

  in   = n[ (int)p[0] ];
  out  = n[ (int)p[1] ];
  prob = n[ (int)p[2] ];
  
  /* calculating the minimum beforehand helps avoiding overflow in the
     exponential */
  min_in = 0;
  if( p[4] == 1 ) {
    for( i=min; i<max; i++ ) {
      if( in[i] < min_in )
	min_in = in[i];
    }
  }

  /* calculate relative probabilities */
  for( i=min; i<max; i++ ) {
    prob[i] = exp( slope * ( in[i] - min_in ) );
  }

  /* using the multinomial distribution with 1 attempt only will set
     to 1 only one of the components of out, and set the others to
     0 */
  out_int = malloc( (max-min) * sizeof(int) );
  mint_random_multinomial( prob, out_int, 1, max-min );
  for( i=min; i<max; i++ ) {
    out[i] = out_int[i];
  }
  free( out_int );
  
}

#undef SET_VAR
