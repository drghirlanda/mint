#ifndef MINT_STR_H
#define MINT_STR_H

#include <stdlib.h>
#include <stdio.h>

struct mint_str *mint_str_new( size_t len );

void mint_str_del( struct mint_str *str );

struct mint_str *mint_nodes_dup( const struct mint_str *src );

void mint_str_cpy( struct mint_str *dst, const struct mint_str *src );

struct mint_str *mint_str_load( FILE * );

void mint_str_save( const struct mint_str *, FILE * );

int mint_str_size( const struct mint_str * );

#endif

