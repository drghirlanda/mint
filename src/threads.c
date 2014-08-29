#include "threads.h"
#include "utils.h"
#include "network.h"
#include "weights.h"
#include "nodes.h"
#include "spread.h"
#include "op.h"
#include "wop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MINT_USE_THREADS

#include <pthread.h>

struct mint_thread_data {
  mint_weights w;  /* weights object */
  mint_nodes n1;   /* nodes (pre-synpatic if w != 0) */ 
  mint_nodes n2;   /* nodes (post-synaptic if w != 0) */
  int min;         /* a thread operates on a [min, max[ slice */
  int max;       
  int var;
};

/* this function is called by mint_threads_mult for each thread, with
   approriate data to perform part or all of a matrix-vector
   multiplication  */
void *mint_threads_weights_helper( void *arg ) {
  struct mint_thread_data *td;

  td = (struct mint_thread_data *)arg;

  mint_check( td->w, "null weights argument" );
  mint_check( td->n1, "null nodes 1 argument" );
  mint_check( td->n2, "null nodes 2 argument" );
  mint_check( td->min >= 0, "td->min is negative" );
  mint_check( td->max > td->min, "td->max not larger than td->min" );

  mint_weights_run( td->w, td->n1, td->n2, td->min, td->max,
		    mint_op_weights_operate );
  mint_weights_run( td->w, td->n1, td->n2, td->min, td->max,
		    mint_op_weights_update );

  return 0;
}

void mint_threads_weights( struct mint_thread_data tmpl,
			   int nthreads ) {
  int step, max, i, j;
  pthread_t *tids;
  struct mint_thread_data *td;
  pthread_attr_t tattr;

  max = mint_weights_rows( tmpl.w );
  step = max / nthreads;

  if( !step ) { /* small job */
    tmpl.min = 0;
    tmpl.max = max;
    mint_threads_weights_helper( (void *) &tmpl );
    return;
  }

  /* initialize thread system and create thread data structures */
  pthread_attr_init( &tattr );
  pthread_attr_setscope( &tattr, PTHREAD_SCOPE_SYSTEM );
  pthread_attr_setdetachstate( &tattr, PTHREAD_CREATE_JOINABLE );
  td = malloc( nthreads * sizeof(struct mint_thread_data) );
  tids = malloc( nthreads * sizeof(pthread_t) );

  for( i = 0; i < nthreads; i++ ) {
    td[i].w = tmpl.w;
    td[i].n1 = tmpl.n1;
    td[i].n2 = tmpl.n2;
    td[i].min = i * step;
    td[i].max = i == nthreads - 1 ? max : (i+1) * step;
    td[i].var = tmpl.var;
    j = pthread_create( tids + i, &tattr, mint_threads_weights_helper, 
			(void *) (td + i) );
    mint_check( j==0, "thread creation failed" );
  }

  /* wait for other threads to complete */
  for( i = 0; i < nthreads; i++ )
    mint_check( pthread_join( tids[i], 0 ) == 0, "thread join failed" );

  free( tids );
  free( td );
}

/* this function is called by mint_threads_nodes for each thread */
void *mint_threads_nodes_helper( void *arg ) {
  struct mint_thread_data *td;
  int i;

  td = (struct mint_thread_data *)arg;

  mint_check( td->n1, "null nodes argument" );
  mint_check( td->min >= 0, "td->min is negative" );
  mint_check( td->max > td->min, "td->max not larger than td->min" );

  if( td->var > -1 ) { /* node reset */
    for( i = td->min; i < td->max; i++ )
      td->n1[ td->var ][ i ] = 0;
  } else
    mint_nodes_update( td->n1, td->min, td->max );

  return 0;
}

void mint_threads_nodes( struct mint_thread_data tmpl,
			 int nthreads ) {
  int step, max, i, j;
  pthread_t *tids;
  struct mint_thread_data *td;
  pthread_attr_t tattr;
			 
  max = mint_nodes_size( tmpl.n1 );
  step = max / nthreads;

  if( !step ) { /* small job */
    tmpl.min = 0;
    tmpl.max = max;
    mint_threads_nodes_helper( (void *) &tmpl );
    return;
  }

  /* initialize thread system and create thread data structures */
  pthread_attr_init( &tattr );
  pthread_attr_setscope( &tattr, PTHREAD_SCOPE_SYSTEM );
  pthread_attr_setdetachstate( &tattr, PTHREAD_CREATE_JOINABLE );
  td = malloc( nthreads * sizeof(struct mint_thread_data) );
  tids = malloc( nthreads * sizeof(pthread_t) );

  for( i = 0; i < nthreads; i++ ) {
    td[i].w = tmpl.w;
    td[i].n1 = tmpl.n1;
    td[i].n2 = tmpl.n2;
    td[i].min = i * step;
    td[i].max = i == nthreads -1 ? max : (i+1) * step;
    td[i].var = tmpl.var;
    j = pthread_create( tids + i, &tattr, mint_threads_nodes_helper, 
			(void *) (td + i) );
    mint_check( j==0, "thread creation failed" );
  }

  /* wait for threads to complete */
  for( i = 0; i < nthreads; i++ )
    mint_check( pthread_join( tids[i], 0 ) == 0, "thread join failed" );

  free( tids );
  free( td );
}

/* this function schedules node reset, matrix-vector multiplications,
   node updates, and weight updates in parallel */
void mint_threads_spread( struct mint_network *net, float *p ) {
  int wid, nid, i, nthreads, ifrom, ito, target, groups;
  int threaded_nodes, threaded_weights;
  struct mint_thread_data tmpl;
  struct mint_spread *spread;

  nthreads = p[0];
  threaded_nodes = p[1];
  threaded_weights = p[2];

  /* reset targets of weight matrices */
  groups = mint_network_groups(net);
  if( threaded_nodes ) {
    for( i = 0; i < mint_network_matrices( net ); i++ ) {
      tmpl.w = mint_network_get_weights( net, i );
      ito = mint_weights_get_to( tmpl.w );
      tmpl.n1 = mint_network_get_nodes( net, ito );
      tmpl.var = mint_weights_get_target( tmpl.w );
      tmpl.w = 0; /* not needed */
      tmpl.n2 = 0;
      mint_threads_nodes( tmpl, nthreads );
    }
  } else {
    for( i=0; i<mint_network_matrices( net ); i++ ) {
      tmpl.w = mint_network_get_weights( net, i );
      ito =  mint_weights_get_to( tmpl.w );
      target = mint_weights_get_target( tmpl.w );
      mint_nodes_set( mint_network_get_nodes(net, ito), target, 0. );
    }
  }

  spread = mint_network_get_spread( net );

  for( i = 0; i < mint_spread_len( spread ); i++ ) {

    wid = mint_spread_get_weights( spread, i );
    nid = mint_spread_get_nodes( spread, i );
    
    if( wid > -1 ) { /* operate and update matrix */
      tmpl.w = mint_network_get_weights( net, wid );
      ifrom = mint_weights_get_from( tmpl.w );
      tmpl.n1 = mint_network_get_nodes( net, ifrom );
      ito = mint_weights_get_to( tmpl.w );
      tmpl.n2 = mint_network_get_nodes( net, ito );
      tmpl.var = -1; /* not needed */
      if( threaded_weights )
	mint_threads_weights( tmpl, nthreads );
      else {
	mint_weights_run( tmpl.w, tmpl.n1, tmpl.n2, 0,
			  mint_weights_rows( tmpl.w ),
			  mint_op_weights_operate );
	mint_weights_run( tmpl.w, tmpl.n1, tmpl.n2, 0,
			  mint_weights_rows( tmpl.w ),
			  mint_op_weights_update );
      }
    } else if( nid > -1 ) { /* node update */
      tmpl.n1 = mint_network_get_nodes( net, nid );
      if( threaded_nodes ) {
	tmpl.w = 0;
	tmpl.n2 = 0;
	tmpl.var = -1;
	mint_threads_nodes( tmpl, nthreads );
      } else
	mint_nodes_update( tmpl.n1, 0, mint_nodes_size( tmpl.n1 ) );
    }
  }
}

void mint_network_init_threads( struct mint_network *net, float *p ) {
  struct mint_ops *ops;
  struct mint_op *op;
  float pdef[3] = { 1, 0, 0 };

  /* add threads_spread op to mint */
  if( !mint_op_exists( "threads_spread", mint_op_network_operate ) )
    mint_op_add( "threads_spread", mint_op_network_operate,
		 mint_threads_spread, 3, pdef );

  ops = mint_network_get_ops( net );

  /* delete spread op if present */
  op = mint_ops_get_name( ops, "spread", mint_op_network_operate );
  if( op )
    mint_op_del( op );
  
  /* now add threads_spread op to network */
  op = mint_op_new( "threads_spread", mint_op_network_operate );
  mint_op_set_param( op, 0, p[0] );
  mint_op_set_param( op, 1, p[1] );
  mint_op_set_param( op, 2, p[2] );
  mint_ops_append( ops, op );
  mint_op_del( op ); /* mint_ops_append stores a copy */
}

#else /* MINT_USE_THREADS */

void mint_network_init_threads( struct mint_network *net, float *p ) {
  MINT_UNUSED( net );
  MINT_UNUSED( p );
  mint_check( 0, "threads support not compiled into this libmint" );
}

#endif /* MINT_USE_THREADS */
