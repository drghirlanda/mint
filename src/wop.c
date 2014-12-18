/* #include "op.h" */
/* #include "wop.h" */
/* #include "random.h" */
/* #include "utils.h" */

/* #include <math.h> */


/* void mint_weights_init_rows( mint_weights w, int rmin, int rmax,  */
/* 			     float *p ) { */
/*   MINT_UNUSED( w ); */
/*   MINT_UNUSED( rmin ); */
/*   MINT_UNUSED( rmax ); */
/*   mint_check( p[0] != -1, "rows argument missing (parameter 0)" ); */
/*   mint_check( p[0] > 0, "rows argument negative" ); */
/* } */

/* void mint_weights_init_cols( mint_weights w, int rmin, int rmax,  */
/* 			     float *p ) { */
/*   MINT_UNUSED( w ); */
/*   MINT_UNUSED( rmin ); */
/*   MINT_UNUSED( rmax ); */
/*   mint_check( p[0] != -1, "cols argument missing (parameter 0)" ); */
/*   mint_check( p[0] > 0, "cols argument negative" ); */
/* } */

/* void mint_weights_init_states( mint_weights w, int rmin, int rmax,  */
/* 			       float *p ) { */
/*   MINT_UNUSED( w ); */
/*   MINT_UNUSED( rmin ); */
/*   MINT_UNUSED( rmax ); */
/*   mint_check( p[0] != -1, "states argument missing (parameter 0)" ); */
/*   mint_check( p[0] >= 0, "states argument negative" ); */
/* } */

/* void mint_weights_init_sparse( mint_weights w, int rmin, int rmax, */
/* 			       float *p ) { */
/*   MINT_UNUSED( w ); */
/*   MINT_UNUSED( rmin ); */
/*   MINT_UNUSED( rmax ); */
/*   MINT_UNUSED( p ); */
/* }; */

/* void mint_weights_mult( mint_weights w, mint_nodes from, mint_nodes to, */
/* 			int rmin, int rmax, float *p ) { */
/*   int i, j, cols, target, jmax; */
/*   int *colind;  */
/*   MINT_UNUSED( p ); */
/*   target = mint_weights_get_target(w); */
/*   cols = mint_weights_cols(w); */
/*   if( mint_weights_is_sparse( w ) ) { */
/*     for( i=rmin; i<rmax; i++ ) { */
/*       colind = mint_weights_colind( w, i ); */
/*       jmax = mint_weights_rowlen( w, i ); */
/*       for( j=0; j<jmax; j++ ) */
/* 	to[ target ][ i ] += w[0][i][j] * from[ 1 ] [ colind[j] ]; */
/*     } */
/*   } else { */
/*     for( i=rmin; i<rmax; i++ ) {  */
/*       for( j=0; j<cols; j++ ) { */
/* 	to[target][i] += w[0][i][j] * from[1][j];   */
/*       } */
/*     } */
/*   } */
/* } */

/* void mint_weights_hebbian( mint_weights w, mint_nodes pre,  */
/* 			   mint_nodes post, int rmin, int rmax, float *p ) { */
/*   MINT_WEIGHTS_LOOP_INIT; */

/*   MINT_WEIGHTS_LOOP( w,	 */
/* 		     w[0][i][k] += p[0] * ( ( pre[1][j] - p[1] ) *  */
/* 					    ( post[1][i] - p[2] ) ) */
/* 		     - p[3] * w[0][i][k] ); */
/* } */


/* #include <float.h> */

/* /\* the algorithm for numerical differentiation is derived from */
/*    en.wikipedia.org/wiki/Numerical_differentiation, merging the second */
/*    two-point method with the 'practical considerations' that */
/*    follow. *\/ */

/* static  */
/* float numerical_derivative( mint_nodes n, int i ) { */
/*   int j, s; */
/*   float *old; */
/*   float x1, y1, x2, y2, h; */
/*   volatile float dx; */

/*   h = sqrt( FLT_MIN ) * n[0][i]; /\* finite difference step *\/ */

/*   /\* record neuron state to be able to restore it later *\/ */
/*   s = 2 + mint_nodes_states( n ); */
/*   old = malloc( s * sizeof(float) ); */
/*   for( j=0; j<s; i++ ) */
/*     old[j] = n[j][i]; */

/*   x1 = old[0] - h; */
/*   x2 = old[0] + h; */
/*   dx = x2 - x1; */

/*   n[0][i] = x1; */
/*   mint_nodes_update( n, i, i ); */
/*   y1 = n[1][i]; */

/*   n[0][i] = x2; */
/*   mint_nodes_update( n, i, i ); */
/*   y2 = n[1][i]; */
  
/*   /\* restore neuron state *\/ */
/*   for( j=0; j<s; j++ ) */
/*     n[j][i] = old[j]; */

/*   free( old ); */

/*   return ( y2 - y1 ) / dx; */
/* } */

/* void mint_weights_delta( mint_weights w, mint_nodes pre,  */
/* 			 mint_nodes post, int rmin, int rmax, float *p ) { */
/*   MINT_WEIGHTS_LOOP_INIT; */

/*   MINT_UNUSED( j ); */

  float d;

  int desired = (int)p[1];
  float lrate = p[0];

  MINT_WEIGHTS_LOOP( w,
		     d = numerical_derivative( post, i );
		     w[0][i][k] += lrate * d * 
		     ( post[desired][i] - post[1][i] ) * pre[1][k] );
}

void mint_weights_stdp( mint_weights w, mint_nodes pre, 
			mint_nodes post, int rmin, int rmax, float *p ) {
  MINT_WEIGHTS_LOOP_INIT;

  float decay = p[0];
  float plus = p[1];
  float minus = p[2];
  float min = p[3];
  float max = p[4];
  float *x = 0;
  int precount, postcount;
  float pretime, posttime, dt;

  /* figure out where the spike counters are */
  if( mint_nodes_get_property( pre, "counter", 0, x ) )
    precount = *x;
  else
    mint_check( 0, "no counter defined for pre-synaptic nodes" );

  if( mint_nodes_get_property( post, "counter", 0, x ) )
    postcount = *x;
  else
    mint_check( 0, "no counter defined for post-synaptic nodes" );

  MINT_WEIGHTS_LOOP( w,
		     pretime = pre[ precount ][ i ];
		     posttime = post[ postcount ][ j ];
		     dt = posttime - pretime;
		     x = w[0][i] + k;
		     if( dt>=0 )
		       *x += plus / ( 1 + dt * decay );
		     else
		       *x += minus / ( 1 - dt * decay );
		     if( *x < min )
		       *x = min; 
		     else if( *x > max )
		       *x = max );
}

void mint_weights_init_random_sparse( mint_weights w, int rmin, 
				      int rmax, float *p, 
				      float (*rnd)( float, float ) ) {
  float *val;
  int len, cols, i, r;
  int *ind;
  cols = mint_weights_cols(w);
  for( r=rmin; r<rmax; r++ ) {
    len = mint_random_binomial( p[2], cols );
    if( len ) {
      val = malloc( len*sizeof(float) );
      for( i=0; i<len; i++ )
	val[i] = rnd( p[0], p[1] );
      ind = malloc( len*sizeof(ind) );
      mint_random_subset( ind, len, 0, cols, 1 );
      mint_weights_set_row( w, r, len, val, ind, 0 );
      free( val );
      free( ind );
    } else {
      mint_weights_set_row( w, r, 0, 0, 0, 0 );
    }
  }
}

void mint_weights_init_random_dense( mint_weights w, 
				     int rmin, int rmax, float *p, 
				     float (*rnd)( float, float ) ) {
  int i, j, r, c;
  c = mint_weights_cols(w);
  r = mint_weights_rows(w);
  mint_check( rmin>=0 && rmax<=r, "rmin, rmax out of range" );
  for( i=rmin; i<rmax; i++ ) {
    for( j=0; j<c; j++ ) {
      if( mint_random()<p[2] )
	w[0][i][j] = rnd( p[0], p[1] );
      else
	w[0][i][j] = 0;
    }
  }
}

void mint_weights_init_uniform( mint_weights w, int rmin, int rmax, 
			       float *p ) {
  if( mint_weights_is_sparse(w) )
    mint_weights_init_random_sparse( w, rmin, rmax, p, 
				     mint_random_uniform );
  else
    mint_weights_init_random_dense( w, rmin, rmax, p, 
				    mint_random_uniform );    
}

void mint_weights_init_normal( mint_weights w, int rmin, int rmax,
			       float *p ) {
  if( mint_weights_is_sparse(w) )
    mint_weights_init_random_sparse( w, rmin, rmax, p, 
				     mint_random_normal );
  else
    mint_weights_init_random_dense( w, rmin, rmax, p, 
				    mint_random_normal );    
}

void mint_weights_init_diagonal( mint_weights w, int rmin, int rmax,
				 float *p ) {
  int i, r, c;
  r = mint_weights_rows(w);
  c = mint_weights_cols(w);
  mint_check( r==c, "from size != to size" );
  for( i=rmin; i<rmax; i++ )
    mint_weights_set( w, 0, i, i, p[0] );
}

void mint_weights_init_target( mint_weights w, int rmin, int rmax,
				 float *p ) {
  MINT_UNUSED( rmin );
  MINT_UNUSED( rmax );
  mint_weights_set_target( w, p[0] );
}

void mint_weights_init_normalize( mint_weights w, int rmin, int rmax, 
				   float *p ) {
  float sum, total;
  int rows, cols, i, j, rowlen;
  float *val;

  MINT_UNUSED( rmin );
  MINT_UNUSED( rmax );

  rows = mint_weights_rows( w );
  cols = mint_weights_cols( w );
  total = p[0];
  for( i=0; i<rows; i++ ) {
    sum = 0;
    rowlen = mint_weights_rowlen( w, i );
    for( j=0; j<rowlen; j++ )
      sum += w[0][i][j];
    if( sum ) {
      for( j=0; j<rowlen; j++ )
	w[0][i][j] *= total / sum;
    } else {
      if( mint_weights_is_sparse( w ) ) {
	val = malloc( cols * sizeof(float) );
	for( j=0; j<cols; j++ )
	  mint_weights_set_row( w, i, cols, val, 0, 0 );
	free( val );
      } else {
	for( j=0; j<rowlen; j++ )
	  w[0][i][j] = total / rowlen;
      }
    }
  }
}

void mint_weights_lateral( mint_weights w, mint_nodes nfrom, mint_nodes nto, 
			   int rmin, int rmax, float *p ) {
  int i, ri, rj, ci, cj, size, nrows, ncols;
  struct mint_ops *ops;
  float val, val0, dmax, valmax, d, a, b;

  MINT_UNUSED( rmin );
  MINT_UNUSED( rmax );

  if( p[3] ) /* init done */
    return;

  mint_check( nfrom == nto, "from and to nodes must be the same!" );

  ops = mint_nodes_get_ops( nfrom );
  i = mint_ops_find( ops, "rows", mint_op_weights_init );
  if( i > -1 ) {
    nrows = mint_op_get_param( mint_ops_get( ops, i ), 0 );
  } else
    nrows = 1;

  size = mint_nodes_size( nfrom );
  ncols = size / nrows;

  val0 = p[0];
  dmax = p[1] + 1;
  valmax = p[2];

  mint_check( dmax>0, "distance must be >0 (parameter 2)" );

  /* NOTE: because w might be sparse for memory rather than for speed
     reasons, we access weight values with the clunkier interface
     valid for both sparse and dense matrices */

  /* coefficients of linear transformation between distance and weight
     value */ 
  a = (val0 - valmax) / ( 1 - dmax ); /* shortcut used below */
  b = val0 - a;

  for( ri=0; ri<nrows; ri++ ) {
    for( ci=0; ci<ncols; ci++ ) {
      for( rj=0; rj<nrows; rj++ ) {
	for( cj=0; cj<ncols; cj++ ) {
	  d = sqrt( (ri-rj)*(ri-rj) + (ci-cj)*(ci-cj) );
	  if( d>=1 && d<=dmax ) {
	    val = a * d + b;
	    /* translate between 2-dim and 1-dim indexing: */
	    mint_weights_set( w, 0, rj*ncols+cj, ri*ncols+ci, val );
	  }
	}
      }
    }
  }

  p[3] = 1; /* init done */
}
