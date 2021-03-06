#ifndef MINT_UTILS_H
#define MINT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MINT_UNUSED(x) (void)(x)

#define mint_check( test, ... ) if(!(test)) { fprintf( stderr, "%s (%s:%d): ", __FUNCTION__, __FILE__, __LINE__ ); fprintf( stderr, __VA_ARGS__ ); fprintf( stderr, "\n" ); abort(); }

/* check whether a specific string is on file. if yes, return 1 and
   advance file position past the string. if no, return 0 and do not
   change file position. */
int mint_next_string( FILE *file, char *string, int len );

/* advance until a non space character, the latter remains available
   for reading and is also returned */
int mint_skip_space( FILE *file );

/* returns 1 if the first non-space charachter is a digit, a dot, a
   plus or a minus sign, otherwise returns 0; the character remains
   available for reading */
int mint_values_waiting( FILE *file );

/* returns 1 if the next string in file is a mint keyword (network,
   nodes, weights, spread), 0 otherwise. The file position is not
   changed, i.e., anything that was available for reading before
   mint_keyword is called remains available for reading after the
   function returns. */
int mint_keyword( char *name );

/* returns 1 if the string is the name of a default op for nodes,
   weights, or networks, 0 otherwise. */
int mint_default( const char *string );

#endif
