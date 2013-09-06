#ifndef MINT_CAMERA_H
#define MINT_CAMERA_H

#include "nodes.h"

struct mint_network;

/** \file camera.h

    \brief Functions to capture images from a camera and to paste them
    onto node groups.

    The camera is initialized the first time one of these functions is
    called. The initialization may take a few seconds. 

    The image capture functionality is provided by camshot, see
    \url{https://code.google.com/p/camshot}.

*/

/** Initialize the camera. It must be called before any access to the
    camera is attempted from within MINT (including loading an
    architecture file using the "camera" network operation). */
void mint_camera_init( void );

/** Get an image from the camera. See the image functions (image.h)
    for information about image manipulation. */
struct mint_image *mint_camera_image( void );

/** Get an image from the camera and paste it onto one (for grayscale
    images) or three (for color images) node groups. If either or both
    of ngreen and nblue are null, a grayscale version of the image is
    stored in nred. Otherwise, nred, ngreen, and nblue will contain
    the three RGB components of the image. The nodes are considered as
    rectangular arrays according to their rows (see
    mint_node_rows). */
void mint_camera_paste( mint_nodes nred, mint_nodes ngreen, mint_nodes
			nblue, int var, int xpos, int ypos );

/** Set node activation according to an image captured from
    camera. This function considers the nodes as being arranged in a
    rectangular arrays, according to their number of rows (see
    mint_node_rows).

 Parameters: 0: Which node variable will receive image data. As usual
             in MINT: 0: input, 1: output >=2: other state variable
             (default: 1, i.e., set node output directly.

             1,2,3: Index of the node groups that will receive the
             image. If only one index is given, the image is treated
             as grayscale; if three groups are given they will receive
             the R, G, and B components of the image. The three node
             groups must all have the same size. */
void mint_network_camera( struct mint_network *net, float *p );

#endif /* MINT_CAMERA_H */

