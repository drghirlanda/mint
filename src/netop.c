/* needed to access nanosleep() in gcc */
#define _POSIX_C_SOURCE 199309L

#include "netop.h"

#include <time.h>
#include <math.h>

void mint_network_clocked( struct mint_network *net, float *p ) {
  float towait, last, elapsed, tosleep;
  int warn;
  struct timespec t, r;

  towait = 1. / p[0];
  warn = p[1];
  last = p[2];

  elapsed = ( (float) clock() - last ) / CLOCKS_PER_SEC;

  tosleep = towait - elapsed;

  if( tosleep>0 ) {
    t.tv_sec = floor(tosleep);
    t.tv_nsec = 1e9 * (tosleep - t.tv_sec);
    nanosleep( &t, &r );
  } else if( tosleep<0 && warn==1 ) {
    fprintf( stderr, "mint: cannot update network at desired speed\n" );
    fprintf( stderr, "mint: you asked for %.2f Hz, you got %.2f Hz\n", 
	     p[0], 1/elapsed );
  }
  p[2] = (float) clock();
}
