#ifndef MINT_ARRAY_H
#define MINT_ARRAY_H

#include <stddef.h>
#include <stdio.h>

typedef void *mint_array;

mint_array mint_array_new( size_t ndim, size_t *size );

void mint_array_del( mint_array );

mint_array mint_array_dup( const mint_array );

void mint_array_cpy( mint_array dst, const mint_array src );

mint_array mint_array_load( FILE * );

void mint_array_save( mint_array , FILE * );

/* get the size of dimension i */
size_t mint_array_size( mint_array, size_t i );

/* set all values in an array slice to v. a slice is a rectangular
   subset along one or more dimensions. a value of -1 for the end of a
   slice means "until the end". thus to set all the rows, but only
   columns from 2 to 5, of a 2-dim array to value 1.5 one would use:

   int start[2] = {0, -1};
   int end[2] = {2, 5};
   mint_array_set_slice( a, 1.5, 0, -1, 2, 5 ); 
*/
void mint_array_set_slice( mint_array, float v, int *start, int *end );

/* set values at random between min and max */
/* void mint_array_set_slice_random( mint_array, float min, float max ); */

/* access extra information */
void *mint_array_get_extra( mint_array );

/* set extra information for a mint_array */
void mint_array_set_extra( mint_array, void * );

#endif

