#ifndef MINT_NETOP_H
#define MINT_NETOP_H

#include "network.h"

/** Set feedforward network dynamics: a single call to
   mint_network_nupdate fully propagates the input to the output
   group(s). Aborts if the network is actually recurrent rather than
   feedforward. 

   Parameters: none. */
void mint_network_init_feedforward( struct mint_network *, float *p );

/** Set synchronous network dynamics: all matrix operations are
    performed (default: matrix-vector multiplications), then all node
    groups are updated.

    Parameters: none. */
void mint_network_init_synchronous( struct mint_network *, float * );

/** Set asynchronous network dynamics: a number of randomly picked
   nodes are updated at each call of mint_network_nupdate.

   Parameters: 0: The number of node updates to make per network
                  update (defaults to the number of nodes).  */
void mint_network_asynchronous( struct mint_network *, float *param );

/* This is an undocumented library function that should not be called
    by users. */
void mint_network_init_spread( struct mint_network *, float *p );

void mint_network_spread( struct mint_network *, float *p );

void mint_network_asynchronous( struct mint_network *, float *p );

/** This network update op clocks the number of updated to at most n
   per second, where n is the value of the first op parameter. If the
   second parameter is 1 (default), an error message is issued on
   stderr whenever the desired clock rate cannot be maintained. If the
   second parameter is not 1, no message is issued. */
void mint_network_clocked( struct mint_network *, float *p );

#endif
