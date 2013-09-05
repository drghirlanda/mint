#include "mint.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* this is a dummy function used below to show how input could be
   received from some system outside the network, e.g., robot
   hardware, a simulated environment, a simulated stimulus, and so
   on. here it just sets constant input to the network */
void receive_input( struct mint_network *net ) {
  mint_nodes n;
  n = mint_network_nodes(net, 0);
  n[1][0] = n[1][1] = .5;
  n[1][2] = n[1][3] = n[1][4] = 0.25;
}

/* likewise, this function is used below in the place where one would
   need to send network output to some system like hardware,
   simulation environment, and so on. here it does nothing */
void send_output( struct mint_network *net ) {
}

int main( void ) {
  FILE *file;
  struct mint_network *net;
  mint_nodes n;
  int t, i, size;

  /* init random number generator */
  mint_random_seed( time(0) );

  /* read network architecture from file */
  file = fopen( "recnet.arc", "r" );
  net = mint_network_load( file );
  fclose( file );

  /* save network to file. it seems nothing has happened since we
     loaded the network, but actually the architecture file contains
     instructions to initialize network weights at random */
  file = fopen("output/recnetDetail.arc", "w");
  mint_network_save( net, file );
  fclose( file );
 
  /* save a graph representing the network in graphviz format, see
     www.graphviz.org */
  file = fopen("output/recnetArc.dot", "w");
  mint_network_graph( net, file );
  fclose( file );

  /* we get a pointer to this node group, and its size, because we
     want to save its activity to file later */
  n = mint_network_nodes( net, 1 );
  size = mint_nodes_size(n);

  file = fopen( "output/recnet.dat", "w" );

  for( t=0; t<200; t++ ) {
    receive_input( net ); 
    mint_network_nupdate( net ); /* this function updates the network */
    send_output( net ); 

    fprintf( file, "%d ", t );
    for( i=0; i<size; i++ )
      fprintf( file, "%f ", n[1][i] );
    fprintf( file, "\n" );
  }

  fclose( file );
  mint_network_del( net );
  return 0;
}
