#include "image.h"
#include "utils.h"
#include "op.h"
#include "str.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>

struct mint_image {
  SDL_Surface *surf;
};

static int mint_image_init_flag = 0;
static SDL_Surface *mint_screen = 0;

static float node_snapshot_param[4] = { 1, 1, 1, 0 };

/* functions to get and set pixels in SDL surfaces, from
   http://sdl.beuc.net/sdl.wiki/Pixel_Access */

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void mint_image_init( void ) {
  if( mint_image_init_flag )
    return;
  mint_check( SDL_Init( SDL_INIT_VIDEO ) != 0,
	      "cannot initialize image functions: %s",
	      SDL_GetError() );
  atexit( SDL_Quit );
  mint_image_init_flag = 1;

  mint_op_add( "snapshot", mint_op_nodes_update, mint_node_snapshot,
	       4, node_snapshot_param );
}

struct mint_image *mint_image_load( char *filename ) {
  struct mint_image *image;
  
  image = malloc( sizeof( struct mint_image ) );
  image->surf = IMG_Load( filename );
  mint_check( image->surf, "cannot load image from file %s", filename );
  return image;
}

void mint_image_del( struct mint_image *image ) {
  SDL_FreeSurface( image->surf );
  free( image );
}

void mint_image_save( const struct mint_image *image, char *filename ) {
  SDL_SaveBMP( image->surf, filename );
}

struct mint_image *mint_image_nodes( const mint_nodes nred, 
				     const mint_nodes ngreen, 
				     const mint_nodes nblue,
				     int var ) {
  struct mint_image *image;
  struct mint_ops *ops;
  int i, rows, cols, size, x, y, R, G, B;

  ops = mint_nodes_get_ops( nred );
  i = mint_ops_find( ops, "rows", mint_op_nodes_init );
  rows = i != -1 ? mint_op_get_param( mint_ops_get(ops, i), 0 ) : 1;

  size = mint_nodes_size( nred );
  cols = size / rows;

  if( ngreen && nblue ) {
    mint_check( size == mint_nodes_size(ngreen), 
		"image and second node sizes don't match" );
    mint_check( size == mint_nodes_size(nblue), 
		"image and third node sizes don't match" );
  } else if( ngreen || nblue ) {
      mint_check( 0, "must provide 1 or 3 node arguments, not 2" );
  } 

  /* FIX hardcoded bpp? */
  image = malloc( sizeof( struct mint_image ) );
  image->surf = SDL_CreateRGBSurface( 0, cols, rows, 24, 0, 0, 0, 0 ); 

  if( SDL_MUSTLOCK( image->surf )  )
    SDL_LockSurface( image->surf );

  for ( y=0; y<rows; y++ ) {
    for ( x=0; x<cols; x++ ) {
      if( !ngreen ) {
	R = G = B =  255 * nred[var][ y+rows*x ];
      } else {
	R = 255 * nred[var][ y+rows*x ];
	G = 255 * ngreen[var][ y+rows*x ];
	B = 255 * nblue[var][ y+rows*x ];
      }
      putpixel( image->surf, x, y, 
		SDL_MapRGB( image->surf->format, R, G, B ) );
    }
  }

  if( SDL_MUSTLOCK( image->surf )  )
    SDL_UnlockSurface( image->surf );
  
  return image; 
}

struct mint_image *mint_image_weights( const mint_weights w, int irows, 
				       int var ) {
  struct mint_image *image;
  int icols, cols, rows, x, y, wx, wy, count;
  int intensity, R, G, B;
  float max;

  cols = mint_weights_cols( w );
  rows = mint_weights_rows( w );

  if( irows>0 ) {
    mint_check( cols*rows % irows == 0, 
		"matrix size (%d * %d) not a multiple of image rows (%d)", 
		rows, cols, irows );
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

  /* FIX hardcoded bpp? */
  image = malloc( sizeof( struct mint_image ) );
  image->surf = SDL_CreateRGBSurface( 0, icols, irows, 24, 0, 0, 0, 0 ); 

  if( SDL_MUSTLOCK( image->surf )  )
    SDL_LockSurface( image->surf );

  count = 0;
  for ( y=0; y<irows; y++ ) {
    for ( x=0; x<icols; x++ ) {
      intensity = 255 *  *( &w[var][0][0] + count ) / max;
      if( intensity>0 )
	R = G = intensity, B = 0; 
      else
	R = G = 0, B = -intensity;
      putpixel( image->surf, x, y, 
		SDL_MapRGB( image->surf->format, R, G, B ) );
      count++;
    }
  }

  if( SDL_MUSTLOCK( image->surf )  )
    SDL_UnlockSurface( image->surf );

  /* this sends cell 1,1 in the top left corner as it is customary in
     math */
  mint_image_flipv( image );

  return image; 
}

void mint_image_fliph( struct mint_image *image ) {
  mint_check( 0, "unimplemented" );
}

void mint_image_flipv( struct mint_image *image ) {
  mint_check( 0, "unimplemented" );
}

void mint_image_rotate( struct mint_image *image, float angle ) {
  mint_check( 0, "unimplemented" );
}

void mint_image_paste( const struct mint_image *image, mint_nodes n,
		       int xpos, int ypos, int scale ) {
  int i, j, irows, icols, nrows, ncols, size, x, y, idx, var;
  float weight;
  SDL_Surface *surf;

  Uint8 mask, shift, loss, intensity;
  Uint32 pixel;
  struct mint_ops *ops;
  struct mint_op *op;
  const char *name;

  surf = 0;

  irows = image->surf->h;
  icols = image->surf->w;
    
  /* go through all ops and process red, green, and blue ops only */
  ops = mint_nodes_get_ops( n );
  for( i=0; i < mint_ops_size( ops ); i++ ) {
    mask = -1;
    op = mint_ops_get( ops, i );
    name = mint_op_name( op );
    if( strcmp( name, "red" ) == 0 ) {
      mask = image->surf->format->Rmask;
      shift = image->surf->format->Rshift;
      loss =  image->surf->format->Rloss;
    } else if( strcmp( name, "green" ) == 0 ) {
      mask = image->surf->format->Gmask;
      shift = image->surf->format->Gshift;
      loss =  image->surf->format->Gloss;
    } else if( strcmp( name, "blue" ) == 0 ) {
      mask = image->surf->format->Bmask;
      shift = image->surf->format->Bshift;
      loss =  image->surf->format->Bloss;
    }

    if( mask != -1 ) { /* if red/green/blue op found */
      
      j = mint_ops_find( ops, "rows", mint_op_nodes_init );
      if( j == -1 ) nrows = 1; 
      else nrows = mint_op_get_param( mint_ops_get(ops, j), 0 );
      size = mint_nodes_size( n );
      ncols = size / nrows;

      /* we set surf only here to potentially save us an image rescale
	 if there are no image ops for this node group. if surf is
	 already nonzero, the job has already been done erlier in the
	 loop, and we can just continue  */
      if( !surf ) {
	if( scale && (nrows != irows || ncols != icols) )
	  surf = zoomSurface( image->surf, 
			      ((float)ncols)/icols, 
			      ((float)nrows)/irows, SMOOTHING_ON );
	else
	  surf = image->surf;
      }
      
      var = mint_op_get_param( op, 1 );
      weight = mint_op_get_param( op, 0 );

      for ( y=0; y<irows; y++ ) {
	for ( x=0; x<icols; x++ ) {
	  idx = y+ypos + nrows*(x+xpos);
	  if( idx>=0 && idx<size ) {
	    pixel = getpixel( surf, x, y );
	    pixel = pixel & mask;
	    pixel = pixel >> shift;
	    intensity = (Uint8)( pixel << loss );
	    n[ var ][ idx ] += weight * intensity / 255;
	  }
	}
      }
    }
  }
  if( surf != image->surf )
    SDL_FreeSurface( surf );
}


void mint_image_scale( struct mint_image *image, float scale ) {
  SDL_Surface *newsurf;
  
  newsurf = rotozoomSurface( image->surf, 0, scale, SMOOTHING_ON );
  SDL_FreeSurface( image->surf );
  image->surf = newsurf;
}

SDL_Surface *mint_image_get_surface( struct mint_image *image ) {
  return image->surf;
}

struct mint_image *mint_image_from_surface( SDL_Surface *surf ) {
  struct mint_image *image;
  image = malloc( sizeof(struct mint_image) );
  image->surf = surf;
  return image;
}

void mint_node_snapshot( mint_nodes n, int min, int max, float *p ) {
  int frequency, state, overwrite, len;
  struct mint_image *img;
  char *filename;
  struct mint_str *name;

  frequency = p[0];
  state = p[1];
  overwrite = p[2];

  p[3] += 1; /* counter for number of times function called */

  if( ( (int)p[3] - 1 ) % frequency )
    return;

  img = mint_image_nodes( n, 0, 0, state ); 

  name = mint_nodes_get_name( n );
  len = mint_str_size( name );
  len += 9; /* "mint/" + ".bmp" */

  if( !overwrite )
    len += mint_str_numlen( (int)p[3] ) + 1;

  filename = malloc( len + 1 );
  
  if( overwrite )
    sprintf( filename, "mint/%s.bmp", mint_str_char(name) );
  else
    sprintf( filename, "mint/%s-%d.bmp", mint_str_char(name), (int)p[3] );

  mint_image_save( img, filename );
  mint_image_del( img );
}
