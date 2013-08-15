#include "mint.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  mint_nodes n;
  int i, size;
  float x;

  /* create a node group from file */
  file = fopen("sigmoid.arc", "r");
  n = mint_nodes_load( file );
  fclose( file );

  /* the contents of sigmoid.arc allow to create a node group equivalent to the
     one produced by this code:

  n = mint_nodes_new( 1, 1 );
  op = mint_op_new( "sigmoid" );
  mint_op_set_param( op, 0, 0.1 );
  mint_op_set_param( op, 1, 1 );
  ops = mint_nodes_get_ops( n );
  mint_ops_append( ops, op );
  mint_op_del( op );
  */

  file = fopen( "output/sigmoid.dat", "w" );

  for( x=-2; x<=4; x+=0.01 ) {
    n[0][0] = x; /* set node input */
    mint_nodes_update( n, 0, 1 ); 
    fprintf( file, "%f %f\n", x, n[0][1] ); /* write node output*/
  }

  return 0;
}
