#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE

/* the bird's eye view of camera operation is as follows. in
   mint_camera_init we start camshot, setting it up to write image
   data to shared memory, and then we register the mint_network_camera
   op . any time the letter runs, an image is uploaded to shared
   memory, then */   

#include "camera.h"
#include "network.h"
#include "op.h"
#include "utils.h"
#include "nop.h"

#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include <unistd.h> /* fork, exec */
#include <sys/types.h> /* pid_t */
#include <sys/shm.h> /* shared memory */

#include "../camshot/shmem.h" /* semaphore utilities from from camshot */

#ifdef __STDC_VERSION__ 
#if __STDC_VERSION__ >= 199901L
#include <stdint.h>
#else
typedef unsigned char uint8_t;
#endif
#else
typedef unsigned char uint8_t;
#endif

static pid_t mint_camera_pid = 0; /* pid of camshot process */
static int mint_camera_lock = 0;  /* 1 thread only accesses camera */
static int mint_shared_memory_key = 1972;
char *mint_shared_memory_key_string = "1972";

/* since this function is called through atexit(), errors can be
   non-fatal */
void mint_camera_close( void ) {
  char cmd[256];

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

static float network_camera_param[] = {0, 0};

/* this function initializes the camera by forking off an instance of
   camshot, set to write image data to shared memory. it aborts on
   failure */
void mint_camera_init( void ) {
  mint_op_add( "camera", mint_op_network_operate, 
	       mint_network_camera, 2, network_camera_param ); 
} 

void mint_camera_start( int width, int height ) {
  char *camarg[] = { CAMSHOT, "-f", "rgb", "-s", mint_shared_memory_key_string, 
		     "-W", 0, "-H", 0, 0 };
  int len;

  if( mint_camera_pid ) /* camera already set up */
    return;

  mint_check( width>0, "requested image width is negative" );
  mint_check( height>0, "requested image height is negative" );

  len = floor( log10( width ) ) + 2;
  camarg[6] = malloc( len );
  sprintf( camarg[6], "%d", width );
  
  len = floor( log10( height ) ) + 2;
  camarg[8] = malloc( len );
  sprintf( camarg[8], "%d", height );

  mint_camera_pid = fork();
  mint_check( mint_camera_pid != -1, 
	      "cannot fork to create camshot process" );

  if( mint_camera_pid==0 ) { /* child */
    execv( CAMSHOT, camarg );
    mint_check( 0, "%s", strerror(errno) );
  } else {                   /* parent */
    atexit( mint_camera_close );
    mint_op_add( "camera", mint_op_network_operate, 
		 mint_network_camera, 0, 0 ); 
    free( camarg[6] );
    free( camarg[8] );
    sleep( 2 );
  }
} 

void mint_network_camera( struct mint_network *net, float *p ) {
  int groups, shared_memory_id, semaphore;
  int width, height, rows, cols, var; 
  int i, k;
  int xorig, yorig;
  int x, y;
  mint_nodes n;
  struct mint_ops *node_ops;
  struct mint_op *op;
  float r, g, b;
  uint8_t *img, *pixel;
  struct timespec t1, t2;

  if( ! mint_camera_pid )
    mint_camera_start( p[0], p[1] );

  /* another thread might be reading from the camera */
  while( mint_camera_lock ) {
    t1.tv_sec = 0;
    t1.tv_nsec = 100000;
    nanosleep( &t1, &t2 );
  }
  mint_camera_lock = 1;

  width = p[0];
  height = p[1];

  /* access camshot shared memory */
  shared_memory_id = shmget( mint_shared_memory_key,  width*height*3,
			     0666 );
  mint_check( shared_memory_id>=0, "cannot get shared memory id" );
  img = (uint8_t *)shmat( shared_memory_id, 0, 0 );
  mint_check( p != (void *) -1, "cannot get shared memory pointer" );

  /* init semaphore and lock memory access */
  semaphore = semget( (key_t)mint_shared_memory_key, 1, 0666 );
  sem_set( &semaphore );
  sem_down( &semaphore ); 

  /* at this point, img points to a uint8_t array of size
     width*height*3. we have to paste it onto our node array,
     considered as having dimensions rows and cols which could be
     smaller than height and width. */

  /* now we loop over all node groups, copying the image onto nodes
     according to user requests */
  groups = mint_network_groups( net );
  for( i=0; i<groups; i++ ) {

    n = mint_network_get_nodes( net, i );    
    node_ops = mint_nodes_get_ops( n );    
    op = mint_ops_get_name( node_ops, "color", mint_op_nodes_init );

    if( op ) { /* this node accepts image input */

      /* get target variable and rgb weights */
      r   = mint_op_get_param( op, 0 );
      g   = mint_op_get_param( op, 1 );
      b   = mint_op_get_param( op, 2 );
      var = mint_op_get_param( op, 3 );

      /* figure out rows and columns */
      op = mint_ops_get_name( node_ops, "rows", mint_op_nodes_init );
      mint_check( op, "cannot determine node rows" );
      rows = mint_op_get_param( op, 0 );
      cols = mint_nodes_size( n ) / rows;
      mint_check( rows <= height, 
		  "node array has more rows than camera image" );
      mint_check( cols <= width, 
		  "node array has more columns than camera image" );

      mint_nodes_set( n, var, 0 ); /* set node variable to zero */

      pixel = img;
      for( yorig=0; yorig<height; yorig++ ) {
	for( xorig=0; xorig<width; xorig++ ) {
	  x = xorig * ((float)cols / width);
	  y = yorig * ((float)rows / height);
	  k = x + y * cols;
	  n[var][k] += r * pixel[0];
	  n[var][k] += g * pixel[1];
	  n[var][k] += b * pixel[2];
	  n[var][k] /= 3*256;
	  pixel += 3;
	}
      }
    }
  }
  sem_up( &semaphore );
  mint_camera_lock = 0;
}
