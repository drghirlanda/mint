#include "network.h"
#include "spread.h"
#include "update.h"
#include "random.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct mint_network {
  int groups;                 /* number of node groups */
  int matrices;               /* number of weight matrices */
  int size;                   /* total number of nodes */ 
  mint_nodes *n;              /* array of node arrays */
  mint_weights *w;            /* array of weight matrices */
  struct mint_spread *spread; /* spread scheme (see spread.h) */
};

static struct mint_network *mint_network_alloc( int groups, 
						int matrices ) {
  int i;
  struct mint_network *net;
  net = malloc( sizeof(struct mint_network) );
  net->n = malloc( groups * sizeof(mint_nodes) );
  net->w = malloc( matrices * sizeof(mint_weights) );
  net->groups = groups;
  net->matrices = matrices;
  net->spread = 0;
  for( i=0; i<groups; i++ ) net->n[i] = 0;
  for( i=0; i<matrices; i++ ) net->w[i] = 0;
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
  return net2;
}

void mint_network_del( struct mint_network *net )
{
  int i;
  mint_check( net!=0, "attempt to delete null network" );
  mint_spread_del( net->spread );
  for( i=0; i<net->groups; i++ ) mint_nodes_del( net->n[i] );
  free( net->n );
  for( i=0; i<net->matrices; i++ ) mint_weights_del( net->w[i] );  
  free( net->w );
  
  free( net );
}

void mint_network_cpy( struct mint_network *net1, 
		       const struct mint_network *net2 ) {
  int i;
  mint_check( net1, "destination network is null" );
  mint_check( net2, "source network is null" );
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
}

void mint_network_acalc( struct mint_network *net ) {
  int i, j, k, steps, from;
  mint_nodes n;

  if( !net->size ) {
    for( i=0; i<net->groups; i++ )
      net->size += mint_nodes_size( net->n[i] );
  }

  steps = -mint_spread_len( net->spread );
  while( steps-- ) {
    /* pick node group at random, weigting by size */
    i = j = 0; 
    k = mint_random_int( 0, net->size );
    while( j<=k ) {
      n = mint_network_nodes( net, i );
      if( mint_nodes_get_update( n ) )
	j += mint_nodes_size( net->n[i] );
      i++;
    }

    /* pick node at random from group i */
    j = mint_random_int( 0, mint_nodes_size(net->n[i]) );

    /* calculate input to this node */
    for( k=0; k<net->matrices; k++ ) {
      if( mint_weights_get_to(net->w[k]) == i ) { /* leads to this node */
	from = mint_weights_get_from(net->w[k]);
	mint_weights_multrow( net->w[k], net->n[from], net->n[i], j );
      }
    }
    mint_update_napply( net->n[i], j, j+1 ); /* update node j only */
  }
}

void mint_network_scalc( struct mint_network *net ) {
  int i, k, from, to;
  mint_weights w;
  struct mint_update *u;
  for( i=0; i<net->groups; i++ ) {
    u = mint_nodes_get_update( net->n[i] );
    if( u )
      mint_nodes_set( net->n[i], 0, 0. );
  }
  for( i=0; i<mint_spread_len(net->spread); i++ ) {
    k = mint_spread_get_weights( net->spread, i );
    if( k > -1 ) {
      w = net->w[k];
      to = mint_weights_get_to( net->w[k] );
      from = mint_weights_get_from( net->w[k] );
      mint_weights_mult( w, net->n[from], net->n[to] );
    }
    k = mint_spread_get_nodes( net->spread, i );
    if( k > -1 )
      mint_update_napply( net->n[k], 0, mint_nodes_size(net->n[k]) );
  }
}

void mint_network_nupdate( struct mint_network *net ) {
  if( mint_spread_len(net->spread) < 0 ) mint_network_acalc( net ); 
  else mint_network_scalc( net );
}

void mint_network_wupdate( struct mint_network *net ) {
  int k, from, to;
  for( k=0; k<net->matrices; k++ ) {
    to = mint_weights_get_to( net->w[ k ] );
    from = mint_weights_get_from( net->w[ k ] );
    mint_update_wapply( net->w[k], net->n[from], net->n[to] );
  }
}

void mint_network_save( const struct mint_network *net, FILE *dest ) {
  int i;
  fprintf( dest, "network %d %d\n", net->groups, net->matrices );
  for( i=0; i<net->groups; i++ ) 
    mint_nodes_save( net->n[i], dest );
  for( i=0; i<net->matrices; i++ )
    mint_weights_save( net->w[i], dest );
  mint_spread_save( net->spread, dest );
}

struct mint_network *mint_network_load( FILE *file ) {
  char c;
  int i, groups, matrices;
  struct mint_network *net;

  mint_skip_space( file );

  i = fscanf( file, "network %d %d", &groups, &matrices );
  mint_check( i==2, "cannot read network geometry" );
  net = mint_network_alloc( groups, matrices );

  for( i=0; i<net->groups; i++ )
    net->n[i] = mint_nodes_load( file );
  for( i=0; i<net->matrices; i++ )
    net->w[i] = mint_weights_load( file );

  c = mint_skip_space( file );
  if( c == 's' ) 
    net->spread = mint_spread_load( file );
  else
    mint_spread_synchronous( net );

  return net;
}

mint_nodes mint_network_nodes( struct mint_network *n, int i ) {
  mint_check( i>=0 && i<n->groups, "index out of range" );
  return n->n[i];
}

mint_weights mint_network_weights( struct mint_network *n, int i ) {
  mint_check( i>=0 && i<n->matrices, "index out of range" );
  return n->w[i];
}

int mint_network_groups( const struct mint_network *n ) {
  return n->groups;
}

int mint_network_matrices( const struct mint_network *n ) {
  return n->matrices;
}

struct mint_spread *mint_network_get_spread( struct mint_network *net ) {
  return net->spread;
}

void mint_network_set_spread( struct mint_network *net,
			      struct mint_spread *s ) {
  mint_spread_del( net->spread );
  net->spread = mint_spread_dup( s );
}

struct mint_network *mint_network_join( const struct mint_network *net1,
					int n1,
					const struct mint_network *net2,
					int n2, 
					int s, float x, int step ) {
  int i, j, g1, g2, m1, m2, r, c, len1, len2;
  struct mint_network *net;
  struct mint_spread *spread;

  g1 = net1->groups; 
  g2 = net2->groups; 
  m1 = net1->matrices; 
  m2 = net2->matrices; 
  
  mint_check( n1>=0 && n1<g1, "2nd arg out of range" ); 
  mint_check( n2>=0 && n2<g2, "4th arg out of range" ); 

  net = mint_network_alloc( g1+g2, m1+m2+1 );

  /* 1st: copy nodes and matrices with new indices */
  for( i=0; i<g1; i++ ) net->n[i] = mint_nodes_dup( net1->n[i] );
  for( i=0; i<g2; i++ ) net->n[ g1+i ] = mint_nodes_dup( net2->n[i] );

  for( i=0; i<m1; i++ ) net->w[i] = mint_weights_dup( net1->w[i] );
  for( i=0; i<m2; i++ ) {
    net->w[ m1+i ] = mint_weights_dup( net2->w[i] );
    mint_weights_set_from( net->w[ m1+i ], 
			   g1 + mint_weights_get_from( net2->w[i] ) );
    mint_weights_set_to( net->w[ m1+i ], 
			 g1 + mint_weights_get_to( net2->w[i] ) );
  }

  /* 2nd: create and init the new matrix */
  r = mint_nodes_size( net2->n[n2] );
  c = mint_nodes_size( net1->n[n1] );
  net->w[m1+m2] = mint_weights_new( r, c, s );
  mint_weights_set_from( net->w[m1+m2], n1 );
  mint_weights_set_to( net->w[m1+m2], g1+n2 );
  for( i=0; i<r; i++ ) {
    for( j=0; j<c; j++ ) 
      net->w[m1+m2][0][i][j] = x;
  }

  /* 3rd: create new spread scheme */
  len1 = mint_spread_len( net1->spread );
  len2 = mint_spread_len( net2->spread );
  spread = mint_spread_new(  len1 + len2 + 1 );
  for( i=0; i<len1; i++ ) { 
    j = mint_spread_get_weights( net1->spread, i );
    mint_spread_set_weights( spread, i, j );
    j = mint_spread_get_nodes( net1->spread, i );
    mint_spread_set_nodes( spread, i, j );
  }
  mint_spread_set_weights( spread, len1, m1+m2 );
  mint_spread_set_nodes( spread, len1, g1+n2 );
  for( i=0; i<len2; i++ ) { 
    j = mint_spread_get_weights( net2->spread, i );
    mint_spread_set_weights( spread, i, j+len1+1 );
    j = mint_spread_get_nodes( net2->spread, i );
    mint_spread_set_nodes( spread, i, j+len1+1 );
  }
  mint_network_set_spread( net, spread );
  mint_spread_del( spread );

  return net;
}

void mint_network_graph( const struct mint_network *net, FILE *f ) {
  int g, m, i;
  mint_nodes n;
  mint_weights w;
  /* struct mint_update *u; */
 
  fprintf( f, "digraph network {\n" );
  g = mint_network_groups( net );
  for( i=0; i<g; i++ ) {
    n = mint_network_nodes( (struct mint_network *)net, i );
    fprintf( f, "n%d [label=\"\\N %d %d", i, mint_nodes_size(n),
	     mint_nodes_states(n) );
    /*    u = mint_nodes_get_update(n);
    if( u )
    fprintf( f, "\\n%s", mint_update_name(u) ); */
    fprintf( f, "\"]\n" );
  }
  m = mint_network_matrices( net );
  for( i=0; i<m; i++ ) {
    w = mint_network_weights( (struct mint_network *)net, i );
    fprintf( f, "n%d -> n%d\n", mint_weights_get_from(w), 
	     mint_weights_get_to(w) );
  }
  fprintf( f, "}\n" );
}
