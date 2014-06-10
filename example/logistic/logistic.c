#include "nodes.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  mint_nodes n;
  float x;
  long t;

  /* create a node group from file */
  file = fopen("logistic.arc", "r");
  n = mint_nodes_load( file );
  fclose( file );

  file = fopen( "logistic.dat", "w" );

  for( x=-6; x<=6; x+=0.01 ) {
    n[0][0] = x; /* set node input */
    for( t=0; t<500000; t++ )
      mint_nodes_update( n, 0, 1 ); 
    fprintf( file, "%f %f\n", x, n[0][1] ); /* write node output*/
  }

  return 0;
}
