#ifndef MINT_STR_H
#define MINT_STR_H

#include <stdio.h>

typedef char *mint_string;
 
mint_string mint_string_new( const char *name );

void mint_string_del( mint_string str );

mint_string mint_string_dup( const mint_string src );

void mint_string_cpy( mint_string dst, const mint_string src );

mint_string mint_string_load( FILE * );

void mint_string_save( const mint_string, FILE * );

int mint_string_size( const mint_string );

char *mint_string_char( mint_string );

int mint_string_find( mint_string, char );

mint_string mint_string_substr( mint_string str, int start, 
				int stop );

mint_string mint_string_incr( mint_string str );

int mint_string_numlen( int n );

#endif

