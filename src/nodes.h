#ifndef MINT_NODES_H
#define MINT_NODES_H

#include "string.h"

#include <stdio.h>

/** \file mint.h

    \brief Master header file. Just include this and you are ready to
    go. */

/** \file nodes.h
    
    \brief Dealing with network nodes (artificial neurons, or neural
    network "units").

    Defines the mint_nodes type which is the data structure that
    represents a groups of network nodes (sometimes called a "layer"),
    as well as functions to manipulate nodes. */

/** A mint_nodes object can be treated as a 2-dim array of floats,
    with the following semantics:

    n[0][i]   --> input to node i \n
    n[1][i]   --> output of node i \n
    n[1+k][i] --> k-th state variable of node i \n

    State variables are optional, but input and output variables are
    always created for every nodes object. Info such as the number of
    nodes or states is provided by the functions below. */
typedef float **mint_nodes;

/** Creates a node group of n nodes, each with s state variables. */
mint_nodes mint_nodes_new( int n, int s );

/** Destroys a node group, freeing up the memory used. */
void mint_nodes_del( mint_nodes );

/** Creates a duplicate (exact copy) of a nodes object. */
mint_nodes mint_nodes_dup( const mint_nodes );

/** Makes the dst object identical to the src object. */
void mint_nodes_cpy( mint_nodes dst, const mint_nodes src );

/** Loads a node group from an open file. The file format is:

    nodes n s \n
    <update> \n
    <values and states> \n
    
    where: n is the number of nodes in the group, s the number of
    state variables for each node (in addition to input and output
    variables, which are always present); <update> is an OPTIONAL
    update function specification (see update.h) - no update function
    is set unless one is specified; <values and states> is an OPTIONAL
    specification of input, output and state values for each
    node. This usually exists when the file has been written by
    mint_nodes_save function, but not when it is hand-written
    file. The layout is such that all inputs come first, then all
    outputs, and then the state variables, fi any:

    n[0][0] ... n[0][i] ... n[0][size] \n
    n[1][0] ... n[1][i] ... n[1][size] \n
    ... \n
    n[states+1][0] ... n[states+1][size] \n
 */
mint_nodes mint_nodes_load( FILE * );

/** Saves a node group to file, see mint_nodes_load for the format. */
void mint_nodes_save( const mint_nodes, FILE * );

/** Saves a node group to file, apart from the values */
void mint_nodes_info( const mint_nodes, FILE * );

/** Saves to file only variable k of a node group. Useful to save
    outputs, for instance. */
void mint_nodes_save_var( const mint_nodes, int var, FILE * );

/** Returns the number of nodes. */
int mint_nodes_size( const mint_nodes );

/** Returns the number of state variables. */
int mint_nodes_states( const mint_nodes );

/** Retrieves ops.*/
struct mint_ops *mint_nodes_get_ops( const mint_nodes );

/** Retrieves number of ops. */
int mint_nodes_ops( const mint_nodes );

/** Set variable i of all nodes to x. */
void mint_nodes_set( mint_nodes n, int i, float x );

/** Resize a nodes object (UNTESTED). */
mint_nodes mint_nodes_resize( mint_nodes n, int newsize );

/** Update all nodes between min (included) and max (excluded), using
    all update ops in sequence. */
void mint_nodes_update( mint_nodes n, int min, int max );

/** Get nodes name string. Manipulating the returned string will
    directly change the name. */
mint_string mint_nodes_get_name( mint_nodes s );

/** Store the value of parameter i of property prop in *value. If the
    propert is found, return 1, if not found returns 0 (*value is not
    modified). If invalid parameter number, aborts. To check the
    existance of an op with no parameters, pass value=0 (i is ignored
    in these cases). */
int mint_nodes_get_property( mint_nodes n, const char *prop, int i,
			     float *value );

/** Set the value of parameter i of property prop. If the property is
    found, return 1, if not found returns 0. If invalid parameter
    number, aborts. */
int mint_nodes_set_property( mint_nodes n, const char *prop, int i,
			     float value );

/* see .c for docs */
int mint_nodes_bytes( int size, int states );

void mint_nodes_index2coord( mint_nodes n, int i, int *x, int *y );

int mint_nodes_coord2index( mint_nodes  n, int x, int y );

#endif

