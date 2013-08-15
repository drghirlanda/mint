mint_nodes mint_nodes_load_pgm( FILE *file ) {
  int size, read, i, k;
  float max;
  mint_nodes n;
  char c;

  c = fgetc( file );
  if( c!='P' ) { 
    ungetc( c, file );
    return 0; /* non-fatal error, handled by mint_nodes_load */
  }

  c = fgetc( file );
  mint_check( c=='2', "image file does not start with 'P2'" );
  read = fscanf( file, "%d %d %f", &i, &k, &max );
  mint_check( read==3, "cannot read PGM parameters" );
  size = i*k;

  n = mint_nodes_new( size, 0 );
  for( i=0; i<size; i++ ) {
    read = fscanf( file, " %f", n[1]+i );
    mint_check( read==1, "cannot read values" );
    n[1][i] /= max;
  }
  return n;
}
