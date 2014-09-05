#include "nodes.h"
#include "nop.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  mint_nodes n;
  float x;

  /* create a node group from file */
  file = fopen("gradient.arc", "r");
  n = mint_nodes_load( file );
  fclose( file );

  file = fopen( "gradient.dat", "w" );

  for( x=-10; x<=10; x+=0.01 ) {
    n[0][0] = x;
    mint_nodes_update( n, 0, 1 ); 
    fprintf( file, "%f %f %f\n", x, n[1][0], n[2][0] );
  }

  return 0;
}
