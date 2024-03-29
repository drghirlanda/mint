#include "image.h"
#include "utils.h"
#include "op.h"
#include "string.h"
#include "network.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>

struct mint_image {
  SDL_Surface *surf;
};

/* SDL variables and parameters */
static int mint_image_init_flag = 0;
static SDL_Surface *mint_screen = 0;
static int mint_vmode = SDL_SWSURFACE | SDL_ANYFORMAT | SDL_RESIZABLE;
static SDL_Color mint_color = { 0, 192, 0, 255 };
static SDL_Color mint_background = { 0, 0, 0, 255 };
static TTF_Font *mint_font = 0;
static TTF_Font *mint_font_small = 0;

static float node_snapshot_param[] = { 1, 1, 1, 0 };
static float network_display_param[] = { 1, 0, 1 };
static float node_key_param[] = { 0, 0, 0, 0 };

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
  int i;
  if( mint_image_init_flag )
    return;
  i = SDL_Init( SDL_INIT_VIDEO );
  mint_check( i == 0,
	      "cannot initialize image functions: %s",
	      SDL_GetError() );
  atexit( SDL_Quit );
  mint_image_init_flag = 1;

  mint_op_add( "snapshot", mint_op_nodes_update, mint_node_snapshot,
	       4, node_snapshot_param );
  mint_op_add( "key", mint_op_nodes_event, mint_node_key,
	       4, node_key_param );
  mint_op_add( "display", mint_op_network_operate, mint_network_display,
	       3, network_display_param );
  mint_op_add( "events", mint_op_network_operate, mint_network_events,
	       0, 0 );
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
  int rows, cols, size, x, y, bpp;
  Uint8 R, G, B;
  float frows;

  if( mint_nodes_get_property( nred, "rows", 0, &frows ) )
    rows = frows;
  else
    rows = 1;

  size = mint_nodes_size( nred );

  cols = size / rows;

  while( cols*rows < size ) 
    cols++;

  if( ngreen && nblue ) {
    mint_check( size == mint_nodes_size(ngreen), 
		"image and second node sizes don't match" );
    mint_check( size == mint_nodes_size(nblue), 
		"image and third node sizes don't match" );
  } else if( ngreen || nblue ) {
      mint_check( 0, "must provide 1 or 3 node arguments, not 2" );
  } 

  /* same pixel format as screen*/
  if( mint_screen ) {
    bpp = mint_screen->format->BitsPerPixel;
  } else {
    bpp = 24;
  }

  image = malloc( sizeof( struct mint_image ) );
  image->surf = SDL_CreateRGBSurface( SDL_SWSURFACE, 
				      cols, rows, bpp, 0, 0, 0, 0 ); 

  if( SDL_MUSTLOCK( image->surf )  )
    SDL_LockSurface( image->surf );

  for ( x=0; x<cols; x++ ) {
    for ( y=0; y<rows; y++ ) {
      if( !ngreen ) {
	R = G = B =  255 * nred[var][ x + y*cols ];
      } else {
	R = 255 * nred[var][ x + y*cols ];
	G = 255 * ngreen[var][ x + y*cols ];
	B = 255 * nblue[var][ x + y*cols ];
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
  int icols, cols, rows, x, y, wx, wy, count, bpp, intensity;
  Uint8 R, G, B;
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

  /* same pixel format as screen*/
  if( mint_screen ) {
    bpp = mint_screen->format->BitsPerPixel;
  } else {
    bpp = 24;
  }

  image = malloc( sizeof( struct mint_image ) );
  image->surf = SDL_CreateRGBSurface( SDL_SWSURFACE, 
				      icols, irows, bpp, 0, 0, 0, 0 ); 

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

  return image; 
}

void mint_image_fliph( struct mint_image *image ) {
  MINT_UNUSED( image );
  mint_check( 0, "unimplemented" );
}

void mint_image_flipv( struct mint_image *image ) {
  MINT_UNUSED( image );
  mint_check( 0, "unimplemented" );
}

void mint_image_rotate( struct mint_image *image, float angle ) {
  MINT_UNUSED( image );
  MINT_UNUSED( angle );
  mint_check( 0, "unimplemented" );
}

void mint_image_paste( const struct mint_image *image, mint_nodes n,
		       int xpos, int ypos, int scale ) {
  int i, j, irows, icols, nrows, ncols, size, x, y, idx, var, 
    *init, states;
  float weight;
  SDL_Surface *surf;

  Uint8 intensity;
  Uint32 pixel, mask, shift, loss;
  struct mint_ops *ops;
  struct mint_op *op;

  surf = 0;

  irows = image->surf->h;
  icols = image->surf->w;

  /* we want to set the target variable to zero only once, hence we
     have to keep track of when we do that */
  states = mint_nodes_states( n );
  init = malloc( ( 2 + states ) * sizeof(int) );
  for( i=0; i<2+states; i++ )
    init[i] = 0;

  /* go through all ops and process red, green, and blue ops only */
  ops = mint_nodes_get_ops( n );
  op = mint_ops_get_name( ops, "color", mint_op_nodes_init );
  if( op ) {
    var = mint_op_get_param( op, 3 );
    mint_nodes_set( n, var, 0 );
    for( i=0; i<3; i++ ) {
      switch( i ) {
      case 0:
	mask = image->surf->format->Rmask;
	shift = image->surf->format->Rshift;
	loss =  image->surf->format->Rloss;
	break;
      case 1:
	mask = image->surf->format->Gmask;
	shift = image->surf->format->Gshift;
	loss =  image->surf->format->Gloss;
	break;
      case 2:
	mask = image->surf->format->Bmask;
	shift = image->surf->format->Bshift;
	loss =  image->surf->format->Bloss;
      }
      
      j = mint_ops_find( ops, "rows", mint_op_nodes_init );
      if( j == -1 ) nrows = 1; 
      else nrows = mint_op_get_param( mint_ops_get(ops, j), 0 );
      size = mint_nodes_size( n );
      ncols = size / nrows;

      if( !surf ) {
	if( scale && (nrows != irows || ncols != icols) )
	  surf = zoomSurface( image->surf, 
			      ((float)ncols)/icols, 
			      ((float)nrows)/irows, SMOOTHING_OFF );
	else
	  surf = image->surf;
      }
      
      weight = mint_op_get_param( op, i );

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
  free( init );
}


void mint_image_scale( struct mint_image *image, float scale ) {
  SDL_Surface *newsurf;
  
  newsurf = rotozoomSurface( image->surf, 0, scale, SMOOTHING_OFF );
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

void mint_nodes_event( mint_nodes n, int min, int max, SDL_Event ev ) {
  int i;
  struct mint_ops *ops;
  struct mint_op *op;
  ops = mint_nodes_get_ops( n );

  for( i=0; i<mint_ops_size(ops); i++ ) {
    op = mint_ops_get( ops, i );
    if( mint_op_type( op )  == mint_op_nodes_event )
      mint_op_run( op, n, min, max, mint_op_get_params( op ), ev );
  }
}

void mint_node_key( mint_nodes n, int min, int max, float *p,
		    SDL_Event ev ) {
  int i, var, mode, key, states; 
  float value;

  key = p[0];
  var = p[1];
  value = p[2];
  mode = p[3];

  states = mint_nodes_states(n);

  mint_check( key>=32 && key<=127, 
	      "key (param 0) must be between 32 and 127" );
  mint_check( var >= 0, "variable (param 1) must be >= 0" );
  mint_check( var < 2+states, "variable ([aram 1) must be < %d",
	      2+states );
  mint_check( mode==0 || mode==1, "mode (param 3) must be 0 or 1" );

  if( ev.type != SDL_KEYUP || ev.key.keysym.sym != (unsigned int)key )
    return;

  /* mode 0/1 switches between increment value or set value */
  for( i=min; i<max; i++ )
    n[ var ][i] = mode * n[ var ][i] + value;
 
}

void mint_node_snapshot( mint_nodes n, int min, int max, float *p ) {
  int frequency, state, overwrite, len;
  struct mint_image *img;
  char *filename;
  mint_string name;

  MINT_UNUSED( min );
  MINT_UNUSED( max );

  frequency = p[0];
  state = p[1];
  overwrite = p[2];

  p[3] += 1; /* counter for number of times function called */

  if( ( (int)p[3] - 1 ) % frequency )
    return;

  img = mint_image_nodes( n, 0, 0, state ); 

  name = mint_nodes_get_name( n );
  len = mint_string_size( name );
  len += 9; /* "mint/" + ".bmp" */

  if( !overwrite )
    len += mint_string_numlen( (int)p[3] ) + 1;

  filename = malloc( len + 1 );
  
  if( overwrite )
    sprintf( filename, "mint/%s.bmp", name );
  else
    sprintf( filename, "mint/%s-%d.bmp", name, (int)p[3] );

  mint_image_save( img, filename );
  mint_image_del( img );
}

void mint_init_screen( int groups ) {
  int w, h, wgroup;
  const SDL_VideoInfo *info;

  if( mint_screen ) 
    return;

  info = SDL_GetVideoInfo();
  w = info->current_w / 2;
  wgroup = ( w - 0.05 * w * (groups+1) ) / groups;
  h = 1.75 * wgroup;

  mint_screen = SDL_SetVideoMode( w, h, 0, mint_vmode );
  mint_check( mint_screen, "cannot open window: %s", SDL_GetError() );
  SDL_WM_SetCaption( "MINT", "MINT" );

  if( TTF_Init() == 0 ) {
    mint_font = TTF_OpenFont( MINT_DIR "/include/mint/FreeSans.ttf", 24 );
    mint_font_small = TTF_OpenFont( MINT_DIR "/include/mint/FreeSans.ttf", 14 );
    if( mint_font || mint_font_small )
      atexit( TTF_Quit );
  }
}

void mint_text_display( const char *message, int x, int y, 
			TTF_Font *font ) {
  SDL_Surface *text;
  SDL_Rect dest;
  int wtext, htext;

  if( !font || !mint_screen )
    return;

  TTF_SizeUTF8( font, message, &wtext, &htext );
  
  dest.x = x - .5 * wtext;
  dest.y = y - .5 * htext;

  text = TTF_RenderUTF8_Shaded( font, message, mint_color, mint_background );
  SDL_BlitSurface( text, 0, mint_screen, &dest );
  SDL_UpdateRect( mint_screen, dest.x, dest.y, dest.w, dest.h );
  SDL_FreeSurface( text );
}

void mint_image_display_status( int status ) {
  SDL_Rect r;
  int w; 

  r.x = 0.05 * mint_screen->w;
  r.y = 0.90 * mint_screen->h;
  r.w = 0.05 * mint_screen->w;
  r.h = 0.05 * mint_screen->w;

  switch( status ) {
  case -1: /* simulation stopped - draw a square */
    boxRGBA( mint_screen, r.x, r.y, r.x + r.w, r.y + r.h, 
		  0, 192, 0, 255 );
    break;
  case 0: /* display stopped, simulation active - draw pause symbol*/
    w = r.w/3 - 1; /* ensures same rounding in both lines below */
    boxRGBA( mint_screen, r.x, r.y, r.x + w, r.y + r.h, 
	     0, 192, 0, 255 );
    boxRGBA( mint_screen, r.x + 2*w, r.y, r.x + 3*w, r.y+r.h, 
	     0, 192, 0, 255 );
    break;
  case 1: /* display and simulation active - erase symbols */
    boxRGBA( mint_screen, r.x - 5, r.y - 5, 
	     r.x + r.w + 5, r.y + r.h + 5, 
	     0, 0, 0, 255 );
    break;
  }

  SDL_UpdateRect( mint_screen, r.x, r.y, r.w, r.h );
}

void mint_image_display( struct mint_image *src, 
			 int x, int y, int w, int h ) {
  SDL_Rect dest;
  SDL_Surface *scaled;
  int i;
  float scale;

  MINT_UNUSED( h );

  mint_init_screen( 1 );

  scale = (float)w / src->surf->w;
  scaled = rotozoomSurface( src->surf, 0, scale, SMOOTHING_OFF );

  dest.x = x;
  dest.y = y;
  i = SDL_BlitSurface( scaled, 0, mint_screen, &dest ); 
  mint_check( i==0, "cannot display image: %s", SDL_GetError() );
  SDL_UpdateRect( mint_screen, dest.x-1, dest.y-1, dest.w+1, dest.h+1 );
  SDL_FreeSurface( scaled );
}

void *mint_poll_stdin_helper( void *data ) {
  int *c = (int *)data;

  *c = fgetc( stdin );
  pthread_exit( 0 );
  return 0;
}

/* translate stdin keypresses into SDL events */
int mint_poll_stdin( SDL_Event *event ) {
  static pthread_t thread_id = 0;
  static int c = -1;

  /* create thread that will read c from stdin */
  if( thread_id == 0 )
    pthread_create( &thread_id, 0, mint_poll_stdin_helper, (void *) &c );

  /* return if c not ready */
  if( c == -1 )
    return 0; 

  /* c ready! */

  pthread_join( thread_id, 0 ); /* cleanup */
  event->type = SDL_KEYUP;      /* build SDL_Event */
  event->key.keysym.sym = c;

  c = -1;                       /* reset to clean state */
  thread_id = 0;

  return 1;
}


void mint_network_events( struct mint_network *net, float *p ) {
  int i;
  mint_nodes n;
  SDL_Event event;
  SDLKey key;
  float active, rate;

  MINT_UNUSED( p );

  if( !mint_screen && 
      !mint_network_get_property( net, "display", 0, 0 ) ) {
    mint_screen = SDL_SetVideoMode( 0, 0, 0, mint_vmode );
  }

  while( 1 ) {

    if( !SDL_PollEvent( &event ) && !mint_poll_stdin( &event ) )
      return;
    
    switch( event.type ) {

    case SDL_VIDEORESIZE: /* resize MINT window */
      SDL_SetVideoMode( event.resize.w, event.resize.h, 0, 
			mint_vmode );
      break;
      
    case SDL_KEYUP:
      key = event.key.keysym.sym;
      
      if( key == SDLK_SPACE ) { /* pause simulation */
	/* wait for another space press */
	mint_image_display_status( -1 );
	while( SDL_WaitEvent( &event ) ) {
	  if( event.type == SDL_KEYUP &&
		event.key.keysym.sym == SDLK_SPACE ) {
	    mint_image_display_status( 1 );
	    break;
	  }
	}
	
      } else if( key == SDLK_q ) { /* quit simulation */
	exit( EXIT_SUCCESS );
	
      } else if( key == SDLK_p ) { /* pause/resume display */
	mint_network_get_property( net, "display", 2, &active );
	if( active ) {
	  mint_network_set_property( net, "display", 2, 0. );
	  mint_image_display_status( 0 );
	} else {
	  mint_network_set_property( net, "display", 2, 1. );
	  mint_image_display_status( 1 );
	}
	
      } else if( key == SDLK_EQUALS ) { /* decrease sampling */
	mint_network_get_property( net, "display", 0, &rate );
	rate = rate + 10;
	  rate = rate - fmod(rate,10);
	  mint_network_set_property( net, "display", 0, rate );
	  
      } else if( key == SDLK_MINUS ) { /* increase sampling */
	mint_network_get_property( net, "display", 0, &rate );
	rate = rate - 10 >= 1 ? rate - 10 : 1;
	mint_network_set_property( net, "display", 0, rate );
      } else {
	/* other events */
	for( i=0; i<mint_network_groups(net); i++ ) {
	  n = mint_network_get_nodes( net, i );
	  mint_nodes_event( n, 0, mint_nodes_size(n), event );
	}
      }
      break;
	
    case SDL_QUIT:
      exit( EXIT_SUCCESS );
      break;
    }
  }
}

void mint_network_display( struct mint_network *net, float *p ) {
  int i, groups, size, rows, rate, x, y, w, h;
  float frows;
  struct mint_op *op;
  struct mint_image *img;
  mint_nodes n;
  char msg[256];
 
  rate = p[0];
  mint_check( rate>0, "invalid rate: %d", rate );

  p[1] += 1;

  /* add events op if not present, this will poll graphical
     elements. */
  if( !mint_network_get_property( net, "events", 0, 0 ) ) {
    op = mint_op_new( "events", mint_op_network_operate );
    mint_ops_append( mint_network_get_ops(net), op );
    mint_op_del( op );
  }

  groups = mint_network_groups( net );
  mint_init_screen( groups );

  /* info at lower right corner (won't do anything until MINT window
     has been created) */
  sprintf( msg, "   1/%d, %d   ", rate, (int)p[1] );
  mint_text_display( msg, 0.9 * mint_screen->w, 
		     0.925 * mint_screen->h, mint_font_small );

  if( p[2] == 0 ) /* display inactive */
    return;

  if( ( (int)p[1] - 1 ) % rate ) /* not a sampled step */
    return;

  y = 0.05 * mint_screen->h;
  w = ( ( 1.0 - 0.05 * (groups+1) ) / groups ) * mint_screen->w;

  for( i=0; i<groups; i++ ) {
    n = mint_network_get_nodes( net, i );
    size = mint_nodes_size( n );

    if( mint_nodes_get_property( n, "rows", 0, &frows ) )
      rows = frows;
    else
      rows = sqrt( size );

    h = (w * rows * rows) / size;

    img = mint_image_nodes( n, 0, 0, 1 );
    x = w * i + 0.05 * (i+1) * mint_screen->w;
    mint_image_display( img, x, y, w, h );
    mint_image_del( img );

    mint_text_display( mint_nodes_get_name(n),
		       x + .5*w, y + h + 0.1 * mint_screen->h, 
		       mint_font );
  }
}
