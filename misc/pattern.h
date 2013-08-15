#ifndef MINT_PATTERN_H
#define MINT_PATTERN_H

#include "nodes.h"
#include <stdio.h>

typedef float **mint_pattern;

/* management of patterns: */

mint_pattern mint_pattern_new( int rows, int cols );
void mint_pattern_del( mint_pattern );
mint_pattern mint_pattern_dup( const mint_pattern );
void mint_pattern_cpy( mint_pattern p1, const mint_pattern p2 );
void mint_pattern_save( const mint_pattern p, FILE *dest );
mint_pattern mint_pattern_load( FILE * );

void mint_pattern_paste( const mint_pattern, mint_nodes,
             int nr, float x, float y );

/* paste a mint_pattern onto an array of nodes at a random position */
void mint_pattern_paste_random( const mint_pattern, mint_nodes, int nr );

/* paste a mint_pattern onto an array of nodes, centered */
void mint_pattern_paste_center( const mint_pattern, mint_nodes, int nr );

#endif
