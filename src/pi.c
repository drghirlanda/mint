#include "pi.h"
#include "utils.h"
#include "op.h"

#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*** default parameters for mint_pi_* functions ***/

/* the second and third defaults ensure that the servo is not damaged
   by excessive rotation. the user is expected to set them */
static float mint_pi_servomotor_default[4] = {-1, 1500, 1500, -1};

static float mint_pi_dcmotor_default[5] = { -1, -1, -1, 0.5, 0.1 };

static float mint_pi_gpiosensor_default[2] = { -1, 1 };

/*** end default parameters ***/

static unsigned int mint_pi_gpio_used = 0;
static int mint_pi = 0;

void mint_pi_close( void ) {
  int i; 

  if( !mint_pi ) 
    return;

  /* set all GPIOs used as output to 0 to avoid leaving stuff running */
  for( i = 0; i<31; i++ ) {
    if( mint_pi_gpio_used & (1 << i) )
      gpioWrite( i, 0 );
  }
  gpioTerminate();
  mint_pi = 0;
  mint_pi_gpio_used = 0;
}

void mint_pi_init( void ) {
  int i;

  if( mint_pi ) 
    return;

  i = gpioInitialise();
  mint_check( i>=0, "cannot initialise pigpio library" );

  mint_pi = 1;
  atexit( mint_pi_close );

  /* now register node ops with mint */

  mint_op_add( "servomotor", mint_op_nodes_update, 
	       mint_pi_servomotor, 4, mint_pi_servomotor_default );

  mint_op_add( "dcmotor", mint_op_nodes_update, 
	       mint_pi_dcmotor, 5, mint_pi_dcmotor_default );

  mint_op_add( "gpiosensor", mint_op_nodes_update, 
	       mint_pi_gpiosensor, 2, mint_pi_gpiosensor_default );

}

void mint_pi_servomotor( mint_nodes n, int min, int max, float *p ) {
  int i, size;
  float activity;
  int enable_pin, output_pin, rot_min, rot_max;

  enable_pin = p[0];
  output_pin = p[1];
  rot_min = p[2];
  rot_max = p[3];

  mint_check( enable_pin != -1, "enable pin not set (1st op parameter)" );
  mint_check( output_pin != -1, "output pin not set (2nd op parameter)" );

  mint_pi_gpio_used |= 1 << enable_pin;
  mint_pi_gpio_used |= 1 << output_pin;

  /* make sure pin mode is output */
  i = gpioSetMode( enable_pin, PI_OUTPUT );
  mint_check( i==0, "cannot set enable pin mode to output" );
  i = gpioSetMode( output_pin, PI_OUTPUT );
  mint_check( i==0, "cannot set output pin mode to output" );


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

  /* enable motor output and send servo pulse */ 
  i = gpioWrite( enable_pin, 1 );
  mint_check( i==0, "cannot set enable pin" ); 
  i = gpioServo( output_pin, activity );
  mint_check( i==0, "cannot set output pin" ); 
}


void mint_pi_dcmotor( mint_nodes n, int min, int max, float *p ) {
  int i, size;
  float activity;
  int enable_pin, output_pin1, output_pin2;
  float zero_point, threshold;

  enable_pin = p[0];
  output_pin1 = p[1];
  output_pin2 = p[2];
  zero_point = p[3];
  threshold = p[4];

  mint_check( enable_pin != -1, "enable pin not set (1st op parameter)" );
  mint_check( output_pin1 != -1, "output pin 1 not set (2nd op parameter)" );
  mint_check( output_pin2 != -1, "output pin 1 not set (2nd op parameter)" );

  /* calculate average activity (see mint_pi_servo) */
  size = mint_nodes_size( n );
  activity = 0;
  for( i=0; i<size; i++ )
    activity += n[1][i];
  activity /= size;

  if( fabs( activity - zero_point ) < threshold )
    return;

  mint_pi_gpio_used |= 1 << enable_pin;
  mint_pi_gpio_used |= 1 << output_pin1;
  mint_pi_gpio_used |= 1 << output_pin2;

  /* make sure pin mode is output */
  i = gpioSetMode( enable_pin, PI_OUTPUT );
  mint_check( i==0, "cannot set enable pin to output" );
  i = gpioSetMode( output_pin1, PI_OUTPUT );
  mint_check( i==0, "cannot set output pin 1 mode to output" );
  i = gpioSetMode( output_pin2, PI_OUTPUT );
  mint_check( i==0, "cannot set output pin 2 mode to output" );

  /* if activity is below the zero point, we turn it around so that it
     is above it by the same amount, and we swap the output pins to
     drive the motor in reverse */
  if( activity<zero_point ) {
    activity = 2*zero_point - activity;
    i = output_pin2;
    output_pin2 = output_pin1;
    output_pin1 = i;
  }

  /* scale activity from [0, 1] to the pin range */
  i = gpioGetPWMrange( output_pin1 );
  mint_check( i != PI_BAD_USER_GPIO, "cannot get output pin range" );
  activity *= i; 

  /* turn off one pin, enable motor output, turn on the other pin */
  i = gpioWrite( output_pin2, 0 );
  mint_check( i==0, "cannot turn off output pin" );
  i = gpioWrite( enable_pin, 1 );
  mint_check( i==0, "cannot turn on enable pin" );
  i = gpioPWM( output_pin1, activity );
  mint_check( i==0, "cannot turn on output pin" );
}


void mint_pi_gpiosensor( mint_nodes n, int min, int max, float *p ) {
  int input_pin, i, size;
  float increment;

  input_pin = p[0];
  increment = p[1];

  mint_check( input_pin != -1, "input_pin not set (1st op parameter)" );

  i = gpioSetMode( input_pin, PI_INPUT );
  mint_check( i==0, "cannot set read mode on input_pin" );

  input_pin = gpioRead( input_pin );
  mint_check( input_pin==0, "cannot read from input_pin" );

  if( !input_pin )
    return;

  size = mint_nodes_size( n );
  for( i=0; i<size; i++ )
    n[0][i] += increment;
  
}
