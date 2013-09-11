#ifndef MINT_NETWORK_H
#define MINT_NETWORK_H

#include "nodes.h"
#include "weights.h"
#include <stdio.h>

/** \file network.h 

   \brief Creating, manipulating, and running networks 

*/

/** Networks are managed through structures of type mint_network. 

    NOTE: There is no mint_network_new function, because networks are
    created either reading from file (either a short description
    resulting in a network with null values for weights and nodes, or
    a previously saved network) or duplicating other networks via
    mint_network_dup() */
struct mint_network;

/** Destroy a network, freeing up used memory */
void mint_network_del( struct mint_network *net );

/** Duplicate a network */
struct mint_network *mint_network_dup( const struct mint_network * );

/** Make net1 equal to net2 */
void mint_network_cpy( struct mint_network *net1, 
		       const struct mint_network *net2 );

/** Save a network to an open file */
void mint_network_save( const struct mint_network *net, FILE *dest );

/** Save a network to an open file, apart from weight and node
    values. */
void mint_network_info( const struct mint_network *net, FILE *dest );

/** Load a network from an open file. The file format is:

   network g m \n
   <node operations>
   <node groups> \n
   <weight matrices> \n
   <spread scheme> \n

   where: g and m are the number of node groups and weight matrices;
   <node operations> are optional node operations; <node groups>
   consists of g node group specifications; <weight matrices> consists
   of m weight matrix specifications. Both may be either in short
   form, giving only architectural details such as number of nodes and
   state variables, or in long form giving values for all
   nodes/weights. See nodes.h and weights.h for details. Lastly,
   <spread scheme> is an optional spreading scheme (see spread.h). If
   no spreading scheme is on file, if none is set by the network
   operations, and if there is no 'operate' operation specified,
   synchronous spreading is set. 

   NOTE: It is possible to have 'operate' ops AND a spread. It is up
   to you to decide whether this makes sense. For example, MINT
   provides an 'operate' op to acquire images from camera, which does
   not do any spreading of activation and thus should be used in
   conjunction with other ops. */
struct mint_network *mint_network_load( FILE * );

/** Number of node groups in this network */
int mint_network_groups( const struct mint_network *n );

/** Number of weight matrices in this network */
int mint_network_matrices( const struct mint_network *n );

/** Total number of nodes in the network. */
int mint_network_size( struct mint_network *net );

/** Access to node group i of the network */
mint_nodes mint_network_nodes( struct mint_network *, int i );

/** Return index of nodes with given name, or -1 if no such name. */
int mint_network_nodes_find( struct mint_network *, char *name );

/** Return index of the first node group that has an op with given
    name, or -1 if no such group. */
int mint_network_nodes_find_op( struct mint_network *, char *name );

/** Return index of weight matrix connecting nodes with given names,
    or -1 if no such matrix. */
int mint_network_weights_find( struct mint_network *, char *name );

/** Access to weight matrix i of the network */
mint_weights mint_network_weights( struct mint_network *, int i );

/** Update network nodes, performing matrix-vector multiplications and
   invoking node update functions, according to the current update
   scheme (see spread.h). If the network is not frozen, weight updates
   are also performed. */
void mint_network_operate( struct mint_network * );

/** Freeze (f true) or unfreeze (f false) weight matrix number i (see
    mint_weights_freeze and mint_weights_unfreeze). If i is negative,
    all weight matrices a refrozen/unfrozen. */
void mint_network_freeze( struct mint_network *net, int i, int f );

/** Returns the number of frozen weight matrices. */
int mint_network_frozen( struct mint_network *net );

/** Retrieve the spread object associated with this network */
struct mint_spread *mint_network_get_spread( struct mint_network *net );

/** Set the spread object (it is advisable to use the functions in
   spread.h, though) */
void mint_network_set_spread( struct mint_network *net,
			      struct mint_spread *s );

/** Retrieve the ops list for with this network */
struct mint_ops *mint_network_get_ops( struct mint_network *net );

/** Add a network to an existing one. The n1 node group of the first
    network is set to send its output to the n2 group of the second
    network. The weight matrix supplied as input is used for the
    connection (and must have the appropriate dimensions). If the
    weight matrix is null, a weight matrix is created with all entries
    equal, and normalized so that the sum of all weights is 1. Each
    weight will have s state variables (this argument is ignored if w
    is not null). The spreading schemes of the joined network consists
    of the spreading of net1, then the newly added weight matrix and
    node update, then the spreading of net2. */
void mint_network_add( struct mint_network *net1, int n1,
		       const struct mint_network *net2,
		       int n2, const mint_weights w, int s );

/** Write to file a rendition of the network architecture in graphviz format */
void mint_network_graph( const struct mint_network *net, FILE *f );

/** Write a graphviz rendition of the whole network, including
   individual nodes and weights (INCOMPLETE) */
void mint_network_graph_full( const struct mint_network *net, FILE *f );

/** Add a copy of node group n to net */
void mint_network_add_nodes( struct mint_network *net, const mint_nodes n );

/** Add a copy of weight matrix w to net. Note: you must set the from
   and to arguments for this matrix to make sense in this network. You
   must also modify the spread scheme appropriately. */
void mint_network_add_weights( struct mint_network *net, const mint_weights w );

/** Add a step at position pos of the spreading scheme */
void mint_network_add_spread_step( struct mint_network *net, 
				   int pos, int wi, int ni );

/** Delete weight matrix i from the network and put a copy of weight
    matrix w in its place. */
void mint_network_replace_weights( struct mint_network *net, 
				   int i, mint_weights w );

/** Run init ops defined for this network. */
void mint_network_init( struct mint_network * );

#endif
