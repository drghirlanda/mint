#ifndef MINT_CAMERA_H
#define MINT_CAMERA_H

#include "nodes.h"
#include "image.h"

/** \file camera.h

    \brief Functions to capture images from a camera and to paste them
    onto node groups.

    The camera is initialized the first time one of these functions is
    called. The initialization may take a few seconds. 

    The image capture functionality is provided by camshot, see
    \url{https://code.google.com/p/camshot}.

*/

/** Get an image from the camera. See the image functions (image.h)
    for information about image manipulation. */
struct mint_image *mint_camera_image( void );

/** Get an image from the camera and paste it onto one (for grayscale
    images) or three (for color images) node groups. If either or both
    of ngreen and nblue are null, a grayscale version of the image is
    stored in nred. Otherwise, nred, ngreen, and nblue will contain
    the three RGB components of the image. */
void mint_camera_paste( mint_nodes nred, mint_nodes ngreen, mint_nodes
			nblue, int var, int nrows, int xpos, int ypos );

#endif /* MINT_CAMERA_H */

