#include "op.h"
#include "nop.h"
#include "wop.h"
#include "netop.h"
#include "utils.h"
#include "string.h"
#include "threads.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

struct mint_op {
  char *name;   /* op name: a unique identifier */
  int type;     /* see enum in op.h */
  void *op;     /* the op function */
  int nparam;   /* # op parameters */
  float *param; /* parameter values */
};

struct mint_ops {
  int n;               /* number of ops in the list */
  struct mint_op **p;  /* array of op pointers */
};

/* default parameter values for ops that need them */
static float node_logistic_param[] = { 0.1, 1, 0, 1 };
static float node_identity_param[] = { 0, 1 };
static float node_fastlogistic_param[] = { 0.1, 1, 0, 1 };
static float node_sigmoid_param[] = { 0.1, 1, 0, 1 };
static float node_integrator_param[] = { 1., 0., 0, 1 };
static float node_izzy_param[] = { 0.02, 0.2, -65., 8., 0, 1, 2, 3};
static float node_noise_param[] = { 0, 0.01, 0 };
static float node_bounded_param[] = { 0, 1, 0, 1 };
static float node_counter_param[] = { 1, 1, 2 };
static float node_spikes_param[] = { 5 };
static float node_size_param[] = { -1 };
static float node_rows_param[] = { -1 };
static float node_states_param[] = { 0 };
static float node_color_param[] = { 1, 1, 1, 1 };
static float node_habituation_param[] = { 1, 0, 2, 1 };
static float node_gradient_param[] = { 1, 0, 2, 0.01, 0 };
static float node_softmax_param[] = { 0, 1, 2, 1, 1 };

static float weights_hebbian_param[] = { 0., 0., 0., 0. };
static float weights_delta_param[] = { 0.05, 2 };
static float weights_stdp_param[] = { 0.05, .1, -.12, -.1, .1 };
static float weights_lateral_param[] = {0, 0, 0, 0};

static float weights_init_uniform_param[] = { 0., 1., 1. };
static float weights_init_normal_param[] = { 0., 0.01, 1. };
static float weights_init_diagonal_param[] = { 0. };
static float weights_init_target_param[] = { 0. };
static float weights_init_states_param[] = { 0 };
static float weights_init_cols_param[] = { -1 };
static float weights_init_rows_param[] = { -1 };
static float weights_init_normalize_param[] = { 1 };

static float network_init_threads_param[] = { 1, 0, 0 };
static float network_asynchronous_param[] = { 0 };
static float network_clocked_param[] = { 25, 0 };

/*  number of built-in ops, see mint_op_atexit. it is a #define
    and not a static variable otherwise it must be exposed in the
    header file.

    NOTE: change this whenever adding or removing from the table
    above, otherwise crashes can occur when adding ops! */
#define mint_nop_builtin 38

/* built-in ops */
static struct mint_op mint_op_static_table[] = {

  /* nodes update */

  { "logistic",mint_op_nodes_update,mint_node_logistic,4,
    node_logistic_param },

  { "fastlogistic",mint_op_nodes_update,mint_node_fastlogistic,4,
    node_fastlogistic_param },

  { "sigmoid",mint_op_nodes_update,mint_node_sigmoid,4,
    node_sigmoid_param },

  { "integrator",mint_op_nodes_update,mint_node_integrator,4,
    node_integrator_param },

  { "izzy",mint_op_nodes_update,mint_node_izzy,8,node_izzy_param },

  { "noise",mint_op_nodes_update,mint_node_noise,
    3,node_noise_param },

  { "bounded",mint_op_nodes_update,mint_node_bounded,
    4,node_bounded_param },

  { "counter",mint_op_nodes_update,mint_node_counter,
    3,node_counter_param },

  { "spikes",mint_op_nodes_update,mint_node_spikes,
    1,node_spikes_param },

  { "habituation",mint_op_nodes_update,mint_node_habituation,
    4,node_habituation_param },

  { "identity", mint_op_nodes_update,mint_node_identity,2,
    node_identity_param },

  { "gradient", mint_op_nodes_update,mint_node_gradient,5,
    node_gradient_param },

  { "softmax", mint_op_nodes_update,mint_node_softmax,5,
    node_softmax_param },

  /* nodes init */

  { "size", mint_op_nodes_init, mint_node_size, 1, node_size_param },

  { "rows", mint_op_nodes_init, mint_node_rows, 1, node_rows_param },

  { "states", mint_op_nodes_init, mint_node_states, 1, 
    node_states_param },

  { "color", mint_op_nodes_init, mint_node_color, 4, node_color_param },

  /* weights operate */

  { "mult",mint_op_weights_operate,mint_weights_mult,0,0 },

  /* weights update */

  { "hebbian",mint_op_weights_update,mint_weights_hebbian,4,
    weights_hebbian_param },

  { "delta",mint_op_weights_update,mint_weights_delta,2,
    weights_delta_param },

  { "stdp",mint_op_weights_update,mint_weights_stdp,5,
    weights_stdp_param },

  { "frozen", mint_op_weights_update,0,0,0 },


  /* weights init */

  { "rows", mint_op_weights_init, mint_weights_init_rows, 1, 
    weights_init_rows_param },

  { "cols", mint_op_weights_init, mint_weights_init_cols, 1, 
    weights_init_cols_param },

  { "states", mint_op_weights_init, mint_weights_init_states, 1, 
    weights_init_states_param },

  { "uniform",mint_op_weights_init,mint_weights_init_uniform,3,
    weights_init_uniform_param },

  { "normal",mint_op_weights_init,mint_weights_init_normal,3,
    weights_init_normal_param },

  { "diagonal",mint_op_weights_init,mint_weights_init_diagonal,1,
    weights_init_diagonal_param },

  { "target",mint_op_weights_init,mint_weights_init_target,1,
    weights_init_target_param },

  { "sparse",mint_op_weights_init,mint_weights_init_sparse,0,0 },

  {"normalize",mint_op_weights_init,mint_weights_init_normalize,1,
   weights_init_normalize_param },


  /* weights connect */

  { "lateral",mint_op_weights_connect,mint_weights_lateral,4,
    weights_lateral_param },


  /* network init */

  { "feedforward",mint_op_network_init,mint_network_init_feedforward,
    0,0 },

  { "synchronous",mint_op_network_init,mint_network_init_synchronous,
    0,0 },

  { "threads",mint_op_network_init,mint_network_init_threads,3,
    network_init_threads_param },

  /* network operate */

  { "asynchronous",mint_op_network_operate,mint_network_asynchronous,
    1,network_asynchronous_param },

  { "clocked", mint_op_network_operate,mint_network_clocked,2,
    network_clocked_param },

  { "spread", mint_op_network_operate, mint_network_spread, 0, 0 }

};

/* we store the address of the static table in a pointer to circumvent
   the fact that we cannot reallocate the static table (see
   mint_op_append). we need to reallocate if the user adds ops. */
static struct mint_op *mint_op_table = &mint_op_static_table[0];

/* current number of ops */
static int mint_nop = mint_nop_builtin;

/* frees storage used by user-defined op names and parameters */
static void mint_op_atexit( void ) {
  int i;
  if( mint_nop > mint_nop_builtin ) {
    for( i=mint_nop_builtin; i<mint_nop; i++ ) {
      free( mint_op_table[i].name );
      free( mint_op_table[i].param );
    }
    free( mint_op_table );
  }
}

/* looks up a rule id by name and type. returns mint_nop if not
   found */
static int mint_op_id( const char *name, int type ) {
  int id;
  mint_check( name != 0, "'name' is null" ); 
  for( id=0; id<mint_nop; id++ ) {
    if( type & mint_op_table[id].type &&
	strcmp( name,mint_op_table[id].name ) == 0 )
      return id;
  }
  return id;
}

int mint_op_exists( const char *name, int type ) {
  if( !name ) return 0;
  return mint_op_id(name, type) < mint_nop;
}

static struct mint_op *mint_op_alloc( const char *name, 
					      int nparam ) {
  struct mint_op *h = malloc( sizeof(struct mint_op) );
  h->name = malloc( strlen(name)+1 ); /* +1 for terminating null */
  if( nparam>0 ) h->param = malloc( nparam*sizeof(float) );
  else h->param = 0;
  h->nparam = nparam;
  return h;
}

struct mint_op *mint_op_new( const char *name, int type ) {
  struct mint_op *h, *hsrc;
  int i, len;
  i = mint_op_id( name, type );
  mint_check( i<mint_nop, "invalid op name: %s", name );
  hsrc = mint_op_table + i;
  h = mint_op_alloc( hsrc->name, hsrc->nparam );
  len = strlen( hsrc->name );
  strncpy( h->name, name, len );
  h->name[len] = 0; 
  h->nparam = hsrc->nparam;
  h->op = hsrc->op;
  h->type = hsrc->type;
  memcpy( h->param, hsrc->param, h->nparam * sizeof(float) );
  return h;
}

void mint_op_del( struct mint_op *h ) {
  mint_check( h!=0, "attempt to delete null op" );
  free( h->name );
  if( h->nparam>0 ) free( h->param );
  free( h );
}

struct mint_op *mint_op_dup( const struct mint_op *h1 ) {
  struct mint_op *h2;
  mint_check( h1!=0, "attempt to duplicate null op" );
  h2 = mint_op_new( h1->name, h1->type );
  memcpy( h2->param, h1->param, h2->nparam * sizeof(float) );
  return h2;
}

void mint_op_save( const struct mint_op *u, FILE *dest ) {
  int i;
  mint_check( u!=0, "attempt to save null op" );
  fprintf( dest, "  %s ", u->name );
  for( i=0; i<u->nparam; i++ ) 
    fprintf( dest, "%g ", u->param[i] );
  fprintf( dest, "\n" );
}

struct mint_op *mint_op_load( FILE *file, int type ) {
  int i, j;
  long pos;
  struct mint_op *h;
  mint_string name;

  pos = ftell( file );
  name = mint_string_load( file );

  if( !name ) 
    return 0;

  if( mint_keyword( name ) ) {
    fseek( file, pos, SEEK_SET );
    return 0;
  }

  h = mint_op_new( name, type );

  /* load parameters (missing ones already got a default value */
  j = 0;
  while( mint_values_waiting(file) && j < h->nparam ) {
      i = fscanf( file, " %f", h->param + j );
      mint_check( i==1, "can't load op parameter %d", j );
      j++;
  }
  return h;
}

const char *mint_op_name( const struct mint_op *h ) {
  return h->name;
}

int mint_op_name_is( const struct mint_op *self, const char *name ) {
  return strncmp( self->name, name, strlen(self->name) ) == 0;
}

int mint_op_type( const struct mint_op *h ) {
  return h->type;
}

int mint_op_nparam( const struct mint_op *h ) {
  return h->nparam;
}

float *mint_op_get_params( struct mint_op *h ) {
  return h->param;
}

float mint_op_get_param( struct mint_op *h, int i ) {
  mint_check( i>=0 && i<h->nparam, "index out of range" );
  return h->param[i];
}

void mint_op_set_param( struct mint_op *h, int i, float x ) {
  mint_check( i>=0 && i<h->nparam, "index out of range" );
  h->param[i] = x;
}

/* this internal function avoids duplication in following functions */
void mint_op_add( const char *name, int type, void *f, 
		  int nparam, float *param ) {
  int i, len;
  struct mint_op *h;
  /* first time this function is called mint_op_table can't be realloc'd */  
  if( mint_nop == mint_nop_builtin ) {
    mint_op_table = malloc( (mint_nop+1)*sizeof(struct mint_op) );
    memcpy( mint_op_table, mint_op_static_table, 
	    mint_nop*sizeof(struct mint_op) );
  } else {
    mint_op_table = realloc( mint_op_table, 
			     (mint_nop+1)*sizeof(struct mint_op) );
  }
  h = mint_op_table + mint_nop;
  len = strlen(name);
  h->name = malloc( len+1 ); /* +1 for terminating null */
  strncpy( h->name, name, len+1 );
  h->name[len]=0;
  h->type = type;
  h->op = f;
  h->nparam = nparam;
  h->param = malloc( nparam*sizeof(float) );
  for( i=0; i<nparam; i++ ) h->param[i] = param[i];
  /* register deallocation routine, only 1st time a rule is added */
  if( mint_nop == mint_nop_builtin ) 
    atexit( mint_op_atexit );
  mint_nop++;
}

void mint_op_run( struct mint_op *op, ... ) {
  mint_nodes n1, n2;
  mint_weights w;
  struct mint_network *net;
  int min, max;
  va_list ap;
#ifdef MINT_IMAGE
  SDL_Event event;
#endif

  va_start( ap, op );

  switch( op->type ) {

  case mint_op_nodes_update:
  case mint_op_nodes_init:
    n1 = va_arg( ap, mint_nodes );
    min = va_arg( ap, int );
    max = va_arg( ap, int );
    ( (mint_nop_t)op->op )( n1, min, max, op->param );
    break;

#ifdef MINT_IMAGE
  case mint_op_nodes_event:
    n1 = va_arg( ap, mint_nodes );
    min = va_arg( ap, int );
    max = va_arg( ap, int );
    event = va_arg( ap, SDL_Event );
    ( (mint_nop_ev_t)op->op )( n1, min, max, op->param, event );
    break;
#endif

  case mint_op_weights_update:
  case mint_op_weights_operate:
  case mint_op_weights_connect:
    w = va_arg( ap, mint_weights );
    n1 = va_arg( ap, mint_nodes );
    n2 = va_arg( ap, mint_nodes );
    min = va_arg( ap, int );
    max = va_arg( ap, int );
    ( (mint_wop_t)op->op )( w, n1, n2, min, max, op->param );
    break;

  case mint_op_weights_init:
    w = va_arg( ap, mint_weights );
    min = va_arg( ap, int );
    max = va_arg( ap, int );
    ( (mint_winit_t)op->op )( w, min, max, op->param );
    break;
    
  case mint_op_network_init:
  case mint_op_network_operate:
    net = va_arg( ap, struct mint_network * );
    ( (mint_netop_t)op->op )( net, op->param );
    break;

  }

  va_end( ap );
}

struct mint_ops *mint_ops_new( void ) {
  struct mint_ops *ops;
  ops = malloc( sizeof(struct mint_ops) );
  ops->p = malloc( 0 );
  ops->n = 0;
  return ops;
}

void mint_ops_append( struct mint_ops *ops, struct mint_op *op ) {
  mint_check( ops!=0, "cannot append to null ops list" );
  ops->p = realloc( ops->p, ++ops->n * sizeof(struct mint_op *) );
  ops->p[ ops->n - 1 ] = mint_op_dup( op );
}

void mint_ops_del( struct mint_ops *ops ) {
  int i;
  mint_check( ops!=0, "attempt to delete null ops list" );
  for( i=0; i< ops->n; i++ )
    mint_op_del( ops->p[i] );
  free( ops->p );
  free( ops );
}

struct mint_ops *mint_ops_dup( const struct mint_ops *src ) {
  int i;
  struct mint_ops *dst;
  mint_check( src!=0, "attempt to duplicate null ops list" );
  dst = malloc( sizeof(struct mint_ops) );
  dst->p = malloc( src->n * sizeof(struct mint_op *) );
  for( i=0; i<src->n; i++ )
    dst->p[i] = mint_op_dup( src->p[i] );
  dst->n = src->n;
  return dst;
}

void mint_ops_save( const struct mint_ops *ops, FILE *dest ) {
  int i;
  mint_check( ops!=0, "attempt to save null ops list" );
  for( i=0; i<ops->n; i++ ) {
    if( !mint_default( ops->p[i]->name ) )
      mint_op_save( ops->p[i], dest );
  }
}

struct mint_ops *mint_ops_load( FILE *file, int type ) {
  struct mint_ops *ops;
  struct mint_op *op;
  ops = mint_ops_new();
  while( (op = mint_op_load(file, type)) ) {
    mint_ops_append( ops, op ); 
    mint_op_del( op );
  }
  return ops;
}

int mint_ops_size( const struct mint_ops *ops ) {
  return ops->n;
}

struct mint_op *mint_ops_get( struct mint_ops *ops, int i ) {
  mint_check( ops!=0, "null ops object" );
  mint_check( i>=0 && i<ops->n, "index i out of range" );
  return ops->p[i];
}

void mint_ops_set( struct mint_ops *ops, int i, struct mint_op *op ) {
  mint_check( ops!=0, "null ops object" );
  mint_check( i>=0 && i<ops->n, "index i out of range" );
  mint_op_del( ops->p[i] );
  ops->p[i] = mint_op_dup( op );
}

int mint_ops_del_type( struct mint_ops *ops, int type ) {
  int i, k;
  struct mint_op **pnew;
  mint_check( ops!=0, "null ops object" );
  pnew = malloc( ops->n * sizeof(struct mint_op *) );
  k = 0;
  for( i=0; i<ops->n; i++ ) {
    if( ops->p[i]->type != type )
      pnew[ k++ ] = ops->p[i];
    else
      mint_op_del( ops->p[i] );
  }
  pnew = realloc( pnew, k * sizeof(struct mint_op *) );
  ops->n = k;
  free( ops->p );
  ops->p = pnew;
  return k;
}

int mint_ops_del_name( struct mint_ops *ops, const char *name ) {
  int i, k;
  struct mint_op **pnew;
  mint_check( ops!=0, "null ops object" );
  pnew = malloc( ops->n * sizeof(struct mint_op *) );
  k = 0;
  for( i=0; i<ops->n; i++ ) {
    if( strncmp( ops->p[i]->name, name, strlen( ops->p[i]->name ) ) != 0 )
      pnew[ k++ ] = ops->p[i];
    else
      mint_op_del( ops->p[i] );
  }
  pnew = realloc( pnew, k * sizeof(struct mint_op *) );
  ops->n = k;
  free( ops->p );
  ops->p = pnew;
  return k;
}

struct mint_op *mint_ops_get_name( struct mint_ops *ops, 
				   const char *name,
				   int type ) {
  int i;
  i = mint_ops_find( ops, name, type );
  return i>=0 ? ops->p[i] : 0;
}

int mint_ops_count( struct mint_ops *ops, int type ) {
  int i, count;
  mint_check( ops!=0, "null ops object" );
  count = 0;
  for( i=0; i<ops->n; i++ ) 
    count += ops->p[i]->type & type;
  return count;
}

int mint_ops_find( struct mint_ops *ops, const char *name, 
		   int type ) {
  int i;
  const char *opname;
  mint_check( ops!=0, "null ops object" );
  for( i=0; i<ops->n; i++ ) {
    opname = mint_op_name( ops->p[i] );
    if( ops->p[i]->type & type 
	&&
	strncmp( opname, name, strlen(opname) ) == 0 )
      return i;
  }
  return -1;
}

