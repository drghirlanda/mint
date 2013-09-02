#ifndef MINT_SPREAD_H
#define MINT_SPREAD_H

#include <stdio.h>

/** \file spread.h

    \brief Deciding how to update a network (synchronous,
    asynchronous, feed-forward, custom sequence of matrix-vector
    multiplications, etc.) */
 
/** Spread objects represent the sequence of operations performed when
   updating a network ("spread" of activation through the network). At
   each step of a spread sequence, either a weight-vector
   multiplication is performed or a node update is performed, or both,
   as detailed in mint_spread_load below. 

   NOTE: There is no _cpy function, see NOTE in update.h */
struct mint_spread;

struct mint_network;

/** Create a dummy spread object that does not contain any spreading
   scheme. */
struct mint_spread *mint_spread_new( int len );

/** Destroy a spread object */
void mint_spread_del( struct mint_spread * );

/** Duplicate a spread object */
struct mint_spread *mint_spread_dup( const struct mint_spread * );

/** Load a spread object from file. The format is, for all kinds of
   spreading but the asynchronous one:

   spread <len>
   w1 ... w<len>
   n1 ... n<len>

   where <len> is an integer giving the length of the spread sequence.
   w1 ... w<len> is a sequence of integers stating what weight matrix
   is involved in each step of the spreading sequence. if wi is
   negative, no matrix-vector multiplication is performed; if wi is
   zero or positive, the matrix-vector multiplication relative to
   weight matrix wi is performed (which node groups are involved is
   stored in the weight matrix itself, see weights.h). The node group
   sequence specifies similarly which node group is involved: if ni is
   negative, no node update operation is performed; if ni is zero or
   positive, the node group with index ni is updated.

   Example: spreading for a two-layer feed-forward network is given
   by:
 
   spread 2
   -1 0
   0 1

   Meaning: update node group 0 (input nodes), perform the
   matrix-vector multiplication involving the weight matrix with index
   0 (the only one), update node group 1 (output nodes). Note that the
   sequence can be compressed in just two steps because matrix-vector
   multiplications are performed before node updates. An equivalent
   sequence, one step longer, is:

   spread 3
   -1 0 -1
   0 -1 1

   In general, if ni is -1 and wi+1 is also -1, the sequence can be
   shortened by one step replacing the pairs (ni,wi) (ni+1,wi+1) with
   (wi,ni+1).
*/
struct mint_spread *mint_spread_load( FILE *, struct mint_network *net );

/** Save a spread object to file, according to the above format */
void mint_spread_save( struct mint_spread *, FILE *,
		       struct mint_network *net );

/** Retrieve length of spread sequence */
int mint_spread_len( struct mint_spread * );

/** Retrieve the node group index for step i of the sequence */
int mint_spread_get_nodes( struct mint_spread *, int i );

/** Set the node group index for step i of the sequence to group k */
void mint_spread_set_nodes( struct mint_spread *, int i, int k );

/** Retrieve the weight matrix index for step i of the sequence */
int mint_spread_get_weights( struct mint_spread *, int );

/** Set the weight matrix index for step i of the sequence to matrix k */
void mint_spread_set_weights( struct mint_spread *, int i, int k );

/* Add a step in position pos of the spread sequence, involving
   weights wi and nodes ni. */
void mint_spread_add_step( struct mint_network *net, int pos, int wi, int ni ) ;

#endif
