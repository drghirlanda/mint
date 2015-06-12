#ifndef MINT_CAMERA_H
#define MINT_CAMERA_H

#include "nodes.h"

struct mint_network;

/** \file camera.h

    \brief Functions to capture images from a camera and paste them
    onto node groups.
*/

/** Initialize the camera. This function must be called before any
    access to the camera is attempted from within MINT (including
    loading an architecture file that uses the "camera" directive). */
void mint_camera_init( void );

/** Set node activation according to an image captured from
    camera. This function considers the nodes as being arranged in a
    rectangular array, according to their number of rows - see
    mint_node_rows(). The image is scaled to fit as closely as
    possible to this geometry (it will not fit perfectly if the aspect
    ratios differ, meaning that some nodes will not receive any input
    from the camera). A node group may be set up to receive any linear
    combination of the red, green, or blue components. See the
    documentation for mint_node_color().

    State variables: none.

    Parameters: 

    0: image width
    1: image height

*/
void mint_network_camera( struct mint_network *net, float *p );

#endif /* MINT_CAMERA_H */

