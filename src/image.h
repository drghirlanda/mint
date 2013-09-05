#ifndef MINT_IMAGE_H
#define MINT_IMAGE_H

#include "nodes.h"
#include "weights.h"
#include "image.h"

#include <stdio.h>
#include <FreeImage.h>

/** \file image.h

    \brief Function to manipulate images, including representing nodes
    and weights as images as transforming images into node activity
    patterns. 

    We use the FreeImage library for image functionality, providing a
    simplified interface and the ability to convert mint objects to
    and from images. The FreeImage bitmap held by a mint_image struct
    can be retrieved to use all FreeImage functions. Note, however,
    that the scale, flip and rotate methods below allow to realize all
    2D affine transformations. */

/** Data structure used to manipulate images. */
struct mint_image;

/** Read an image from file. */
struct mint_image *mint_image_load( char *file );

/** Write image to file. The format can be any format suppported by
    FreeImage; common formats are: jpg, png, svg, tiff, gif, pnm.
    Corresponding format names are FIF_JPEG, FIF_PNG, and so on. */
void mint_image_save( const struct mint_image *, 
		      char *filename, FREE_IMAGE_FORMAT fif );

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

/** Paste an image on the 'var' variable of 1 or 3 node objects (1
    node while treat the image as grayscale, 3 will paste the RGB
    components on different nodes. The nodes are interpreted as
    rectangular artificial retinas, with a number of rows equal to
    their row value (see mint_op_rows) and number of columns equal to
    size)/nrows columns (if size is not an exact multiple of rows,
    some columns will be unaffected by image pasting). The image can
    be translated by xpos and ypos pixels (can be negative). Any parts
    of the image not falling on the retina are ignored. */
void mint_image_paste( const struct mint_image *, mint_nodes nred,
		       mint_nodes ngreen, mint_nodes nblue,
		       int varr, int varg, int varb, 
		       int xpos, int ypos );

void mint_image_paste_gray( const struct mint_image *, mint_nodes ngray,
			    int var, int xpos, int ypos );

/** Scale an image to 'scale' times its current size. */
void mint_image_scale( struct mint_image *, float scale );

/** Flip image horizontally. UNIMPLEMENTED */
void mint_image_fliph( struct mint_image * );

/** Flip image vertically. UNIMPLEMENTED */
void mint_image_flipv( struct mint_image * );

/** Rotate image 'angle' degrees counterclockwise. */
void mint_image_rotate( struct mint_image *, float angle );

/** Access the FIBITMAP stored within a mint image. */
FIBITMAP *mint_image_get_FreeImage( struct mint_image * );

/** Create a mint_image from a FIBITMAP. */
struct mint_image *mint_image_from_FreeImage( FIBITMAP * );

#endif /* MINT_IMAGE_H */
