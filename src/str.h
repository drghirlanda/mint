#ifndef MINT_STR_H
#define MINT_STR_H

#include <stdio.h>

struct mint_str *mint_str_new( const char *name );

void mint_str_del( struct mint_str *str );

struct mint_str *mint_str_dup( const struct mint_str *src );

void mint_str_cpy( struct mint_str *dst, const struct mint_str *src );

struct mint_str *mint_str_load( FILE * );

void mint_str_save( const struct mint_str *, FILE * );

int mint_str_size( const struct mint_str * );

char *mint_str_char( struct mint_str * );

void mint_str_append( struct mint_str *, char * );

int mint_str_find( struct mint_str *, char );

struct mint_str *mint_str_substr( struct mint_str *str, int start, 
				  int stop );

void mint_str_incr( struct mint_str *str );

int mint_str_numlen( int n );

#endif

