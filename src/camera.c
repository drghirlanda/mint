/* needed to access nanosleep() in gcc */
#define _POSIX_C_SOURCE 199309L

#include "camera.h"
#include "utils.h"

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

/* since this function is called through atexit(), errors can be
   non-fatal */
void mint_camera_close( void ) {
  char cmd[256];

  if( unlink( mint_camera_pipe ) == -1 )
    fprintf( stderr, 
	     "mint_camera_close: cannot remove '%s'\n", 
	     mint_camera_pipe );

  sprintf( cmd, "kill -15 %u", (unsigned int)mint_camera_pid );
  if( system( cmd ) == -1 ) {
    fprintf( stderr, 
	     "mint_camera_close: cannot stop camshot, pid=%u\n", 
	     (unsigned int)mint_camera_pid );
  }

  mint_camera_pid = 0;
}

/* this function initializes the camera by forking off an instance of
   camshot, set to write image data to a pipe. it aborts on failure */
void mint_camera_init( void ) {
  char *camarg[] = { CAMSHOT, "-p", 0, 0 };
  char cmd[256];
  pid_t pid = 0;
  
  if( mint_camera_pid ) /* camera already set up */
    return;

  /* build  pipe filename to exchange data over with camshot */ 
  sprintf( mint_camera_pipe, "/tmp/mint-camera-%u", (unsigned int)getpid() );
  camarg[2] = mint_camera_pipe;

  pid = fork();

  if( pid == -1 ) { /* parent, failed fork */
    fprintf( stderr, "cannot fork to create camshot process" );
    abort();
  } else if( pid==0 ) { /* child */
#ifndef NDEBUG
    fprintf( stderr, "mint_camera_init: starting camera with pid=%u\n",
	     (unsigned int) getpid() );
#endif
    execv( "/usr/bin/camshot", camarg );
    fprintf( stderr, "mint_camera_init: %s\n", strerror(errno) );
    abort();
  } else { /* parent, successful fork: wait for camera to start */
#ifndef NDEBUG
    fprintf( stderr, "camera master pid=%u\n waiting 5s for pid=%u to start camera\n", getpid(), pid );
#endif
    mint_camera_pid = pid;
    atexit( mint_camera_close );

    /* we grab an image because camshot has a bug: the first two
       images ever read from the pipe are identical! this also serves
       to wait until the camera is ready. */
    sprintf( cmd, "cat %s > /dev/null", mint_camera_pipe );
    if( system( cmd ) == -1 ) {
      fprintf( stderr, "mint_camera_init: cannot read from '%s'\n", 
	       mint_camera_pipe );
      abort();
    }
  }
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

  mint_camera_init(); /* does nothing if camera already set up */

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

void mint_camera_paste( mint_nodes nred, mint_nodes ngreen, mint_nodes nblue,
			int var, int nrows, int xpos, int ypos ) {
  struct mint_image *img;
  img = mint_camera_image();
  mint_image_paste( img, nred, ngreen, nblue, var, nrows, xpos, ypos );
  mint_image_del( img );
}

