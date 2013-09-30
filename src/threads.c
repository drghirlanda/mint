#include "threads.h"
#include "utils.h"
#include "network.h"
#include "weights.h"
#include "nodes.h"
#include "spread.h"
#include "op.h"
#include "optype.h"
#include "wop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MINT_USE_THREADS

#include <pthread.h>

struct mint_thread_wdata {
  mint_weights w;  /* weights object */
  mint_nodes n1;   /* nodes (pre-synpatic if w != 0) */ 
  mint_nodes n2;   /* nodes (post-synaptic if w != 0) */
  int min;         /* a thread operates on a [min, max[ slice */
  int max;       
};

/* this function is called by mint_threads_mult for each thread, with
   approriate data to perform part or all of a matrix-vector
   multiplication  */
void *mint_threads_weights_helper( void *arg ) {
  struct mint_thread_wdata *td;

  td = (struct mint_thread_wdata *)arg;
  mint_weights_operate( td->w, td->n1, td->n2, td->min, td->max );
  mint_weights_update( td->w, td->n1, td->n2, td->min, td->max );

  return 0;
}

void mint_threads_weights( pthread_t *tids,
			   struct mint_thread_wdata *td,
			   pthread_attr_t *tattr,
			   int nthreads ) {
  int step, max, i, j;
  
  max = mint_weights_rows( td[0].w );
  step = max / nthreads;

  td[0].max = step;
  j = pthread_create( tids, tattr, mint_threads_weights_helper, 
		      (void *)td );
  mint_check( j==0, "thread creation failed" ); 

  for( i = 1; i < nthreads - 1; i++ ) {
    memcpy( td + i, td, sizeof(struct mint_thread_wdata) );
    td[i].min = i * step;
    td[i].max = (i+1) * step;
    j = pthread_create( tids + i, tattr, mint_threads_weights_helper, 
			(void *) (td + i) );
    mint_check( j==0, "thread creation failed" );
  }

  /* do the last bit of work */
  memcpy( td + i, td, sizeof(struct mint_thread_wdata) );
  td[i].min = td[i-1].max;
  td[i].max = max; 
  mint_weights_operate( td[i].w, td[i].n1, td[i].n2, td[i].min, 
			td[i].max );
  mint_weights_update( td[i].w, td[i].n1, td[i].n2, td[i].min, 
		       td[i].max );

  /* wait for other threads to complete */
  for( i = 0; i < nthreads - 1; i++ )
    mint_check( pthread_join( tids[i], 0 ) == 0, 
		"thread join failed" );
}

/* this function schedules node reset, matrix-vector multiplications,
   node updates, and weight updates in parallel */
void mint_threads_spread( struct mint_network *net, float *p ) {
  int wid, nid, i, j, nthreads, ifrom, ito, target;
  int threaded_nodes, threaded_weights;
  struct mint_spread *spread;
  struct mint_thread_wdata *td;
  pthread_t *tids;
  pthread_attr_t tattr;
  mint_weights w;
  mint_nodes n;

  nthreads = p[0];
  threaded_nodes = p[1];
  threaded_weights = p[2];

  /* initialize thread system and create thread data structures */
  pthread_attr_init( &tattr );
  pthread_attr_setscope( &tattr, PTHREAD_SCOPE_SYSTEM );
  pthread_attr_setdetachstate( &tattr, PTHREAD_CREATE_JOINABLE );
  td = malloc( nthreads * sizeof(struct mint_thread_wdata) );
  tids = malloc( nthreads * sizeof(pthread_t) );

  /* reset targets of weight matrices */
  for( i=0; i<mint_network_matrices( net ); i++ ) {
    w = mint_network_weights( net, i );
    ito =  mint_weights_get_to( w );
    target = mint_weights_get_target( w );
    mint_nodes_set( mint_network_nodes(net, ito), target, 0. );
  }
  /* for( i = 0; i < mint_network_matrices( net ); i++ ) { */
  /*   tmpl.w = mint_network_weights( net, i ); */
  /*   tmpl.n1 = mint_network_nodes( net,  */
  /* 				  mint_weights_get_to( tmpl.w ) ); */
  /*   tmpl.n2 = 0; /\* not needed *\/ */
  /*   tmpl.var = mint_weights_get_target( tmpl.w ); */
  /*   tmpl.task = nodes_reset; */
  /*   tmpl.w = 0; /\* not needed *\/ */
  /*   if( threaded_nodes ) */
  /*     mint_threads_dispatch( &tmpl, nthreads ); */
  /*   else { */
  /*     for( j=0; j<mint_nodes_size( tmpl.n1 ); j++ ) */
  /* 	tmpl.n1[ tmpl.var ][ j ] = 0; */
  /*   } */
  /* } */

  spread = mint_network_get_spread( net );

  for( i = 0; i < mint_spread_len( spread ); i++ ) {

    wid = mint_spread_get_weights( spread, i );
    nid = mint_spread_get_nodes( spread, i );
    
    if( wid > -1 ) { /* operate and update matrix */
      td[0].w = mint_network_weights( net, wid );
      ifrom = mint_weights_get_from( td[0].w );
      td[0].n1 = mint_network_nodes( net, ifrom );
      ito = mint_weights_get_to( td[0].w );
      td[0].n2 = mint_network_nodes( net, ito );
      mint_threads_weights( tids, td, &tattr, nthreads );
    } else if( nid > -1 ) { /* node update */
      n = mint_network_nodes( net, nid );
      mint_nodes_update( n, 0, mint_nodes_size(n) );
    }
  }

  free( tids );
  free( td );
}

void mint_network_init_threads( struct mint_network *net, float *p ) {
  struct mint_ops *ops;
  struct mint_op *op;

  ops = mint_network_get_ops( net );
  op = mint_ops_get_name( ops, "run_spread", mint_op_network_operate );
  if( !op ) {
    op = mint_op_new( "run_spread", mint_op_network_operate );
    mint_ops_append( ops, op );
    mint_op_del( op );
    op = mint_ops_get_name( ops, "run_spread", mint_op_network_operate );
  }

  op->op = mint_threads_spread;
  op->nparam = 3;
  op->param = realloc( op->param, 3 * sizeof(float) );
  mint_op_set_param( op, 0, p[0] );
  mint_op_set_param( op, 1, p[1] );
  mint_op_set_param( op, 2, p[2] );
}

#else /* MINT_USE_THREADS */

void mint_network_init_threads( struct mint_network *net, float *p ) {
  mint_check( 0, "threads support not compiled into this libmint" );
}

#endif /* MINT_USE_THREADS */
