#include <mint.h>
#include <stdio.h>

void constant_out( mint_nodes n, int min, int max, float *p ) {
  int i;
  for( i=min; i<max; i++ )     
    n[1][i] = p[0];
}

int main( void ) {
  int i, k;
  FILE *f;
  float x[1];
  mint_nodes n, n2;
  struct mint_ops *ops;

  printf( "creating n... " );
  n = mint_nodes_new( 25, 1 );
  printf( "done\n" );

  /* this checks that pointers are set up correctly */
  printf( "writing in n... " );
  k = 0;
  for( i=0; i<mint_nodes_size(n); i++ ) {
    n[0][i] = k++;
    n[1][i] = k++;
    n[2][i] = k++;
  }
  printf( "done\n" );

  /* set up a new nupdate function */
  printf( "creating a new update function... " );
  x[0] = 0.;
  mint_op_add( "constant", mint_op_nodes_update, 
	       (void *)constant_out, 1, x );
  printf( "done\n" );

  /* this checks the nupdate setting mechanism */
  printf( "setting new update function in n... " );
  fflush( stdout );
  ops = mint_nodes_get_ops( n );
  mint_ops_del_type( ops, mint_op_nodes_update );
  mint_ops_append( ops, mint_op_new("constant") );
  printf( "done\n" );

  f = fopen( "output/n", "w" );
  mint_nodes_save( n, f );
  fclose( f );

  /* this checks node copying */
  n2 = mint_nodes_dup( n );
  f = fopen( "output/n2", "w" );
  mint_nodes_save( n2, f );
  fclose( f );

  /* now we check node loading */
  mint_nodes_del( n2 );
  f = fopen( "output/n", "r" );
  n2 = mint_nodes_load( f );
  fclose( f );
  f = fopen( "output/nrestore", "w" );
  mint_nodes_save( n2, f );
  fclose( f );

  mint_nodes_del( n );
  mint_nodes_del( n2 );

  return 0;
}
