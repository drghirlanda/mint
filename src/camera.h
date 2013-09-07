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

/** Get an image from the camera. See the image functions in file
    image.h for information about image manipulation. */
struct mint_image *mint_camera_image( void );

/** Set node activation according to an image captured from
    camera. This function considers the nodes as being arranged in a
    rectangular array, according to their number of rows - see
    mint_node_rows(). The image is scaled to fit as closely as
    possible to this geometry (it will not fit perfectly if the aspect
    ratios differ, meaning that some nodes will not receive any input
    from the camera). A node group may be set up to receive all image
    components (thus, a grayscale image) or one of the red, green, or
    blue components of the RGB color system. Nodes are labeled with
    the ops "gray", "red", "green", "blue" to specify which component
    they receive. For example "nodes n1 size 10 red" means that the
    node will receive the red component. The node groups need not have
    the same geometry, and any number of node groups may be
    given. Furthermore, a node group may receive multiple components,
    as in "nodes n1 size 10 red blue". See the documentation for
    mint_node_color() for additional options. 

    Parameters: none. */
void mint_network_camera( struct mint_network *net, float *p );

#endif /* MINT_CAMERA_H */

