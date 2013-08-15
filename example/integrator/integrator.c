#include "mint.h"
#include <stdio.h>
#include <time.h>

int main( void ) {
  FILE *file;
  mint_nodes n;
  int t, i, size;

  mint_random_seed( time(0) );

  /* create node group loading its architecture from file
     integrator.arc. the file describes a node group with 1 node and 1
     state variable per node, with update function integrator with
     parameters 10 and 0.2 (the first is the time constant, the second
     the leak. */
  file = fopen( "integrator.arc", "r" );
  n = mint_nodes_load( file );
  fclose( file );

  /* what we have done could have been done in code as follows: 

     n = mint_nodes_new( 1, 1 );
     mint_nodes_set_update( n, "integrator" );
     u = mint_nodes_get_update( n );
     mint_update_set_param( u, 0, 10 );  
     mint_update_set_param( u, 1, 0.5 ); 

     the advantage of the configuration file is that it is shorter and
     more flexible (no need to recompile if we change characteristics
     of the node, for example, or even the update function.
  */

  file = fopen( "output/integrator.dat", "w" ); /* output file */

  /* run 100 time steps. node receives input until t=50 */
  for( t=0; t<100; t++ ) {
    if( t<50 )
      n[0][0] = .5; /* set input to 0.5 */
    else
      n[0][0] = 0.; /* set input to 0 */
    mint_nodes_update( n, 0, 1 ); /* call update function */
    fprintf( file, "%d %f\n", t, n[0][1] ); /* print node output */
  }

  return 0;
}
