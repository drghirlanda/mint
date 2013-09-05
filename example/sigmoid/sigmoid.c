#include "nodes.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  mint_nodes n;
  float x;

  /* create a node group from file */
  file = fopen("sigmoid.arc", "r");
  n = mint_nodes_load( file );
  fclose( file );

  file = fopen( "sigmoid.dat", "w" );

  for( x=-6; x<=6; x+=0.01 ) {
    n[0][0] = x; /* set node input */
    mint_nodes_update( n, 0, 1 ); 
    fprintf( file, "%f %f\n", x, n[0][1] ); /* write node output*/
  }

  return 0;
}
