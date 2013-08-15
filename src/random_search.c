#include "network.h"
#include "random_search.h"

float mint_random_search( struct mint_network *net1, 
		     float (*perf)( struct mint_network * ), 
		     void (*mut)( struct mint_network * ), 
		     float target, long max ) {
  float p1, p2;
  long count = 0;
  struct mint_network *net2 = mint_network_dup( net1 );
  p1 = perf( net1 );
  while( p1<target && count++<max ) {
    mut( net2 );
    p2 = perf( net2 );
    if( p2 > p1 ) {
      p1 = p2;
      mint_network_cpy( net1, net2 );
    } else {
      mint_network_cpy( net2, net1 );
    }
  }
  mint_network_del( net2 );
  return p1;
}
