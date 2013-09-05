#include "mint.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

/* competitive.c: simulates self-organization of receptive fields
   through competitive learning. the network has input units (I) and
   hidden units (H). H units inhibit each other via fixed
   weights. they receive input from I units through weights that are
   initially random and are then updated by hebbian learning. the
   hebbian rule is of the form:

   w += a * (I-b) * (H-c) - d*w

   were I and H are two input and hidden units and w is the weight
   that joins them. the values used for parameters a, b, c, and d are
   listed, in this orderd, in the "update hebbian" line of
   architecture file competitive.arc.

   the activity of I units is set by the stimulus() function. the
   activity of H units is determined by the inputs they receive from I
   units and by the inhibition they receive from other H units. H
   units have "integrator" dynamics, i.e., their activity is a
   function of present and past according to:

   H(t) += ( Y - L*H(t-1) ) / T

   where Y is the total input received, L determines the memory and T
   the timescale over which input varies. the values used for these
   parameters are in the architecture file.

   the stimuli are presented in random order, for 50 time steps each,
   for a total of 64 stimulus presentations.

   NOTES: the network tries to self-organize without explicit guidance
   from the environment (there is no reinforcement or error
   signal). both node dyanmics and learning (weight changes) go on
   continuously, i.e., at each time step.
 */


/* sets a peaked activity profile over the 8 input units, something
   like: 0 0 0.13 0.6 1 0.6 0.13 0 */
void stimulus( mint_nodes in, /* input nodes */
	       float x ) {    /* position of maximum activity */
  int i;
  for( i=0; i<mint_nodes_size(in); i++ )
    in[1][i] = exp( -(i-x)*(i-x)/2 );
}

int main( void ) {
  struct mint_image *image; /* used to image the weights */
  FILE *file;               /* to read and write from files */ 
  struct mint_network *net; /* the network */
  mint_nodes 
    in,                     /* pointer to input layer */
    hid;                    /* pointer to hidden layer */
  int t1, t2, size, x;   /* various counters and helpers */   

  mint_random_seed( time(0) );

  /* load network from file */
  file = fopen( "competitive.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  /* useful shortcuts */
  in = mint_network_nodes(net,0);
  hid = mint_network_nodes(net,1);
  size = mint_nodes_size(in);
  
  file = fopen( "output/time.dat", "w" );
  for( t1=0; t1<64; t1++ ) { /* loop over 64 stimulus presentations */

    /* choose a stimulus at random */ 
    stimulus( in, mint_random_int(0,size) );

    /* each stimulus is presented for 50 time steps to allow the
       recurrent network to settle */
    for( t2=0; t2<50; t2++ ) {

      mint_network_operate( net );

      /* save hidden layer output to file */ 
      fprintf( file, "%d ", t1*50+t2 );
      mint_nodes_save_var( hid, 1, file );
      fprintf( file, "\n" );
    }
  }
  fclose( file );

  /* save network architecture in dot format */ 
  file = fopen( "output/competitive.dot", "w" );
  mint_network_graph( net, file );
  fclose( file );

  /* save trained network for analysis */
  file = fopen( "output/end.arc", "w" );
  mint_network_save( net, file );
  fclose( file );

  /* create an image of the final feed-forward weights. we scale the
     image up 20 times because the original is only 8x8 pixels given
     the weight matrix is 8x8. */ 
  image = mint_image_weights( mint_network_weights(net,0), 0, 0 );
  mint_image_scale( image, 20 );
  mint_image_save( image, "output/weights-end.jpg", FIF_JPEG );

  /* make tuning curves */
  mint_network_freeze( net, -1, 1 ); /* prevents weight updates */
  file = fopen( "output/tuning.dat", "w" );
  for( x=0; x<mint_nodes_size(in); x++ ) { /* loop over all stimuli */

    stimulus( in, x ); /* set a specific stimulus */

    /* present stimulus for 50 time steps */
    for( t2=0; t2<50; t2++ )
      mint_network_operate( net );

    /* save output of each hidden node */
    mint_nodes_save_var( hid, 1, file );
    fprintf( file, "\n" );
  }
  fclose( file );

  mint_network_del( net ); /* release memory */

  return 0;
}
