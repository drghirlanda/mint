#include "string.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* this is only to stop looking for '\0' in malformed user-supplied
   char arrays  */
#define MINT_STRMAX (10*1024)

#define MINT_STRBASE(s) ( (int *)s - 1)

/* we store the length of the string immediately before the pointer we
   return to the user, this is why we allocate sizeof(int) more
   bytes. we also allocate 1 byte for null termination. */
mint_string mint_string_alloc( int size ) {
  mint_string s;
  s = malloc( size + 1 + sizeof(int) );
  *( (int *)s ) = 0;            /* current length */ 
  s = (char *)( (int *)s + 1 ); /* advance by sizeof(int) bytes */
  s[0] = '\0';                  /* null termination */
  return s;
}

mint_string mint_string_new( const char *str ) {
  int len;
  mint_string s;

  len = 0;
  while( str[len] != '\0' && len < MINT_STRMAX )
    len++;

  mint_check( len<MINT_STRMAX, "cannot find end of supplied string" );

  s = mint_string_alloc( len );
  memcpy( s, str, len ); /* copy str to s */
  s[ len ] = '\0';       /* ensure null termination */
  return s;
}

void mint_string_del( mint_string str ) {
  free( MINT_STRBASE(str) );
}

mint_string mint_string_dup( const mint_string src ) {
  return mint_string_new( (const char *)src );
}

mint_string mint_string_load( FILE *f ) {
  long pos;
  int len;
  int c;
  mint_string s;
  char format[256];

  /* it's not a MINT string if it starts with a number */
  if( mint_values_waiting( f ) || feof( f ) )
    return 0;

  pos = ftell( f );
  len = 0;
  while( len<MINT_STRMAX && !feof(f) ) {
    c = fgetc( f );
    if( isalnum(c) || c == '-' || c == '.' || c == '_' ) len++;
    else break;
  }
  fseek( f, pos, SEEK_SET );
  if( !len ) return 0;
  s = mint_string_alloc( len );
  sprintf( format, "%%%ds", len );
  fscanf( f, format, s );
  s[ len ] = '\0';
  *MINT_STRBASE(s) = len;
  return s;
}

void mint_string_save( const mint_string s, FILE *f ) {
  if( !s ) return;
  fprintf( f, "%s ", s );
}

int mint_string_size( const mint_string s ) {
  mint_check( s, "asked for size of null object" );
  return *MINT_STRBASE(s);
}

char *mint_string_char( mint_string s ) {
  if( s ) return (char *)s;
  else return 0;
}

int mint_string_find( mint_string str, char c ) {
  int i, len;

  len = *MINT_STRBASE( str );
  for( i=0; i<len; i++ ) {
    if( str[i] == c )
      return i;
  }
  return -1;
}

mint_string mint_string_substr( mint_string str, 
				int start, 
				int stop ) {
  mint_string substr;
  int len;

  mint_check( str, "null string" );

  len = *MINT_STRBASE(str);
  mint_check( start>=0 && start<len, "start out of range" );
  mint_check( stop>=start && stop<=len, "stop out of range" );

  substr = mint_string_alloc( stop - start ); 
  memcpy( substr, str + start, stop - start );
  substr[ stop - start ] = '\0';
  return substr;
}

/* # of digits an integer will print to (adapted from a stackoverflow
   answer). we don't need to handle negative numbers. */
int mint_string_numlen( int n ) {
  if (n < 10) 
    return 1;
  return 1 + mint_string_numlen( n/10 );
}

mint_string mint_string_incr( mint_string str ) {
  int dotpos, j, oldlen, addedlen;
  mint_string newstr;

  oldlen = *MINT_STRBASE( str );
  dotpos = mint_string_find( str, '.' );
  if( dotpos == -1 ) {
    dotpos = oldlen;
    j = 0;
    addedlen = 2;
  } else {
    sscanf( str + dotpos + 1, "%d", &j );
    addedlen = mint_string_numlen(j+1) - mint_string_numlen(j);
  }
  newstr = mint_string_alloc( oldlen + addedlen );
  memcpy( newstr, str, dotpos );
  sprintf( newstr + dotpos, ".%d", j+1 );
  newstr[ oldlen + addedlen ] = '\0';
  *MINT_STRBASE( newstr ) = oldlen + addedlen;
  mint_string_del( str );
  return newstr;
}

#undef MINT_STRMAX
#undef MINT_STRBASE
