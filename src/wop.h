#ifndef MINT_WUPDATE_LIB_H
#define MINT_WUPDATE_LIB_H

#include "weights.h"
#include "nodes.h"


void mint_weights_init_rows( mint_weights w, int rmin, int rmax, 
			     float *p ); 

void mint_weights_init_cols( mint_weights w, int rmin, int rmax, 
			     float *p ); 

void mint_weights_init_states( mint_weights w, int rmin, int rmax,
			       float *p ); 

void mint_weights_init_sparse( mint_weights w, int rmin, int rmax,
			       float *p ); 

/** Perform a matrix-vector multiplication between the values of w and
    the outputs of nfrom, and add the result to the input of nto. This
    is the typical operation performed when operating a network; only
    nto nodes between rmin and rmax-1 (included) are updated (this
    feature is used in multithreading and in asychnronous updating,
    where only some rows must be used at a given time, see network.h
    and threads.h). */
void mint_weights_mult( const mint_weights w, 
			const mint_nodes nfrom, 
			mint_nodes nto,
			int rmin, int rmax, float *p );

void mint_weights_mult_naive( const mint_weights w, 
			      const mint_nodes nfrom, 
			      mint_nodes nto,
			      int rmin, int rmax, float *p );

void mint_weights_hebbian( mint_weights w, mint_nodes pre, 
			   mint_nodes post, int rmin, int rmax, 
			   float *p );

/** A deltarule-like (error correcting) update rule.  The value of
    weight i,j is changed by the quantity p[0] * ( post[ p[1] ][i] -
    post[1][i] ) * pre[1][0]. Thus p[0] is the learning rate, and p[1]
    tells the rule which state variable of post-synaptic nodes stores
    the desired node output. */
void mint_weights_delta( mint_weights w, mint_nodes pre, 
			 mint_nodes post, int rmin, int rmax, float *p );

/** Spike-timing dependent plasticity. Requires the following
    parameters: p[0] = decay rate; p[1] = increment for positive
    post-pre timings; p[2] = increment for negative post-pre timings
    (should be negative). The weights are constrained between p[3]
    (min) and p[4] (max). NOTE: Spike counters must be defined for the
    pre and post synaptic nodes (see "counter" op in nop.h). What
    counts as a "spike" depends on the threshold set for these
    counters; hence this rule can work also with non-spiking
    neurons. It does not matter which node state variable is used for
    the counters, mint figures this out by itself. */
void mint_weights_stdp( mint_weights w, mint_nodes pre, 
			 mint_nodes post, int rmin, int rmax, float *p );

/** Set all weight values to uniformly distributed random values
    between min and max. Use the 'diagonal' op to set self-recurrent
    weights to zero if needed. */
void mint_weights_init_uniform( mint_weights w, int rmin, int rmax, 
			       float *p );

/** Set all weight values to x normally distributed random values with
    given mean and standard deviation. If the matrix is recurrent (to
    and from indices are the same), diagonal values (self-recurrent
    connections) are not set. */
void mint_weights_init_normal( mint_weights w, int rmin, int rmax, 
			       float *p );

/** Set diagonal (self-recurrent) weights in a recurrent matrix to p[0]. */
void mint_weights_init_diagonal( mint_weights w, int rmin, int rmax, 
				 float *p );

/** Set the target variable for a weight matrix operation. The default
    is variable 0 (node input). The index of the target variable is
    the only parameter of this op. */
void mint_weights_init_target( mint_weights w, int rmin, int rmax, 
			       float *p );

/* Normalize weights so that the toal feeding in into a postsynaptic
   node is a given number (default 1). */
void mint_weights_init_normalize( mint_weights w, int rmin, int rmax, 
				   float *p );

/* A weights connect op to perform lateral processing of different
   kinds. The weights are set up so that nodes provide input to their
   neighbors according to a configurable linear function of distance
   (see parameters below). This op is aware of node geometry: if node
   rows have been specified (see mint_node_rows), neighbours are
   calculated in two dimensions. The configurable parameters allow
   several kinds of lateral processing, such as pure lateral
   inhibition and center-surround effects (Mexican hat or reversed
   Mexican hat, see examples).

   State variables: none required.

   Parameters: 0: Connection value for nearest neighbors (positive or
                  negative).

               1: Maximum distance. No input will be sent to nodes
                  further than this.

	       2: Connection value at maximum distance (positive or
	          negative).

   NOTE: Weights values that do not pertain to the lateral connections
   are left unmodified. 

   EXAMPLES: Lateral inhibition of -1 sent to nearest neighbors only:
   "lateral -1 1 0." Lateral excitation for close neighbors, but
   inhibition for more distant ones (Mexican hat): "lateral 1 10
   -1." */
void mint_weights_lateral( mint_weights w, mint_nodes nfrom, mint_nodes nto, 
			   int rmin, int rmax, float *p );

#endif
