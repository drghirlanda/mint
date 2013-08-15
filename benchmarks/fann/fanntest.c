#include <stdio.h>
#include "floatfann.h"
#include "fanntest.h"

int main( void ) {
  fann_type *calc_out;
  fann_type input[ N ];
  int i;

  struct fann *ann = fann_create_standard( 3, N, N, 1 );
  fann_set_activation_function_hidden( ann, ACT );
  fann_set_activation_function_output( ann, FANN_LINEAR );

  for( i=0; i<1000; i++ )
    calc_out = fann_run(ann, input);
  
  fann_destroy(ann);
  return 0;
}
