
#define _POSIX_C_SOURCE 199309L

#include "camera.h"
#include "utils.h"
#include "network.h"
#include "op.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <stdlib.h> /* system */
#include <unistd.h> /* fork, exec, unistd */
#include <sys/types.h> /* pid_t */

static pid_t mint_camera_pid = 0;  /* pid of camshot process */
static char mint_camera_pipe[256]; /* filename of camshot pipe */
static int mint_camera_lock = 0;   /* 1 thread only accesses camera */

static float mint_network_camera_default[4] = { -2, -2, -2, -2 };
static float mint_node_camera_default[1] = { 1 };

/* since this function is called through atexit(), errors can be
   non-fatal */
void mint_camera_close( void ) {
  char cmd[256];

  fprintf( stderr, "mint_camera_close: removing '%s'\n",
	   mint_camera_pipe );
  if( unlink( mint_camera_pipe ) == -1 )
    fprintf( stderr, 
	     "mint_camera_close: cannot remove '%s'\n", 
	     mint_camera_pipe );

  fprintf( stderr, "mint_camera_close: stopping camshot (pid=%u)\n",
	   (unsigned int)mint_camera_pid );
  sprintf( cmd, "kill -15 %u", (unsigned int)mint_camera_pid );
  if( system( cmd ) == -1 ) {
    fprintf( stderr, 
	     "mint_camera_close: cannot stop camshot, pid=%u\n", 
	     (unsigned int)mint_camera_pid );
  }

  mint_camera_pid = 0;
}

void mint_node_camera( mint_nodes n, float *p ) {
  int states;
  states = mint_nodes_states( n );
  mint_check( p[0]>=0 && p[0]<2+states,
	      "parameter 0 out of range 0-%d", 1+states )
}

/* this function initializes the camera by forking off an instance of
   camshot, set to write image data to a pipe. it aborts on failure */
void mint_camera_init( void ) {
  char *camarg[] = { CAMSHOT, "-p", 0, 0 };
  char cmd[256];
  pid_t pid = 0;
  int i;
  FILE *file;

  if( mint_camera_pid ) /* camera already set up */
    return;

  /* build  pipe filename to exchange data over with camshot */ 
  sprintf( mint_camera_pipe, "/tmp/mint-camera-%u", (unsigned int)getpid() );
  camarg[2] = mint_camera_pipe;

  pid = fork();
  mint_check( pid != -1, "cannot fork to create camshot process" );

  if( pid==0 ) { /* child */
    execv( "/usr/bin/camshot", camarg );
    mint_check( 0, "%s", strerror(errno) );
  }

  /* parent:  */
  mint_camera_pid = pid;
  atexit( mint_camera_close );

  /* wait until the camera is ready, signaled by being able to open
     the camera pipe. times out after 30s */
  i = 0;
  do {
    sleep( 1 );
    file = fopen( mint_camera_pipe, "r" );
    i++; 
  } while( !file && i<30 );
  mint_check( i < 30, "cannot open camshot pipe (%s)\n", mint_camera_pipe );

  /* we don't fclose( file ) because that would stop camshot, which is
     at the other end of the pipe */

  /* we grab an image because camshot has a bug: the first two
     images ever read from the pipe are identical! this also checks
     that we can read from the camera pipe. */
  sprintf( cmd, "cat %s > /dev/null", mint_camera_pipe );
  i = system( cmd );
  mint_check( i != -1, "cannot read from camshot pipe (%s)", 
	      mint_camera_pipe );
  
  /* now we register ops that make use of the camera */
  mint_op_add( "camera", mint_op_network_operate, mint_network_camera, 
	       4, mint_network_camera_default ); 
  mint_op_add( "red", mint_op_nodes_init, mint_node_camera, 1, 
	       mint_node_camera_default );
  mint_op_add( "green", mint_op_nodes_init, mint_node_camera, 1, 
	       mint_node_camera_default );
  mint_op_add( "blue", mint_op_nodes_init, mint_node_camera, 1, 
	       mint_node_camera_default );
  mint_op_add( "gray", mint_op_nodes_init, mint_node_camera, 1, 
	       mint_node_camera_default );
} 

struct mint_image *mint_camera_image( void ) {
  struct mint_image *img;
  char buf[256];
  char filename[256];
  struct timespec t, r;

  /* another thread might be reading from the camera */
  while( mint_camera_lock ) {
    t.tv_sec = 0;
    t.tv_nsec = 200000;
    nanosleep( &t, &r );
  }

  mint_camera_lock = 1;

  /* create image filename */
  sprintf( filename, "/tmp/mint-image-%u.bmp", (unsigned int)getpid() );

  /* create image file by cat'ing from camshot pipe */
  sprintf( buf, "cat %s > %s", mint_camera_pipe, filename );
  if( system( buf ) == -1 ) {
    fprintf( stderr, "mint_camera_image: cannot get image from camera\n" );
    abort();
  }

  img = mint_image_load( filename );

  /* remove image file. errors are non-fatal */
  if( unlink( filename ) == -1 ) {
    sprintf( buf, "mint_camera_image: cannot remove %s", filename );
    perror( buf );
  }

  mint_camera_lock = 0;

  return img;
}

void mint_network_camera( struct mint_network *net, float *p ) {
  int varR, varG, varB, varGR, R, G, B, GR;
  struct mint_image *img;
  struct mint_ops *ops;
  mint_nodes nred, ngreen, nblue, ngray;

  nred = ngreen = nblue = ngray = 0;
  varR = varG = varB = varGR = -1;

  /* these are run only the first time and store in the op parameters
     which nodes are involved */
  if( p[0] == -2 )
    R = p[0] = mint_network_nodes_find_op( net, "red" );
  if( p[1] == -2 )
    G = p[1] = mint_network_nodes_find_op( net, "green" );
  if( p[2] == -2 )
    B = p[2] = mint_network_nodes_find_op( net, "blue" );
  if( p[3] == -2 )
    GR = p[3] = mint_network_nodes_find_op( net, "gray" );

  /* now we can set node pointers and variable indices - null node
     pointers are handled OK by image_paste */
  if( R >= 0 ) {
    nred = mint_network_nodes( net, R );
    ops = mint_nodes_get_ops(nred);
    varR = mint_op_get_param( mint_ops_get_name(ops, "red"), 0 );
  }
  if( G >= 0 ) {
    ngreen = mint_network_nodes( net, G );
    ops = mint_nodes_get_ops(ngreen);
    varG = mint_op_get_param( mint_ops_get_name(ops, "green"), 0 );
  }
  if( B >= 0 ) {
    nblue = mint_network_nodes( net, B );
    ops = mint_nodes_get_ops(nblue);
    varB = mint_op_get_param( mint_ops_get_name(ops, "blue"), 0 );
  }
  if( GR >= 0 ) {
    ngray = mint_network_nodes( net, GR );
    ops = mint_nodes_get_ops(ngray);
    varGR = mint_op_get_param( mint_ops_get_name(ops, "gray"), 0 );
  }

  img = mint_camera_image();
  mint_image_paste( img, nred, ngreen, nblue, varR, varG, varB, 0, 0 );
  if( ngray )
    mint_image_paste_gray( img, ngray, varGR, 0, 0 );
  mint_image_del( img );
}
