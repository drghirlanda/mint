#include "image.h"
#include "utils.h"
#include "op.h"

#include <stdlib.h>
#include <math.h>

static int mint_freeimage_init_flag = 0;

struct mint_image {
  FIBITMAP *ptr;
};


void mint_freeimage_init( void ) {
  if( mint_freeimage_init_flag )
    return;
  FreeImage_Initialise( FALSE );
  atexit( FreeImage_DeInitialise );
  mint_freeimage_init_flag = 1;
}

struct mint_image *mint_image_load( char *filename ) {
  struct mint_image *image;
  FREE_IMAGE_FORMAT fif;
  
  mint_freeimage_init();

  image = malloc( sizeof(struct mint_image) );
  fif = FreeImage_GetFileType( filename, 0 );
  if( fif == FIF_UNKNOWN )
    fif = FreeImage_GetFIFFromFilename( filename );
  mint_check( fif != FIF_UNKNOWN, "cannot determine image type" );
  mint_check( FreeImage_FIFSupportsReading(fif),"unsopported image type" ); 
  image->ptr = FreeImage_Load( fif, filename, 0 );
  return image;
}

void mint_image_del( struct mint_image *image ) {
  FreeImage_Unload( image->ptr );
  free( image );
}

void mint_image_save( const struct mint_image *image, char *filename,
		       FREE_IMAGE_FORMAT fif ) {
   FreeImage_Save( fif, image->ptr, filename, 0 );
 }

struct mint_image *mint_image_nodes( const mint_nodes nred, 
				     const mint_nodes ngreen, 
				     const mint_nodes nblue,
				     int var ) {
  struct mint_image *image;
  struct mint_ops *ops;
  int i, rows, cols, size, x, y;
  int stride;
  BYTE *bits;

  ops = mint_nodes_get_ops( nred );
  i = mint_ops_find( ops, "rows" );
  mint_check( i>=0, "rows is not set for first node group" );
  rows = mint_op_get_param( mint_ops_get(ops, i), 0 );

  size = mint_nodes_size( nred );
  cols = size / rows;

  if( ngreen && nblue ) {
    mint_check( size == mint_nodes_size(ngreen), 
		"image and second node sizes don't match" );
    mint_check( size == mint_nodes_size(nblue), 
		"image and third node sizes don't match" );
  } else {
    mint_check( 0, "must provide 1 or 3 node arguments, not 2" );
  } 

  image = malloc( sizeof(struct mint_image) );
  /* FIX hardcoded bpp in next line and in loop below */
  image->ptr = FreeImage_Allocate( cols, rows, 24, 
				   FI_RGBA_RED_MASK, 
				   FI_RGBA_GREEN_MASK, 
				   FI_RGBA_BLUE_MASK );

  stride = FreeImage_GetLine(image->ptr) / FreeImage_GetWidth(image->ptr);

  for ( y=0; y<rows; y++ ) {
    bits = FreeImage_GetScanLine( image->ptr, y );
    for ( x=0; x<cols; x++ ) {
      if( !ngreen ) {
	bits[FI_RGBA_RED] = bits[FI_RGBA_GREEN] = bits[FI_RGBA_BLUE] =
	  255 * nred[var][ y+rows*x ];
      } else {
	bits[FI_RGBA_RED] = 255 * nred[var][ y+rows*x ];
	bits[FI_RGBA_GREEN] = 255 * ngreen[var][ y+rows*x ];
	bits[FI_RGBA_BLUE] = 255 * nblue[var][ y+rows*x ];
      }
      bits += stride;
    }
  }

  return image; 
}

struct mint_image *mint_image_weights( const mint_weights w, int irows, 
				       int var ) {
  struct mint_image *image;
  int icols, cols, rows, x, y, wx, wy, count, stride;
  int intensity;
  float max;
  BYTE *bits;

  cols = mint_weights_cols( w );
  rows = mint_weights_rows( w );

  if( irows>0 ) {
    mint_check( cols*rows % irows == 0, "cols*rows not multiple of irows" );
    icols = irows; /* transpose to get things right */
    irows = cols*rows / icols;
  } else {
    irows = rows;
    icols = cols;
  }

  /* determine min and max to scale the gray values */
  max = 0;
  for( wy=0; wy<rows; wy++ ) {
    for( wx=0; wx<cols; wx++ ) {
      if( fabs(w[var][wy][wx]) > max )
	 max = fabs(w[var][wy][wx]);
    }
  }

  image = malloc( sizeof(struct mint_image) );
  /* FIX hardcoded bpp in next line and in loop below */
  image->ptr = FreeImage_Allocate( icols, irows, 24, 
				   FI_RGBA_RED_MASK, 
				   FI_RGBA_GREEN_MASK, 
				   FI_RGBA_BLUE_MASK );

  stride = FreeImage_GetLine(image->ptr) / FreeImage_GetWidth(image->ptr);

  count = 0;
  for ( y=0; y<irows; y++ ) {
    bits = FreeImage_GetScanLine( image->ptr, y );
    for ( x=0; x<icols; x++ ) {
      intensity = 255 *  *( &w[var][0][0] + count ) / max;
      if( intensity>0 ) {
	bits[FI_RGBA_RED] = bits[FI_RGBA_GREEN] = intensity; 
      } else {
	bits[FI_RGBA_BLUE] = -intensity;
      }
      bits += stride;
      count++;
    }
  }

  return image; 
}

void mint_image_fliph( struct mint_image *image ) {
  FreeImage_FlipHorizontal( image->ptr );
}

void mint_image_flipv( struct mint_image *image ) {
  FreeImage_FlipVertical( image->ptr );
}

void mint_image_rotate( struct mint_image *image, float angle ) {
  FIBITMAP *fib;
  fib = FreeImage_RotateClassic( image->ptr, angle );
  FreeImage_Unload( image->ptr );
  image->ptr = fib;  
}

void mint_image_paste( const struct mint_image *image, 
		       mint_nodes nred,
		       mint_nodes ngreen, 
		       mint_nodes nblue,
		       int var, int xpos, int ypos ) {
  int i, irows, icols, nrows, ncols, size, x, y;
  int stride, idx;
  BYTE *bits;
  FIBITMAP *fib;
  float scale;
  struct mint_ops *ops;

  mint_check( xpos>=0 && ypos>=0, "negative xpos/ypos unimlemented" );

  size = mint_nodes_size( nred );

  if( ngreen && nblue ) {
    mint_check( mint_nodes_size(ngreen)==size, "ngreen size != nred" );
      mint_check( mint_nodes_size(nblue)==size, "nblue size != nred" );
  } else {
    mint_check( 0, "must provide 1 or 3 node arguments, not 2" );
  } 

  /* set node rows and columns, then image rows and columns */
  ops = mint_nodes_get_ops( nred );
  i = mint_ops_find( ops, "rows" );
  mint_check( i>=0, "rows is not set for first node group" );
  nrows = mint_op_get_param( mint_ops_get(ops, i), 0 );
  ncols = size / nrows;
  irows = FreeImage_GetHeight( image->ptr );
  icols = FreeImage_GetWidth( image->ptr );

  /* choose the smaller dimension */
  if( nrows/irows < ncols/icols )
    scale = (float)nrows / irows;
  else
    scale = (float)ncols / icols;
  
  if( scale != 1 )
    fib = FreeImage_Rescale( image->ptr, irows*scale, icols*scale, FILTER_BOX );
  else
    fib = image->ptr;

  irows = FreeImage_GetHeight( fib );
  icols = FreeImage_GetWidth( fib );
  stride = FreeImage_GetLine(fib) / FreeImage_GetWidth(fib);

  for ( y=0; y<irows; y++ ) {
    bits = FreeImage_GetScanLine( fib, y );
    for ( x=0; x<icols; x++ ) {
      idx = y+ypos + nrows*(x+xpos);
      if( idx>=0 && idx<size ) {
	if( !ngreen ) {
	  nred[var][ idx ] = ( bits[FI_RGBA_RED] + bits[FI_RGBA_GREEN] +
			       bits[FI_RGBA_BLUE] ) / 255.0;
	} else {
	  nred[var][ idx ] = bits[FI_RGBA_RED] / 255.0;
	  ngreen[var][ idx ] = bits[FI_RGBA_GREEN] / 255.0;
	  nblue[var][ idx ] = bits[FI_RGBA_BLUE] / 255.0;
	}
      }
      bits += stride;
    }
  }

  if( fib != image->ptr )
    FreeImage_Unload( fib );
}

void mint_image_scale( struct mint_image *image, float scale ) {
  FIBITMAP *newptr;
  int newrows, newcols;
  
  newrows = scale * FreeImage_GetHeight( image->ptr );
  newcols = scale * FreeImage_GetWidth( image->ptr );
  newptr = FreeImage_Rescale( image->ptr, newcols, newrows,
			      FILTER_BOX );
  FreeImage_Unload( image->ptr );
  image->ptr = newptr;
}

FIBITMAP *mint_image_get_FreeImage( struct mint_image *image ) {
  return image->ptr;
}

struct mint_image *mint_image_from_FreeImage( FIBITMAP *fib ) {
  struct mint_image *image;
  image = malloc( sizeof(struct mint_image) );
  image->ptr = FreeImage_Clone( fib );
  return image;
}

