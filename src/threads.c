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

#ifdef MINT_USE_THREADS

#include <pthread.h>

/* holds info needed by mint_threads_mult_helper and
   mint_threads_spread_helper to perform a matrix-vector
   multiplication (part of it in the first case, all of it in the
   second) */
struct mint_thread_wdata {
  mint_weights w;  /* weights object */
  mint_nodes from; /* pre-synpatic nodes */ 
  mint_nodes to;   /* post-synaptic nodes */
  int rmin;        /* each thread multiplies rows in [rmin,rmax[ */
  int rmax;       
};

/* holds info needed by mint_threads_nodes_helper to perform part of a
   node update */
struct mint_thread_ndata {
  mint_nodes n; /* nodes object to be updated */ 
  int imin;     /* each thread updates nodes in [imin,imax[ */
  int imax;
};

/* this function is called by mint_threads_mult for each thread, with
   approriate data to perform part or all of a matrix-vector
   multiplication  */
void *mint_threads_mult_helper( void *arg ) {
  struct mint_thread_wdata *td;
  td = (struct mint_thread_wdata *)arg;
  if( mint_weights_is_sparse( td->w ) )
    mint_weights_mult_sparse( td->w,td->from,td->to,td->rmin,td->rmax,0 );
  else
    mint_weights_mult( td->w,td->from,td->to,td->rmin,td->rmax,0 );
  return 0;
}

/* this function breaks up a matrix-vector multiplication into
   different threads, by assigning a strip of the matrix to each
   thread */
void mint_threads_mult( mint_weights w, mint_nodes from, mint_nodes to,
			int rmin, int rmax, float *p ) {
  struct mint_thread_wdata *td;
  pthread_t *tids;
  pthread_attr_t tattr;
  int rstep, i, nthreads;

  nthreads = p[0];

  /* rstep is the number of rows on which each thread will work */
  rstep = (rmax-rmin) / nthreads;
  
  /* too few rows to warrant threads, do standard multiplication */
  if( rstep==0 ) {
    if( mint_weights_is_sparse(w) )
      mint_weights_mult_sparse( w,from,to,rmin,rmax,0 );
    else
      mint_weights_mult( w,from,to,rmin,rmax,0 );
    return;
  }

  /* we start nthreads-1 thread because we keep the last strip of the
     matrix for the curent thread. each mint_thread_wdata struct holds
     the information needed by mint_threads_mult_helper to multiply
     one strip of the matrix */
  td = malloc( (nthreads-1)*sizeof(struct mint_thread_wdata) );
  tids = malloc( (nthreads-1)*sizeof(pthread_t) );
  pthread_attr_init( &tattr );
  pthread_attr_setscope( &tattr, PTHREAD_SCOPE_SYSTEM );

  /* initialize thread arguments */
  for( i=0; i<nthreads-1; i++ ) {
    td[i].w = w;
    td[i].from = from;
    td[i].to = to;
    td[i].rmin = rmin + i*rstep;
    td[i].rmax = td[i].rmin + rstep;
  }
  
  /* create nthreads-1 threads each working on rstep rows */
  for( i=0; i<nthreads-1; i++ ) {
    mint_check( pthread_create( &tids[i], &tattr, 
				mint_threads_mult_helper, 
				(void *) &td[i] ) == 0,
		"thread creation failed" ); 
  }

  /* this thread does the leftover work */ 
  if( mint_weights_is_sparse(w) )
    mint_weights_mult_sparse( w,from,to,td[nthreads-2].rmax,rmax,0 );
  else
    mint_weights_mult( w,from,to,td[nthreads-2].rmax,rmax,0 );

  for( i=0; i<nthreads-1; i++ ) {
    mint_check( pthread_join( tids[i], 0 ) == 0, "thread join failed" );
  }
  
  free(tids);
  free(td);
}

/* this function is called by mint_threads_spread for each thread,
   with approriate data to perform a matrix-vector multiplication  */
void *mint_threads_spread_helper( void *arg ) {
  struct mint_thread_wdata *td;
  td = (struct mint_thread_wdata *)arg;
  mint_weights_operate( td->w, td->from, td->to, td->rmin, td->rmax );
  return 0;
}

/* this function schedules matrix-vector multiplications in parallel */
void mint_threads_spread( struct mint_network *net, float *p ) {
  struct mint_thread_wdata *td;
  mint_weights w;
  mint_nodes n;
  pthread_t *tids;
  pthread_attr_t tattr;
  int wid, nid, i, j, nthreads, used_threads;
  struct mint_spread *spread;

  nthreads = p[0];

  td = malloc( nthreads*sizeof(struct mint_thread_wdata) );
  tids = malloc( (nthreads-1)*sizeof(pthread_t) );
  pthread_attr_init( &tattr );
  pthread_attr_setscope( &tattr, PTHREAD_SCOPE_SYSTEM );

  for( i=0; i<mint_network_matrices(net); i++ ) {
    w = mint_network_weights(net,i);
    nid =  mint_weights_get_to( w );
    j = mint_weights_get_target( w );
    mint_nodes_set( mint_network_nodes(net, nid), j, 0. );
  }

  used_threads = 0;
  spread = mint_network_get_spread( net );
  for( i=0; i<mint_spread_len( spread ); i++ ) {
    wid = mint_spread_get_weights(spread, i);
    nid = mint_spread_get_nodes(spread, i);
    if( wid>=0 ) { /* schedule matrix-vector multiplication */
      if( used_threads==nthreads ) { /* wait for thread completion */
	/* FIX we really need only 1 thread to complete, but here we
	   wait for all of them... we can add at complete flag in the
	   thread data and call pthread_exit(0) in the thread helper,
	   then loop until a thread has the complete flag set, and
	   reclaim that td space for the next thread. but first we'll
	   get this working. */
	for( j=0; j<used_threads; j++ )
	  mint_check( pthread_join( tids[j], 0 ) == 0, "thread join failed" );
	used_threads = 0;
      }
      w = mint_network_weights( net, wid );
      td[used_threads].w = w;
      td[used_threads].from = mint_network_nodes(net,mint_weights_get_from(w));
      td[used_threads].to = mint_network_nodes( net, mint_weights_get_to(w) );
      td[used_threads].rmin = 0;
      td[used_threads].rmax = mint_weights_rows(w);
      mint_check( pthread_create( &tids[used_threads], 
				  &tattr, mint_threads_spread_helper, 
				  (void *) &td[used_threads] ) == 0, 
		  "thread creation failed" );
      used_threads++;
    }
    if( nid>=0 ) {
      for( j=0; j<used_threads; j++ )
	mint_check( pthread_join( tids[j], 0 ) == 0, "thread join failed" );
      used_threads = 0;
      n = mint_network_nodes(net, nid);
      mint_nodes_update( n, 0, mint_nodes_size(n) );
    }
  }

  free(tids);
  free(td);
}

void mint_network_init_threads( struct mint_network *net, float *p ) {
  struct mint_ops *ops;
  struct mint_op *op;
  mint_weights w;
  int i, num_threads, threaded_spread, threaded_mult;
  float default_threads = 1;

  num_threads = p[0];
  threaded_spread = p[1];
  threaded_mult = p[2];

  if( threaded_spread ) {

    if( !mint_op_exists( "threads_spread" ) )
      mint_op_add( "threads_spread", mint_op_network_operate,
		   (void *)mint_threads_spread, 1, &default_threads );

    ops = mint_network_get_ops( net );
    mint_ops_del_name( ops, "run_spread" );
    op = mint_op_new( "threads_spread" );
    mint_op_set_param( op, 0, num_threads );
    mint_ops_append( ops, op );
    mint_op_del( op );
  }

  if( threaded_mult ) {

    if( !mint_op_exists( "threads_mult" ) )
      mint_op_add( "threads_mult", mint_op_weights_operate,
		   (void *)mint_threads_mult, 1, &default_threads );

    op = mint_op_new( "threads_mult" );
    mint_op_set_param( op, 0, num_threads );

    for( i=0; i<mint_network_matrices( net ); i++ ) {
      w = mint_network_weights( net, i );
      ops = mint_weights_get_ops( w );
      mint_ops_del_type( ops, mint_op_weights_operate );
      mint_ops_append( ops, op );
    }

    mint_op_del( op );

  }
}

#else /* MINT_USE_THREADS */

void mint_network_init_threads( struct mint_network *net, float *p ) {
  mint_check( 0, "threads support not compiled into this libmint" );
}

#endif /* MINT_USE_THREADS */
