#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

/* these variables define some characteristics of stimuli */

float background = 0.1;  /* background intensity */
int Aposition = 0.25;    /* position of stimulus A */
float Aintensity = 0.5;  /* intensity of A above the background */
float Awidth = 0.2;      /* width of the activity peak in A */
int Bposition = 0.75;    /* same stuff for B... */
float Bintensity = 0.5;  
float Bwidth = 0.2;      

float T = 100; /* duration of each training phase */

/* set background input - uniform activity */
void set_input_X( mint_nodes n ) {
  mint_nodes_set( n, 1, background );
}

/* set A input - we use a Gaussian function to create the activity
   peak over the receptor array, with position, height, and width of
   the peak determined by the stimulus parameters defined above */
void set_input_A( mint_nodes n ) {
  int i, size;
  float x;
  size = mint_nodes_size( n );
  for( i=0; i<size; i++ ) {
    x = ( Aposition*size - i ) / ( Awidth*size );
    n[1][i] = background + Aintensity * exp( -x*x );
  }
}

/* here you need to write functions set_input_AB and set_input_B to
   create the AB and B inputs */


int main( void ) {
  FILE *file;
  struct mint_network *net;
  mint_nodes in, out; /* shorthands for input and output nodes */
  int t;    /* loop counter for learning trials */ 
  char *cs; /* conditioned stimulus name (see below) */

  /* init random number generator */
  mint_random_seed( time(0) );

  /* create network */
  file = fopen( "feature-reversal.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  /* init shortcuts for input and output nodes */
  in = mint_network_nodes( net, mint_network_nodes_find( net, "in" ) );
  out = mint_network_nodes( net, mint_network_nodes_find( net, "out" ) );
  
  /* this is the data file */
  file = fopen( "feature-reversal.dat", "w" );

  /* this is the first training phase: A+ X0 */

  for( t=0; t<T; t++ ) { /* we run for T trials, see above */

    /* now we decide which stimulus to present by flipping a coin: */

    if( mint_random() < 0.5 ) {     /* we are presenting A: */

      set_input_A( in );           /* set input activity to A */
      mint_nodes_set( out, 2, 1 ); /* set value to 1 */
      cs = "A";                    /* remember that we have seen A */

    } else { /* we are presenting X: the steps are the same */

      set_input_X( in );
      mint_nodes_set( out, 2, -1 );
      cs = "X";
    }

    /* process stimulus - this automatically uses the delta rule to
       update weights according to stimulus, output, and value */
    mint_network_operate( net );

    /* save info to file */
    fprintf( file, "%d %s %f\n", t, cs, out[1][0] );
  }

  /* we' re done: close data file */
  fclose( file );

  /* save trained network to file in case we want to look at it */
  file = fopen( "feature-reversal-trained.arc", "w" );
  mint_network_save( net, file );
  fclose( file );

  /* cleanup */
  mint_network_del( net );

  return 0;
}
