#include "pi.h"
#include "utils.h"
#include "op.h"

#ifdef MINT_PI
#include <pigpio.h>
#else
#include <stdint.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*** default parameters for mint_pi_* functions ***/

/* the second and third defaults ensure that the servo is not damaged
   by excessive rotation. the user is expected to set them */
static float mint_pi_servomotor_default[] = {-1, 1500, 100, 0, -1, -1, 0};

/* the last parameter (undocumented in the user interface) is for
   storing the init status */
static float mint_pi_dcmotor_default[] = { -1, -1, -1, 0.5, 0.1, 0, 0 };

static float mint_pi_gpiosensor_default[] = { -1, 1, 0, 2, 0 };

/*** end default parameters ***/

static unsigned int mint_pi_gpio_used = 0;
static int mint_pi = 0;

void mint_pi_close( void ) {
  int i; 

  if( !mint_pi ) 
    return;

#ifdef MINT_PI
  /* set all GPIOs used as output to 0 to avoid leaving stuff running */
  for( i = 0; i<31; i++ ) {
    if( mint_pi_gpio_used & (1 << i) )
      gpioWrite( i, 0 );
  }
  gpioTerminate();
#else
  MINT_UNUSED( i );
#endif

  mint_pi = 0;
  mint_pi_gpio_used = 0;
}

void mint_pi_init( void ) {
  int i;

  if( mint_pi ) 
    return;

#ifdef MINT_PI
  i = gpioInitialise();
  mint_check( i>=0, "cannot initialise pigpio library" );
#else
  MINT_UNUSED( i );
#endif

  mint_pi = 1;
  atexit( mint_pi_close );

  /* now register node ops with mint */

  mint_op_add( "servomotor", mint_op_nodes_update, 
	       mint_pi_servomotor, 7, mint_pi_servomotor_default );

  mint_op_add( "dcmotor", mint_op_nodes_update, 
	       mint_pi_dcmotor, 7, mint_pi_dcmotor_default );

  mint_op_add( "gpiosensor", mint_op_nodes_update, 
	       mint_pi_gpiosensor, 5, mint_pi_gpiosensor_default );

}

void mint_pi_servomotor( mint_nodes n, int min, int max, float *p ) {
  int i, size;
  float activity;
  int control_pin, init_done, set_mode, enable_pin, output_pin,
    rot_min, rot_max;

  MINT_UNUSED( min );
  MINT_UNUSED( max );

  control_pin = p[0];
  rot_min = p[1] - p[2];
  rot_max = p[1] + p[2];
  set_mode = p[3];
  enable_pin = p[4];
  output_pin = p[5];
  init_done = p[6];

  mint_check( control_pin != -1, "enable pin not set (1st op parameter)" );

  if( !init_done || set_mode ) {
#ifdef MINT_PI
    /* make sure pin mode is output */
    i = gpioSetMode( control_pin, PI_OUTPUT );
    mint_check( i==0, "cannot set enable pin mode to output" );
#else
    i = 0;
#endif

    if( enable_pin != -1 && output_pin != -1 ) {
      mint_pi_gpio_used |= 1 << enable_pin;
      mint_pi_gpio_used |= 1 << output_pin;
#ifdef MINT_PI
      i = gpioSetMode( enable_pin, PI_OUTPUT );
      mint_check( i==0, "cannot set enable pin mode to output" );
      i = gpioSetMode( output_pin, PI_OUTPUT );
      mint_check( i==0, "cannot set output pin mode to output" );
#else
      i = 0;
#endif
    }

    p[6] = 1; /* init_done */
  }

  /* calculate average activity: we disregard the min and max
     arguments to mint_pi_servomotor because servo motor activation
     should always reflect the activity of the whole node group, even
     if here we are asked to update only one node.  */
  size = mint_nodes_size( n );
  activity = 0;
  for( i=0; i<size; i++ )
    activity += n[1][i];
  activity /= size;

  /* remap activity from [0,1] to [ rot_mint, rot_max ] */
  activity = (rot_max - rot_min) * activity + rot_min;

  /* enable motor output */ 
  if( enable_pin != -1 && output_pin != -1 ) {
#ifdef MINT_PI
    i = gpioWrite( enable_pin, 1 );
    mint_check( i==0, "cannot set enable pin" ); 
    i = gpioWrite( output_pin, 1 );
    mint_check( i==0, "cannot set output pin" ); 
#else
    MINT_UNUSED( i );
#endif
  }

  /* send  servo pulses */ 
#ifdef MINT_PI
  i = gpioServo( control_pin, activity );
  mint_check( i==0, "cannot set control pin" ); 
#else
  MINT_UNUSED( i );
#endif
}

void mint_pi_dcmotor( mint_nodes n, int min, int max, float *p ) {
  int i, size;
  float activity;
  int enable_pin, output_pin1, output_pin2;
  int init_done, set_mode;
  float zero_point, threshold;

  MINT_UNUSED( min );
  MINT_UNUSED( max );

  enable_pin = p[0];
  output_pin1 = p[1];
  output_pin2 = p[2];
  zero_point = p[3];
  threshold = p[4];
  init_done = p[5];
  set_mode = p[6];

  /* pin initialization is performed only once, unless user asks
     otherwise */
  if( set_mode || !init_done ) {

    mint_check( enable_pin != -1, "enable pin not set (1st op parameter)" );
    mint_check( output_pin1 != -1, "output pin 1 not set (2nd op parameter)" );
    mint_check( output_pin2 != -1, "output pin 1 not set (2nd op parameter)" );

    /* register these pins as being used for output */
    mint_pi_gpio_used |= 1 << enable_pin;
    mint_pi_gpio_used |= 1 << output_pin1;
    mint_pi_gpio_used |= 1 << output_pin2;

#ifdef MINT_PI
    /* make sure pin mode is output */
    i = gpioSetMode( enable_pin, PI_OUTPUT );
    mint_check( i==0, "cannot set enable pin to output" );
    i = gpioSetMode( output_pin1, PI_OUTPUT );
    mint_check( i==0, "cannot set output pin 1 mode to output" );
    i = gpioSetMode( output_pin2, PI_OUTPUT );
    mint_check( i==0, "cannot set output pin 2 mode to output" );
#else
    i = 0;
#endif

    p[5] = 1; /* initialization done */
  }

  /* calculate average activity (see mint_pi_servo) */
  size = mint_nodes_size( n );
  activity = 0;
  for( i=0; i<size; i++ )
    activity += n[1][i];
  activity /= size;

  if( fabs( activity - zero_point ) < threshold )
    return;

  /* if activity is below the zero point, we turn it around so that it
     is above it by the same amount, and we swap the output pins to
     drive the motor in reverse */
  if( activity<zero_point ) {
    activity = zero_point + (zero_point - activity);
    i = output_pin2;
    output_pin2 = output_pin1;
    output_pin1 = i;
  }

  /* scale activity from [zero_point+threshold, 1] to [0, 1] */
  activity -= zero_point + threshold;
  activity /= 1 - zero_point - threshold;

  /* scale activity from [0, 1] to the pin range */
#ifdef MINT_PI
  i = gpioGetPWMrange( output_pin1 );
  mint_check( i != PI_BAD_USER_GPIO, "cannot get output pin range" );
#else
  i = 1;
#endif
  activity *= i; 

#ifdef MINT_PI
  /* turn off one pin, enable motor output, turn on the other pin */
  i = gpioWrite( output_pin2, 0 );
  mint_check( i==0, "cannot turn off output pin" );
  i = gpioWrite( enable_pin, 1 );
  mint_check( i==0, "cannot turn on enable pin" );
  i = gpioPWM( output_pin1, activity );
  mint_check( i==0, "cannot turn on output pin" );
#else
  i = 0;
#endif
}

/* getting input from GPIOs is a bit harder because they can be
   triggered at any time, even when the network is not being
   updated. what we do is to set up a callback (courtesy pigpio
   library) to invoke a function everytime the GPIO level changes. the
   function translates the value read on the GPIO into input to
   nodes. */
 
void mint_pi_gpiosensor_callback( int gpio, int level, 
				  uint32_t tick, void *userdata ) {
  mint_nodes n;
  struct mint_ops *ops;
  struct mint_op *op;
  int i, size, storevar;
  float increment, *store;

  MINT_UNUSED( gpio );
  MINT_UNUSED( tick );

  if( level==0 ) /* nothing to do */
    return;

  /* get nodes object and retrieve increment param from gpiosensor op */
  n = (mint_nodes) userdata;
  ops = mint_nodes_get_ops( n );
  i = mint_ops_find( ops, "gpiosensor", mint_op_nodes_any );
  mint_check( i>-1, "no gpiosensor op in nodes" );
  op = mint_ops_get( ops, i );

  increment = mint_op_get_param( op, 1 );
  storevar = mint_op_get_param( op, 3 );
  store = n[ storevar ];

  size = mint_nodes_size( n );
  for( i=0; i<size; i++ )
    store[i] += increment;
}

/* here we first arrange for pigpio to call the function above, which
   stores GPIO level values into a state variable, then we copy these
   values as input (or other configured variable) when called. the
   first step is performed only once (ensured by storing a flag value
   into param 4 of the op). */
void mint_pi_gpiosensor( mint_nodes n, int min, int max, float *p ) {
  int input_pin, i;
  float *store, *to;

  if( ! (int)p[4] ) { /* we still have to set things up */
    input_pin = p[0];
    mint_check( input_pin!=-1, "input_pin not set (1st op parameter)" );

#ifdef MINT_PI
    i = gpioSetMode( input_pin, PI_INPUT );
    mint_check( i==0, "cannot set read mode on input_pin" );

    i = gpioSetPullUpDown( input_pin, PI_PUD_DOWN );
    mint_check( i==0, "cannot set pull down resistor on input_pin" );

    gpioSetAlertFuncEx( input_pin, mint_pi_gpiosensor_callback, 
			(void *)n );
#else
    i = 0;
#endif

    p[4] = 1; /* setup done */
  }

  store = n[ (int)p[3] ]; /* the state variable accruing changes */
  to = n[ (int)p[2] ];   

  for( i=min; i<max; i++ ) {
    to[i] += store[i];
    store[i] = 0;
  }
}

