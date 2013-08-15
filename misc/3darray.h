/* $Id: 3darray.h,v 1.5 2005/02/20 10:49:55 stefano Exp stefano $ */

#ifndef MINT_3DARRAY_H
#define MINT_3DARRAY_H

#include <stddef.h>
#include <stdio.h>

typedef float ***mint_3darray;

mint_3darray mint_3darray_new( size_t x, size_t y, size_t z );

void mint_3darray_del( mint_3darray  );

mint_3darray mint_3darray_dup( const mint_3darray );

void mint_3darray_cpy( mint_3darray dst, const mint_3darray src );

mint_3darray mint_3darray_load( FILE * );

void mint_3darray_save( mint_3darray , FILE * );

/* get the number of rows, columns and states */
size_t mint_3darray_x( mint_3darray );
size_t mint_3darray_y( mint_3darray );
size_t mint_3darray_z( mint_3darray );

/* set all values to v */
void mint_3darray_set( mint_3darray, float v );

/* set values at random between min and max */
void mint_3d_array_random( mint_3darray, float min, float max );

#endif

