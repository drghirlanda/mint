#include "network.h"
#include "op.h"
#include "random.h"
#include "utils.h"
#include "threads.h"
#include "spread.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

struct mint_network {
  int groups;                 /* number of node groups */
  int matrices;               /* number of weight matrices */
  int size;                   /* total number of nodes */ 
  mint_nodes *n;              /* array of node arrays */
  mint_weights *w;            /* array of weight matrices */
  struct mint_spread *spread; /* spread scheme (see spread.h) */
  struct mint_ops *ops;
};


/* internal function used to check compatiblity of dimensions between
   weight matrix and connected node groups */
void mint_weights_compatibility( mint_weights w, mint_nodes from, 
				 mint_nodes to ) {
  int rows, cols, fromsize, tosize;

  rows = mint_weights_rows( w );
  cols = mint_weights_cols( w );
  fromsize = mint_nodes_size( from );
  tosize = mint_nodes_size( to );

  if( rows != tosize ) {
    fprintf( stderr, 
	     "mint_network_load: weight rows (%d) != 'to' node size (%d)\n",
	     rows, tosize );
    abort();
  }

  if( cols != fromsize ) {
    fprintf( stderr, 
	     "mint_network_load: weight cols (%d) != 'from' nodes size (%d)\n",
	     cols, fromsize );
    abort();
  }
}

static struct mint_network *mint_network_alloc( int groups, 
						int matrices ) {
  int i;
  struct mint_network *net;
  net = malloc( sizeof(struct mint_network) );
  net->n = malloc( groups * sizeof(mint_nodes) );
  for( i=0; i<groups; i++ ) net->n[i] = 0;
  net->w = malloc( matrices * sizeof(mint_weights) );
  for( i=0; i<matrices; i++ ) net->w[i] = 0;
  net->groups = groups;
  net->matrices = matrices;
  net->spread = 0;
  net->ops = mint_ops_new();
  return net;
}

struct mint_network *mint_network_dup( const struct mint_network *net1 ) {
  int i;
  struct mint_network *net2;
  net2 = mint_network_alloc( net1->groups, net1->matrices );
  net2->size = net1->size;
  mint_spread_del( net2->spread );
  net2->spread = mint_spread_dup( net1->spread );
  for( i=0; i<net1->groups; i++ ) 
    net2->n[i] = mint_nodes_dup( net1->n[i] );
  for( i=0; i<net1->matrices; i++ ) 
    net2->w[i] = mint_weights_dup( net1->w[i] );
  net2->ops = mint_ops_dup( net1->ops );
  return net2;
}

void mint_network_del( struct mint_network *net )
{
  int i;
  if( !net ) return;
  mint_spread_del( net->spread );
  for( i=0; i<net->groups; i++ ) 
    mint_nodes_del( net->n[i] );
  free( net->n );
  for( i=0; i<net->matrices; i++ ) 
    mint_weights_del( net->w[i] );  
  free( net->w );
  mint_ops_del( net->ops );
  free( net );
}

void mint_network_cpy( struct mint_network *net1, 
		       const struct mint_network *net2 ) {
  int i;
  mint_check( net1 != 0, "destination network is null" );
  mint_check( net2 != 0, "source network is null" );
  if( net1 == net2 ) return;
  
  if( net1->groups != net2->groups || net1->matrices != net2->matrices ) {
    mint_network_del( net1 );
    net1 = mint_network_alloc( net2->groups, net2->matrices );
  }
  net1->size = net2->size;
  mint_spread_del( net1->spread );
  net1->spread = mint_spread_dup( net2->spread );
  for( i=0; i<net1->groups; i++ ) 
    mint_nodes_cpy( net1->n[i], net2->n[i] );
  for( i=0; i<net1->matrices; i++ ) 
    mint_weights_cpy( net1->w[i], net2->w[i] );
  mint_ops_del( net1->ops );
  net1->ops = mint_ops_dup( net2->ops );
}

void mint_network_freeze( struct mint_network *net, int i, int f ) {
  if( i >= 0 ) {
    mint_check( i < net->matrices, 
		"weight matrix index %d too large (max=%d)",
		i, net->matrices - 1);
    mint_weights_freeze( net->w[i], f );
  } else {
    for( i=0; i<net->matrices; i++ )
      mint_weights_freeze( net->w[i], f );
  }
}

int mint_network_frozen( struct mint_network *net ) {
  int i;
  int frozen = 0;
  for( i=0; i<net->matrices; i++ )
    frozen += mint_weights_frozen( net->w[i] );
  return frozen;
}

void mint_network_save( const struct mint_network *net, FILE *dest ) {
  int i;
  fprintf( dest, "network %d %d\n", net->groups, net->matrices );

  mint_ops_save( net->ops, dest );

  for( i=0; i<net->groups; i++ ) 
    mint_nodes_save( net->n[i], dest );

  for( i=0; i<net->matrices; i++ )
    mint_weights_save( net->w[i], dest );

  mint_spread_save( net->spread, dest );
}

struct mint_network *mint_network_load( FILE *file ) {
  int i, groups, matrices, read, from, to;
  struct mint_network *net;
  struct mint_op*op;
  struct mint_spread *spread;

  read = fscanf( file, " network %d", &groups );
  mint_check( read==1, "cannot read number or node groups" );
  read = fscanf( file, " %d", &matrices );
  mint_check( read==1, "cannot read number or weight matrices" );

  net = mint_network_alloc( groups, matrices );

  net->ops = mint_ops_load( file );

  for( i=0; i<net->groups; i++ )
    net->n[i] = mint_nodes_load( file );

  for( i=0; i<net->matrices; i++ ) {
    net->w[i] = mint_weights_load( file );

    from = mint_weights_get_from( net->w[i] );
    to = mint_weights_get_to( net->w[i] );

    mint_weights_compatibility( net->w[i], net->n[from], net->n[to] );

    /* now we can run connect ops for this matrix */
    mint_weights_connect( net->w[i], net->n[from], net->n[to], 
			  0, mint_weights_rows( net->w[i] ) );
  }

   /* we now run init ops (which may set a spread) and also attempt to
     read a spread from file (which takes precedence). if after this
     there is still no spread, and there is no asynchronous op (which
     takes no spread), we add synchronous spread as a default. */
  mint_network_init( net );

  /* if there is a spread on file, it takes precedence */
  spread = mint_spread_load( file );
  if( spread ) {
    fprintf( stderr, "mint_network_load:" 
	     "spread on file overrides existing spread\n" );
    mint_spread_del( net->spread );
    net->spread = spread;
  }

  if( !net->spread && mint_ops_find( net->ops, "asynchronous" )<0 ) {
    op = mint_op_new( "synchronous" );
    mint_ops_append( net->ops, op );
    mint_op_run( op, net );
    mint_op_del( op ); /* a copy has been stored in net->ops */
  }

  return net;
}

mint_nodes mint_network_nodes( struct mint_network *net, int i ) {
  mint_check( i>=0 && i<net->groups, "index %d out of range 0-%d",
	      i, net->groups - 1);
  return net->n[i];
}

mint_weights mint_network_weights( struct mint_network *net, int i ) {
  mint_check( i>=0 && i<net->matrices, "index %d out of range 0-%d",
	      i, net->matrices - 1);
  return net->w[i];
}

int mint_network_groups( const struct mint_network *n ) {
  return n->groups;
}

int mint_network_matrices( const struct mint_network *n ) {
  return n->matrices;
}

int mint_network_size( struct mint_network *net ) {
  int i;
  if( !net->size ) {
    for( i=0; i<net->groups; i++ )
      net->size += mint_nodes_size( net->n[i] );
  }
  return net->size;
}

void mint_network_add( struct mint_network *net1,
		       int n1,
		       const struct mint_network *net2,
		       int n2, 
		       const mint_weights w,
		       int s ) {
  int i, j, g1, g2, m1, m2, r, c, len1, len2;

  g1 = net1->groups; 
  g2 = net2->groups; 
  m1 = net1->matrices; 
  m2 = net2->matrices; 
  
  mint_check( n1>=0 && n1<g1, "2nd arg (%d) out of range 0-%d", n1, g1-1 ); 
  mint_check( n2>=0 && n2<g2, "4th arg (%d) out of range 0-%d", n2, g2-1 ); 

  net1->n = realloc( net1->n, (g1+g2)*sizeof(mint_nodes) );
  net1->w = realloc( net1->w, (m1+m2+1)*sizeof(mint_weights) );
  net1->groups += g2;
  net1->matrices += m2 + 1; /* +1 for new matrix */

  /* 1st: add nodes and matrices with new indices */
  for( i=0; i<g2; i++ ) 
    net1->n[ g1+i ] = mint_nodes_dup( net2->n[i] );
  for( i=0; i<m2; i++ ) {
    net1->w[ m1+i ] = mint_weights_dup( net2->w[i] );
    mint_weights_set_from( net1->w[ m1+i ], 
			   g1 + mint_weights_get_from( net2->w[i] ) );
    mint_weights_set_to( net1->w[ m1+i ], 
			 g1 + mint_weights_get_to( net2->w[i] ) );
  }

  /* 2nd: add the new matrix */
  if( w ) {
    net1->w[ m1 + m2 ] = mint_weights_dup( w );
  } else {
    r = mint_nodes_size( net2->n[n2] );
    c = mint_nodes_size( net1->n[n1] );
    net1->w[ m1 + m2 ] = mint_weights_new( r, c, s );
    mint_weights_set_from( net1->w[ m1 + m2 ], n1 );
    mint_weights_set_to( net1->w[ m1 + m2 ], g1 + n2 );
    for( i=0; i<r; i++ ) {
      for( j=0; j<c; j++ ) 
	net1->w[ m1 + m2 ][0][i][j] = 1. / r*c;
    }
  }

  /* 3rd: modify spread scheme */
  len1 = mint_spread_len( net1->spread );
  len2 = mint_spread_len( net2->spread );
  mint_spread_add_step( net1, len1, m1+m2, g1+n2 );
  for( i=0; i<len2; i++ ) { 
    mint_spread_add_step( net1, len1+1+i, 
			  mint_spread_get_weights( net2->spread, i ),
			  mint_spread_get_nodes( net1->spread, i ) );
  }
}

void mint_network_graph( const struct mint_network *net, FILE *f ) {
  int g, m, i, len;
  mint_nodes n;
  mint_weights w;

  fprintf( f, "digraph network {\n" );
  fprintf( f, "rankdir=LR\n" );
  fprintf( f, "spread [label=\"\\N " );
  len = mint_spread_len( net->spread );
  if( len>0 ) {
    fprintf( f, "\\nw: " ); 
    for( i=0; i<len; i++ )
      fprintf( f, "%d ", mint_spread_get_weights( net->spread, i ) );
    fprintf( f, "\\nn: " );
    for( i=0; i<len; i++ )
      fprintf( f, "%d ", mint_spread_get_nodes( net->spread, i ) );
  } else
    fprintf( f, "async %d", -len );
  fprintf( f, "\", fontsize=12, shape=none]\n" );
  g = mint_network_groups( net );
  for( i=0; i<g; i++ ) {
    n = mint_network_nodes( (struct mint_network *)net, i );
    fprintf( f, "n%d [label=\"\\N %d %d ", i, mint_nodes_size(n),
	     mint_nodes_states(n) );
    mint_ops_save( mint_nodes_get_ops(n), f );
    fprintf( f, "\"]\n" );
  }
  m = mint_network_matrices( net );
  for( i=0; i<m; i++ ) {
    w = mint_network_weights( (struct mint_network *)net, i );
    fprintf( f, "n%d -> n%d [label=\"w%d %d %d %d ", 
	     mint_weights_get_from(w), mint_weights_get_to(w), i,
	     mint_weights_rows(w), mint_weights_cols(w),
	     mint_weights_states(w) );
    mint_ops_save( mint_weights_get_ops(w), f );
    fprintf( f, "\"]\n" );
  }
  fprintf( f, "}\n" );
}

void mint_network_graph_full( const struct mint_network *net_in, FILE *f ) {
  int g, m, i, j, k, ifrom, ito, len;
  mint_nodes n, nfrom, nto;
  float wmax, val;
  mint_weights w;

  /* we don't modify net_in, but we access nodes and weights through
     non-const functions so we cast the const away */
  struct mint_network *net = (struct mint_network *)net_in;

  fprintf( f, "digraph network {\n" );
  fprintf(f,"graph [rankdir=LR,ranksep=1.5,fontname=Helvetica,fontsize=12,nodesep=.75]\n");
  fprintf(f,"node [shape=circle, fontname=Helvetica,fontsize=12]\n");
  fprintf(f,"edge [fontname=Helvetica,fontsize=12]\n");

  /* spread */
  fprintf( f, "spread [label=\"\\N " );
  len = mint_spread_len( net->spread );
  if( len>0 ) {
    fprintf( f, "\\nw: " ); 
    for( i=0; i<len; i++ )
      fprintf( f, "%d ", mint_spread_get_weights( net->spread, i ) );
    fprintf( f, "\\nn: " );
    for( i=0; i<len; i++ )
      fprintf( f, "%d ", mint_spread_get_nodes( net->spread, i ) );
  } else
    fprintf( f, "async %d", -len );
  fprintf( f, "\", shape=none]\n" );

  /* node groups */
  g = mint_network_groups( net );
  for( i=0; i<g; i++ ) {
    n = mint_network_nodes( (struct mint_network *)net, i );
    fprintf( f, "subgraph cluster_%d {\n color=grey label=\"n%d %d %d ", 
	     i, i, mint_nodes_size(n), mint_nodes_states(n) );
    mint_ops_save( mint_nodes_get_ops(n), f );
    fprintf( f, "\"\n" );
    for( j=0; j<mint_nodes_size(n); j++ ) {
      fprintf( f, "n%d_%d [label=\"%d\"]\n", i, j, j );
    }
    fprintf( f, "}\n" );
  }

  /* weight matrices */
  m = mint_network_matrices( net );
  wmax = 0;
  for( i=0; i<m; i++ ) {
    w = mint_network_weights( net, i );
    ifrom = mint_weights_get_from(w);
    nfrom = mint_network_nodes( net, ifrom );
    ito = mint_weights_get_to(w);
    nto = mint_network_nodes( net, mint_weights_get_to(w) );
    for( j=0; j<mint_nodes_size(nfrom); j++ ) {
      for( k=0; k<mint_nodes_size(nto); k++ ) {
	if( fabs(w[0][k][j]) > wmax )
	  wmax = fabs(w[0][k][j]);
      }
    }
  }

  for( i=0; i<m; i++ ) {
    w = mint_network_weights( net, i );
    ifrom = mint_weights_get_from(w);
    nfrom = mint_network_nodes( net, ifrom );
    ito = mint_weights_get_to(w);
    nto = mint_network_nodes( net, mint_weights_get_to(w) );
    for( j=0; j<mint_nodes_size(nfrom); j++ ) {
      for( k=0; k<mint_nodes_size(nto); k++ ) {
	val = w[0][k][j]/wmax;
	fprintf( f, "n%d_%d -> n%d_%d [", ifrom, j, ito, k );
	if( val<0 ) {
	  fprintf( f, "color=\"0.66 %.2f 1\"", -val );
	} else {
	  fprintf( f, "color=\"0 %.2f 1\"", val );
	}
	fprintf( f, " label=\"%.2g\"]\n", w[0][k][j] );
      }
    } 
  }
  
  fprintf( f, "}\n" );
}

void mint_network_add_nodes( struct mint_network *net, const mint_nodes n ) {
  net->n = realloc( net->n, (net->groups+1)*sizeof(mint_nodes) );
  net->n[ net->groups ] = mint_nodes_dup( n );
  net->groups++;
}

void mint_network_add_weights( struct mint_network *net, const mint_weights w ) {
  net->w = realloc( net->w, (net->matrices+1)*sizeof(mint_weights) );
  net->w[ net->matrices ] = mint_weights_dup( w );
  net->matrices++;	
}

struct mint_spread *mint_network_get_spread( struct mint_network *net ) {
  return net->spread;
}

void mint_network_set_spread( struct mint_network *net,
			      struct mint_spread *s ) {
  mint_spread_del( net->spread );
  net->spread = mint_spread_dup( s );
}

struct mint_ops *mint_network_get_ops( struct mint_network *net ) {
  return net->ops;
}

void mint_network_replace_weights( struct mint_network *net, 
				   int i, mint_weights w ) {
  mint_check( i>=0 && i<net->matrices, 
	      "weights index %d out of range 0-%d", i, net->matrices - 1 );
  mint_weights_del( net->w[i] );
  net->w[i] = mint_weights_dup( w );
}


/* some network ops defined here for efficiency / ease of coding */

void mint_network_asynchronous( struct mint_network *net, float *p ) {
  int i, j, k, steps, from;
  mint_nodes n;

  if( !net->size ) {
    for( i=0; i<net->groups; i++ )
      net->size += mint_nodes_size( net->n[i] );
  }

  /* this sets the number of updates to the number of nodes the first
     time the op executes, and unless the user has specified a number
     of updates already (this hinges on the fact that the default p[0]
     value is 0). */
  if( p[0] )
    steps = p[0];
  else
    steps = net->size;

  while( steps-- ) {
    /* pick node group at random, weighing by size and only taking
       into account nodes with an update op */
    i = j = 0; 
    k = mint_random_int( 0, net->size );
    while( j<=k ) {
      n = mint_network_nodes( net, i );
      if( mint_ops_count(mint_nodes_get_ops(n),mint_op_nodes_update)>0 ) 
	j += mint_nodes_size( net->n[i] );
      i++;
    }
    i--;

    /* pick node at random from group i */
    j = mint_random_int( 0, mint_nodes_size(net->n[i]) );

    /* calculate input to this node */
    for( k=0; k<net->matrices; k++ ) {
      if( mint_weights_get_to(net->w[k]) == i ) { /* leads to this node */
	from = mint_weights_get_from(net->w[k]);
	mint_weights_operate( net->w[k], net->n[from], net->n[i], j, j+1 );
      }
    }
    mint_nodes_update( net->n[i], j, j+1 );

    /* update weights that have been involved */
    for( k=0; k<net->matrices; k++ ) {
      if( mint_weights_get_to(net->w[k]) == i ) { /* leads to this node */
	from = mint_weights_get_from(net->w[k]);
	mint_weights_update( net->w[k], net->n[from], net->n[i], j, j+1 );
      }
    }
  }
}

void mint_network_spread( struct mint_network *net ) {
  int i, j, k, from, to, len, target;

  if( !net->spread ) 
    return;

  /* reset the targets of weight matrices */
  for( i=0; i<net->matrices; i++ ) {
    to =  mint_weights_get_to( net->w[i] );
    target = mint_weights_get_target( net->w[i] );
    mint_nodes_set( net->n[to], target, 0. );
  }

  len = mint_spread_len( net->spread );

  for( i=0; i<len; i++ ) {

    j = mint_spread_get_nodes( net->spread, i );
    k = mint_spread_get_weights( net->spread, i );
    to = from = -1; /* avoids "unitialized variable" warning */

    /* matrix-vector multiplication */
    if( k > -1 ) {
      to = mint_weights_get_to( net->w[k] );
      from = mint_weights_get_from( net->w[k] );
      mint_weights_operate( net->w[k], net->n[from], net->n[to],
			    0, mint_weights_rows(net->w[k]) );
    }

    /* node update */
    if( j > -1 )
      mint_nodes_update( net->n[j], 0, mint_nodes_size(net->n[j]) );

    /* weight update */
    if( k > -1 )
      mint_weights_update( net->w[k], net->n[from], net->n[to], 
			   0, mint_weights_rows(net->w[k]) );

  }
}

void mint_network_operate( struct mint_network *net ) {
  int i, n;
  struct mint_op *op;
  n = mint_ops_size( net->ops );
  for( i=0; i<n; i++ ) {
    op = mint_ops_get( net->ops, i );
    if( mint_op_type( op ) == mint_op_network_operate )
      mint_op_run( op, net );
  }
  mint_network_spread( net );
}
