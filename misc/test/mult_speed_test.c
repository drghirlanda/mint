#include <mint.h>
#include <time.h>

void naive_mult( mint_weights w, mint_nodes from, mint_nodes to,
		 int rmin, int rmax, float *p ) {
  unsigned int i, j, cols, rows, target;
  target = mint_weights_get_target(w);
  rows = mint_weights_rows(w);
  cols = mint_weights_cols(w);
  for( i=rmin; i<rmax; i++ ) { 
    for( j=0; j<cols; j++ ) {
      to[target][i] += w[0][i][j] * from[1][j];  
    }
  }
}

int main( void ) {
  clock_t start, stop;
  int i;
  mint_nodes from;
  mint_weights w;
  struct mint_ops *ops;
  struct mint_op *op;
  struct mint_network *net;
  int iterations = 100;
  FILE *f;

  mint_random_seed( time(0) );
  mint_op_add( "naive_mult", mint_op_weights_operate, naive_mult, 0, 0 );

  f = fopen("input/mult_speed.arc", "r" );
  net = mint_network_load( f );
  fclose( f );

  from = mint_network_nodes( net, 0 );
  for( i=0; i<mint_nodes_size(from); i++ )
    from[1][i] = mint_random();

  start = clock();
  for( i=0; i<iterations; i++ ) {
    mint_network_operate( net );
  }
  stop = clock();
  printf( "builtin mult: %g\n", (double)(stop-start) );

  /* replace weight operation with standard one */
  w = mint_network_weights( net, 0 );
  ops = mint_weights_get_ops( w );
  mint_ops_del_type( ops, mint_op_weights_update );
  op = mint_op_new("naive_mult");
  mint_ops_append( ops, op );
  mint_op_del( op );

  start = clock();
  for( i=0; i<iterations; i++ ) {
    mint_network_operate( net );
  }
  stop = clock();
  printf( "naive mult: %g\n", (double)(stop-start) );

  mint_network_del( net );
  return 0;
}
