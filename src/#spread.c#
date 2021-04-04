#include "network.h"
#include "spread.h"
#include "string.h"
#include "weights.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

struct mint_spread {
  int len;
  int *n;
  int *w;
};

struct mint_spread *mint_spread_new( int len ) {
  struct mint_spread *s;
  s = malloc( sizeof(struct mint_spread) );
  s->len = len;
  s->w = malloc( len * sizeof(int) );
  s->n = malloc( len * sizeof(int) );
  return s;
}

void mint_spread_del( struct mint_spread *s ) {
  if( s==0 ) return;
  free( s->w );
  free( s->n );
  free( s );
}

/* the algorithm to determine the spread scheme for a feed forward
   network is: while there are groups left to update: 1) find groups
   that either do not receive any input or receive input only from
   groups that have have been updated; 2) schedule all nodes-weights
   multiplications for said groups; 3) schedule node updates of the
   same groups, and mark them as done.  */
void mint_network_init_feedforward( struct mint_network *net,
				    float *p ) {
  int i, j, g, m, ndone, ndone_old, step, from, to, pass;
  int *done; 
  struct mint_spread *s;
  mint_weights w;

  MINT_UNUSED( p );

  g = mint_network_groups(net);
  m = mint_network_matrices(net);
  s = mint_spread_new( m+g );

  done = malloc( g*sizeof(int) );
  for( i=0; i<g; i++ ) done[i] = 0;
  ndone = 0;
  ndone_old = 0;
  
  step = pass = 0;
  while( ndone<g ) {
    /* if no progress (and no bugs :), topology is not right: */
    mint_check( pass==0 || /* first time */
		ndone > ndone_old, 
		"network is not feedforward");
    pass++;
    for( i=0; i<g; i++ ) {
      if( !done[i] ) {
	/* loop over weight matrices: group is ready for update only
	   if it only receives inputs from groups already updated. */
	for( j=0; j<m; j++ ) {
	  w = mint_network_get_weights(net, j);
	  to = mint_weights_get_to(w);
	  from = mint_weights_get_from(w);
	  mint_check( to != from, "matrix %d is self-recurrent!", j );
	  if( to==i && !done[from] ) /* if input from nodes not done */
	    break;                   
	}
	if( j==m ) { /* loop completed: no unresolved inputs */
	  done[i] = 1;
	  ndone_old = ndone++;
	  /* schedule matrix mults leading to this group */
	  for( j=0; j<m; j++ ) {
	    w = mint_network_get_weights(net, j);
	    if( mint_weights_get_to(w) == i ) {
	      s->w[step] = j;
	      s->n[step] = -1;
	      step++;
	    }
	  }
	  /* schedule node update */
	  s->w[step] = -1;
	  s->n[step] = i;
	  step++;
	}
      }
    }
  }
  mint_network_set_spread( net, s );
  mint_spread_del( s );
  free( done );
}

void mint_network_init_synchronous( struct mint_network *net,
				    float *p ) {
  int i, g, m;
  struct mint_spread *s;

  MINT_UNUSED( p );

  g = mint_network_groups( net );
  m = mint_network_matrices( net );
  s = mint_spread_new( m+g );
  for( i=0; i<m; i++ ) {
    s->w[i] = i;
    s->n[i] = -1;
  }
  for( ; i<m+g; i++ ) {
    s->w[i] = -1;
    s->n[i] = i-m;
  }
  mint_network_set_spread( net, s );
  mint_spread_del( s );
}


struct mint_spread *mint_spread_dup( const struct mint_spread *s1 ) {
  int i;
  struct mint_spread *s2;
  if( s1==0 ) return 0;
  s2 = mint_spread_new( s1->len );
  for( i=0; i<s2->len; i++ ) {
    s2->n[i] = s1->n[i];
    s2->w[i] = s1->w[i];
  }
  return s2;
}

struct mint_spread *mint_spread_load( FILE *f, 
				      struct mint_network *net ) {
  int ni, wi;
  struct mint_spread *spread;
  mint_string name;

  if( !mint_next_string(f, "spread", 6 ) )
    return 0;

  spread = mint_spread_new( 0 );

  for( ;; ) {
    name = mint_string_load( f );
    if( !name || mint_keyword( name ) ) 
      break;
    ni = mint_network_nodes_index( net, name );
    if( ni >= 0 ) {
      wi = -1;
    } else {
      wi = mint_network_weights_index( net, name );
      if( wi >= 0 ) { 
	ni = -1;
      } else {
	mint_check( 0, "no such group or matrix: %s", name );
      }
    }
    spread->len++;
    spread->n = realloc( spread->n, spread->len * sizeof(int) );
    spread->w = realloc( spread->w, spread->len * sizeof(int) );
    spread->n[ spread->len - 1 ] = ni;
    spread->w[ spread->len - 1 ] = wi;
  }

  if( spread->len )
    return spread;
  else
    return 0;
}

void mint_spread_save( struct mint_spread *s, FILE *f,
		       struct mint_network *net ) {
  int i;
  mint_weights w;
  mint_nodes n;

  if( !s ) 
    return;

  fprintf( f, "spread\n" );

  for( i=0; i<s->len; i++ ) {
    if( s->w[i] > -1 ) {
      w = mint_network_get_weights( net, s->w[i] );
      fprintf( f, "  %s\n", mint_weights_get_name( w ) );
    }
    if( s->n[i] > -1 ) {
      n = mint_network_get_nodes( net, s->n[i] );
      fprintf( f, "  %s\n", mint_nodes_get_name( n ) );
    }
  }
}

int mint_spread_len( struct mint_spread *s ) {
  if( s==0 ) return 0;
  return s->len;
}

int mint_spread_get_nodes( struct mint_spread *s, int i ) {
  mint_check( s!=0, "null 1st arg" );
  mint_check( i>=0 && i<s->len, "2nd arg out of range" );
  return s->n[i];
}

void mint_spread_set_nodes( struct mint_spread *s, int i, int k ) {
  mint_check( s!=0, "null 1st arg" );
  mint_check( i>=0 && i<s->len, "2nd arg out of range" );
  s->n[i] = k;
}

int mint_spread_get_weights( struct mint_spread *s, int i ) {
  mint_check( s!=0, "null 1st arg" );
  mint_check( i>=0 && i<s->len, "2nd arg out of range" );
  return s->w[i];
}

void mint_spread_set_weights( struct mint_spread *s, int i, int k ) {
  mint_check( s!=0, "null 1st arg" );
  mint_check( i>=0 && i<s->len, "2nd arg out of range" );
  s->w[i] = k;
}

void mint_spread_add_step( struct mint_network *net, int pos, int wi, int ni ) {
  struct mint_spread *s;
  
  s = mint_network_get_spread( net );
  s->w = realloc( s->w, (s->len+1)*sizeof(int) );
  s->n = realloc( s->n, (s->len+1)*sizeof(int) );
  memmove( s->w+pos, s->w+pos+1, (s->len-pos)*sizeof(int) );
  memmove( s->n+pos, s->n+pos+1, (s->len-pos)*sizeof(int) );
  s->w[pos] = wi;
  s->n[pos] = ni;
  s->len++;
}
