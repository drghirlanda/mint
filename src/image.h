#ifndef MINT_IMAGE_H
#define MINT_IMAGE_H

#include "nodes.h"
#include "weights.h"
#include "image.h"

#include <stdio.h>
#include <SDL/SDL.h>

/** \file image.h

    \brief Function to manipulate images, including representing nodes
    and weights as images as transforming images into node activity
    patterns. 

    We use the SDL library for image functionality, providing a
    simplified interface and the ability to convert mint objects to
    and from images. The SDL_Surface object held by a mint_image
    struct can be retrieved to use all SDL functions. Note, however,
    that the scale, flip and rotate methods below allow to realize all
    2D affine transformations. */

/** Data structure used to manipulate images. */
struct mint_image;

/** Call this before using any image related function or op. */
void mint_image_init( void );

/** Read an image from file. */
struct mint_image *mint_image_load( char *file );

/** Write image to file. The format can be any format suppported by
    FreeImage; common formats are: jpg, png, svg, tiff, gif, pnm.
    Corresponding format names are FIF_JPEG, FIF_PNG, and so on. */
void mint_image_save( const struct mint_image *, char *filename );

/** Delete image object */ 
void mint_image_del( struct mint_image * );

/** Build an image from one (grayscale image) or three (rgb color
    image) nodes objects. The grayscale image is built from the values
    of nred if ngreen and nblue are 0. The nodes objects must all have
    the same size. The number of rows will be the height of the image
    in pixels and will be taken from the first node group (see
    mint_node_rows for how to specify rows). The number of columns
    (image width in pixels) will be size/rows (some columns will be
    discarded if this is not an integer). var is the nodes variable
    whose values are used, e.g., 0 for node input and 1 for node
    output.

    \param nred Nodes whose values will be the RED values of the image
    or, if ngreen == nblue == 0, the grayscale values. 

    \param ngreen Green component values, or 0 for grayscale images. 

    \param nblue Blue component values, or 0 for grayscale images. 

    \param rows The number of rows of the resulting image (the height
    in pixels of the image). It must divide nodes size exactly, so that
    rows/size will be the number of columns of the image (image width).

    \param var Which variable of the nodes to use to build the image,
    e.g., 0 to image node inputs or 1 to image node output.

 */
struct mint_image *mint_image_nodes( const mint_nodes nred, 
				     const mint_nodes ngreen,
				     const mint_nodes nblue,
				     int var );

/** Build an image from a weight matrix. The weights are color coded
    so that yellow pixels represent positive weights and blue pixels
    negative ones. Color values are scaled so that the weight with
    maximum absolute value gets the brightest color (rgb triplet
    (255,255,0) if the maximum is for a positive weight, and (0,0,255)
    if the maximum is for a positive weight.

    \param irows Can be used to reshape the image to have irows rather
   than mint_weights_rows(w) rows. Pass 0 if you don't need to reshape.

   \param var The weights variable to be imaged, e.g., 0 for weight
   values, and >0 for node state variables. */
struct mint_image *mint_image_weights( const mint_weights w, 
				       int irows, int var );

/** Paste an image onto a node object. The nodes are interpreted as
    rectangular artificial retinas, with a number of rows equal to
    parameter 0 of the "rows" op (see mint_op_rows) and number of
    columns = size/rows columns. The image can be translated by xpos
    and ypos pixels (can be negative). Any parts of the image not
    falling on the retina are ignored. If the scale argument is 1, the
    image is first scaled to be as close as possible to the size of
    the grid formed by the node group (it will not be the same size if
    the aspect ratio differs). */
void mint_image_paste( const struct mint_image *, mint_nodes n,
		       int xpos, int ypos, int scale );

/** Scale an image to 'scale' times its current size. */
void mint_image_scale( struct mint_image *, float scale );

/** Flip image horizontally. UNIMPLEMENTED */
void mint_image_fliph( struct mint_image * );

/** Flip image vertically. UNIMPLEMENTED */
void mint_image_flipv( struct mint_image * );

/** Rotate image 'angle' degrees counterclockwise. */
void mint_image_rotate( struct mint_image *, float angle );

/** Access the FIBITMAP stored within a mint image. */
SDL_Surface *mint_image_get_SDL( struct mint_image * );

/** Create a mint_image from a FIBITMAP. */
struct mint_image *mint_image_from_SDL( SDL_Surface * );

/** Display image on the MINT screen. */ 
void mint_image_display( struct mint_image *, 
			 int x, int y, int w, int h );

/** Display activity of network on the MINT screen */
void mint_network_display( struct mint_network *net, float *p );
  
/** Periodically save an image of node state to file. Images are saved
    in a folder named mint (which must be created beforehand) with a
    filename composed of node group name, state variable index, and
    eventually a progressive number (see parameter 2 below).

    State variables: none required.
 
    Parameters: 0: Snapshot frequency. For example, if == 5 a snapshot
                   will be saved every 5 node updates (default 1).

                1: Which state variable to image (default 1).

		2: If == 1, overwrite a single file repeatedly, if ==
                   0 save each snapshot to a separate file, adding a
                   progressive number at the end of the filename
                   (default 1). */
void mint_node_snapshot( mint_nodes n, int min, int max, float *p );

/** Document! */
void mint_node_key( mint_nodes n, int min, int max, float *p, 
		    SDL_Event ev );

/** An op to enable the event framework. Takes no parameters. It is
    automatically added if the display op is used (to manage GUI
    events), but it can also be used on its own without a display to
    enable other events such as keypresses. */
void mint_network_events( struct mint_network *net, float *p );


#endif /* MINT_IMAGE_H */
