#ifndef MINT_WUPDATE_LIB_H
#define MINT_WUPDATE_LIB_H

#include "weights.h"
#include "nodes.h"


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

/** Matrix-vector multiplication for sparse weight matrices. It is set
    as the appropriate operation when a sparse matrix is created (see
    mint_weighs_operate). */ 
void mint_weights_mult_sparse( const mint_weights w,
			       const mint_nodes nfrom,
			       mint_nodes to,
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
void mint_weights_init_random( mint_weights w, int rmin, int rmax, 
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

/* Set the 'from' variable (index of pre-synpatic nodes). */
void mint_weights_init_from( mint_weights w, int rmin, int rmax, 
			       float *p );

/* Set the 'to' variable (index of post-synaptic nodes). */
void mint_weights_init_to( mint_weights w, int rmin, int rmax, 
			       float *p );

/* Set weights to perform lateral processing of different kinds. Every
   time the network is updated, these weights provide input to each
   node according to the current output of neighboring nodes and a
   gaussian kernel function. This op is aware of node geometry: if
   node rows have been specified (see mint_node_rows), neighbours are
   calculated in two dimensions. The effect of neighbor output on the
   focal node is a normal function of distance, with the configurable
   parameters detailed below. The parameters allow several kinds of
   lateral processing, such as pure lateral inhibition and
   center-surround effects (Mexican hat or reversed Mexican hat).

   State variables: none required.

   Parameters: 0: Value at distance 0 (positive or negative).

               1: Maximum distance. No input will be received from
               nodes further than this.

	       2: Value at maximum distance.

   NOTE: The weight matrix is constructed the first time the network
   is updated. Also, weights values that do not pertain to the lateral
   connections are left unmodified. */
void mint_weights_lateral( mint_weights w, mint_nodes nfrom, mint_nodes nto, 
			   int rmin, int rmax, float *p );

#endif
