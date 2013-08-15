#ifndef MINT_WEIGHTS_H
#define MINT_WEIGHTS_H

#include "nodes.h"
#include <stddef.h>
#include <stdio.h>

struct mint_image; 

/** \file weights.h
    
    \brief Dealing with network weights (artificial synapses, or
    connections between network nodes).

    Defines the mint_weights type which is the data structure that
    represents a set of connections between two groups of network
    nodes (usually called a "weight matrix"), as well as functions to
    manipulate weights. */

/** A weights object can be treated as a three-dimensional array of
   floats, with the following semantics:

   w[0][i][j]   --> value of weight (i,j) \n
   w[1+k][i][j] --> k-th state variable of weight (i,j) \n

   State variables are optional, but a value variable is always
   created for every weights object. Info such as the number of rows,
   columns and states is provided by the functions below. */
typedef float ***mint_weights;

/** Create a (dense) weight matrix with given rows, columns and
     states. */
mint_weights mint_weights_new( unsigned int rows, unsigned int cols, 
			       unsigned int states );

/** Destroy a weight matrix, freeing up used memory. */
void mint_weights_del( mint_weights  );

/** Duplicate a weight matrix (create an exact copy). */
mint_weights mint_weights_dup( const mint_weights src );

/** Make weight matrix dst equal to weight matrix src. */ 
void mint_weights_cpy( mint_weights dst, mint_weights src );

/** Load a weight matrix from an open file. The file format is:

   weights r c s \n
   <ops> \n
   <values and states> \n

   where: r and c are the number of rows and columns in the weight
   matrix, and s is the number of state variables for each weight (in
   addition to its value); <ops> is a list of ops, which includes
   typically "from" and "to" settings, and may include update, operate
   and initialize ops. <values and states> is an OPTIONAL
   specification of weight values and state variables for each
   weight. This usually exists when the file has been created via a
   _save function, but not when it is a user written file. The layout
   of weight variables is such that all values come first, then all
   states. The column index is incremented faster, i.e. the weight are
   read (and stored by mint_weights_save) in the sequence:

   (1,1), (1,2) ... (1,c), (2,1) ... (2,c), ... , (r,1) ... (r,c).

   Thus this sequence is first used to save all weight values, then
   repeated to save the first state variable, if it exists, and so
   on. If values are given, initialization ops are not run. */
mint_weights mint_weights_load( FILE * );

void mint_weights_load_values( mint_weights w, FILE *f );

/** Save a weight matrix to file. */
void mint_weights_save( mint_weights , FILE * );

/** Get the number of rows */
unsigned int mint_weights_rows( mint_weights  );

/** Get the number of columns */
unsigned int mint_weights_cols( mint_weights  );

/** Get the number of states */
unsigned int mint_weights_states( mint_weights  );

/** Retrieve index of 'pre-synaptic' node group. See network.h. */
unsigned int mint_weights_get_from( const mint_weights w );

/** Set index of 'pre-synaptic' node group. See network.h. */
void mint_weights_set_from( mint_weights w, unsigned int );

/** Retrieve index of 'post-synaptic' node group. See network.h. */
unsigned int mint_weights_get_to( const mint_weights w );

/** Set index of 'post-synaptic' node group. See network.h. */
void mint_weights_set_to( mint_weights w, unsigned int );

/** Retrieve which variable of 'post-synaptic' that is the target of this
    matrix (it will be variable 0, i.e., node input, unless you change
    it explicitly. */
unsigned int mint_weights_get_target( const mint_weights w );

/** Set which variable 'post-synaptic' node group is the target of
    this weight matrix. */
void mint_weights_set_target( mint_weights w, unsigned int );

/** Retrieve op list. */
struct mint_ops *mint_weights_get_ops( const mint_weights );

/** Operate this weight matrix using rows between rmin (included) and
    rmax (excluded). By default, this performs a matrix-vector
    multiplication of pre-synaptic nodes and weights, adding the
    result to the input of postsynaptic nodes. Different weight
    operations can be specified when loading a weight matrix. */
void mint_weights_operate( mint_weights w, mint_nodes pre,
			   mint_nodes post, int rmin, int rmax );

/** Update weights in rows between rmin (included) and rmax
    (excluded). This function invokes all update ops defined for the
    weight matrix. */
void mint_weights_update( mint_weights w, mint_nodes pre,
			   mint_nodes post, int rmin, int rmax );

/** Initialize weights in rows between rmin (included) and rmax
    (excluded). This function calls all initialization ops defined
    for the weight matrix. */
void mint_weights_init( mint_weights w, int rmin, int rmax );

/** Connect pre and post synaptic nodes through weight matrix w. This
    function calls all ops defined for w with type
    mint_op_weights_connect. */
void mint_weights_connect( mint_weights w, mint_nodes pre,
			   mint_nodes post, int rmin, int rmax );

/** Create a sparse weight matrix. The matrix is initially empty (no
    non-zero values) and can be filled with mint_weights_set_row. */
mint_weights mint_weights_sparse_new( unsigned int rows, 
				      unsigned int cols,
				      unsigned int states );

/** Returns 1 if matrix is sparse, 0 otherwise. */
int mint_weights_is_sparse( const mint_weights );

/** Returns number of nonzero elements in a weight matrix. */
int mint_weights_nonzero( const mint_weights );
 
/** Add weights to a sparse weight matrix. Weights in row r with
    column indices in ind are set to values in val; len is the common
    length of val and ind. This function works with both full and
    sparse matrices (it is more helpful with sparse matrices).

    NOTES: When used with sparse matrices, the function does not check
    for duplicates. This should be fixed.

    Also, the function does not sort the resulting index arrays. This
    may be a performance penalty in matrix-vector multiplication. */
void mint_weights_set_row( mint_weights w,  int r, int len,
			   float *val, unsigned int *ind );

/** Returns number of elements existing in a row (equal to the number
    of columns for dense matrices, possibly less for sparse
    matrices. */
unsigned int mint_weights_rowlen( mint_weights w, int r );

/** Returns the column indices of elements in row r of a sparse
    matrix, or 0 for a dense matrix. */
unsigned int *mint_weights_colind( mint_weights w, int r );

/** Sets the element of a sparse matrix to a given value (element is
    created if it does not exist. */
void mint_weights_set( mint_weights w, int s, int r, int c, float x );

/** Create a copy of matrix w, keeping only weights whose values are
    larger than cutoff in absolute value.  State variables for these
    wieghts are also copied. The returned matrix is sparse if
    sparse==1, and dense if sparse==0. The multiplication op is set to
    mult or mult_sparse as appropriate. All other ops are copied
    unmodified.

    NOTE: This function can be used to convert between sparse and
    dense matrices, by using cutoff=Inf and setting sparse as
    appropriate. */
mint_weights mint_weights_prune( mint_weights w, float cutoff, int sparse );

/** Freeze (f true) or unfreeze (f false) weight matrix w. When a
    matrix is frozen, calss to mint_weights_update have no effect. */
void mint_weights_freeze( mint_weights w, int f );

/** Get freeze status of weight matrix w. */
int mint_weights_frozen( mint_weights w );

#endif

