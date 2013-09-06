#ifndef MINT_OP_H
#define MINT_OP_H

#include "nodes.h"
#include "weights.h"
#include "network.h"

/** These are the types of ops that exist in mint. They specify what
    an op is for: node groups can be initialized and updated; weight
    matrices can be initialized, updated, operated (performing of
    matrix-vector multiplication), and connected to node groups;
    networks can be operated (calculating node activities and weight
    changes) and initialized (setting up how the network is
    operated). */
enum {
  mint_op_nodes_update = 1,
  mint_op_nodes_init = 2,
  mint_op_nodes_any = 1+2,
  mint_op_weights_update = 4,
  mint_op_weights_init = 8,
  mint_op_weights_operate = 16,
  mint_op_weights_connect = 32,
  mint_op_weights_any = 4+8+16+32,
  mint_op_network_init = 64,
  mint_op_network_operate = 128,
  mint_op_network_any = 64+128  
};

/** An op describes an operation that can be performed on nodes or
    weights, such as initialization or update. */
struct mint_op;

/** Data structure to manage lists of ops. */
struct mint_ops;
 
/** Function prototpye for node update and init ops. */
typedef void (*mint_nop_t)( mint_nodes, int, int, float * );

/** Function prototpye for weight update, operate, and connect ops. */
typedef void (*mint_wop_t)( mint_weights, mint_nodes, mint_nodes, 
			      int, int, float * );

/** Function prototpye for weight init ops. */
typedef void (*mint_winit_t)( mint_weights, int, int, float * );

/** Function prototype for network ops. */
typedef void (*mint_netop_t)( struct mint_network *, float * );

/** Create a op by name and type. Aborts if the op cannot be found. To
    check whether a op name exists use mint_op_exists. */
struct mint_op *mint_op_new( const char *name, int type );

/** Delete op object. */
void mint_op_del( struct mint_op *h );

/** Duplicate a op. */
struct mint_op *mint_op_dup( const struct mint_op *h1 );

/** Save op to file. */
void mint_op_save( const struct mint_op *h, FILE *dest );

/** Load a op from file. */
struct mint_op *mint_op_load( FILE *file, int type );

/** Returns 1 if an op by this name and type exists in the
    library. Returns 0 otherwise.*/
int mint_op_exists( const char *name, int type );

/** Return op name. */
const char *mint_op_name( const struct mint_op *h );

/** Return op type. */
int mint_op_type( const struct mint_op *h );

/** Return number of parameters for op. */
int mint_op_nparam( const struct mint_op *h );

/** Return value of parameter i. */
float mint_op_get_param( struct mint_op *h, int i );

/** Return pointer to parameter array . */
float *mint_op_get_params( struct mint_op *h );

/** Set value of op parameter i. */
void mint_op_set_param( struct mint_op *h, int i, float x );

/** Add to the library an op of the given type.  The function f should
    be of the appropriate type, but should be cast to void *. The op
    will have nparam parameters, with default values param[0] through
    param[nparam]. */ 
void mint_op_add( const char *name, int type, void *f,  int nparam, 
		  float *param );

/** Run an op on the appropriate objects. You are responsible for
    passing the correct parameters, in the same order as taken by the
    op prototype. For example, if you are using a weight
    initialization op (type mint_winit_t) you would do: mint_op_run(
    op, w, rmin, rmax ), where w is a weight matrix and rmin and rmax
    the range of rows to initialize according to the op. */
void mint_op_run( struct mint_op *op, ... );

/** Create a list of ops, initially empty. */
struct mint_ops *mint_ops_new( void );

/** Add op to a list of ops. */
void mint_ops_append( struct mint_ops *ops, struct mint_op *op );

/** Delete all ops. */
void mint_ops_del( struct mint_ops *ops );

/** Duplicate ops. */
struct mint_ops *mint_ops_dup( const struct mint_ops *ops );

/** Load as many ops as possible from file of the given type(s). To
    load multiple types, use type1 + type2 + ...  */ 
struct mint_ops *mint_ops_load( FILE *file, int type );

/** Save ops to file. */
void mint_ops_save( const struct mint_ops *ops, FILE *dest );

/** How many ops. */
int mint_ops_size( const struct mint_ops *ops );

/** Get op i. */
struct mint_op *mint_ops_get( struct mint_ops *ops, int i );

/** Set op i, deleting the existing op. */
void mint_ops_set( struct mint_ops *ops, int i, struct mint_op *op );

/** Delete all ops of a given type from an ops array. Returns the
    number of ops deleted. The ops removed from the list are deleted
    with mint_op_del. */
int mint_ops_del_type( struct mint_ops *ops, int type );

/** Delete all ops with a given name from an ops array. Returns the
    number of ops deleted. The ops removed from the list are deleted
    with mint_op_del. */
int mint_ops_del_name( struct mint_ops *ops, const char *name );


/** Retrieve the (first) op with a given name and type, or 0 if no
    such op. */
struct mint_op *mint_ops_get_name( struct mint_ops *ops, 
				   const char *name, int type );

/** Count how many ops of a given type are in an ops object. */
int mint_ops_count( struct mint_ops *ops, int type );

/** Return index of an op with given name and type, or -1 if not
    found. */
int mint_ops_find( struct mint_ops *ops, const char *name, int type );

#endif
