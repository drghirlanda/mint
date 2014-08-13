#include "mint.h"
#include <stdio.h>
#include <stdlib.h>

int main( void ) {
  FILE *f; 
  mint_nodes n;
  f = fopen( "nodes.arc", "r" );
  n = mint_nodes_load( f );        /* load net from file */
  fclose( f );
  mint_nodes_save( n, stdout );    /* display the network */
  mint_nodes_del( n );
  return EXIT_SUCCESS;
}
