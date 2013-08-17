/* functions to interface mint with Raspberry PI */
#ifndef MINT_PI_H
#define MINT_PI_H

#include "nodes.h"

/** Initialize communication with the Raspberry Pi Hardware. Call
    before using any mint_pi feature (including creating a network
    from an architecture file that contains mint_pi ops). Calling more
    than once does not harm. */
void mint_pi_init( void );

/** An op to have mint nodes control a servo motor on the Raspberry
   Pi. Average node activity is used to control servo position. An
   activity of 0 means complete anticlockwise, 0.5 means center, and 1
   complete clockwise. You must provide the following parameters: GPIO
   pin number for the servo motor, minimum and maximum pulses, GPIO
   pin number to enable the motor controller (use the default of -1 if
   you are not using a motor controller). This op assumes that average
   node activity will be between 0 and 1. */
void mint_pi_servomotor( mint_nodes n, int, int, float * );

/** An op to have mint nodes control a dc motor on the Raspberry Pi,
   using a motor controller such as the L293D or SN754410. The op
   takes six parameters: GPIO pin number to enable the motor
   controller, two GPIO pin numbers for the output pins, a value
   denoting the zero point (node activity above or below this value
   will drive the motor in one or ther other direction), activity
   threshold (departures from the zero point below threshold are
   ignored). The defaults are zero point = 0.5 and threshold =
   0.1. Thus node activity between 0.6 and 1 will drive the motor in
   one direction (at increasing speed), activity below 0.4 will drive
   it in the other direction (at increasing speed), and activity
   between 0.4 and 0.6 will leave the motor off. The sixth parameter
   (optional) denotes whether the mode of gpio pins should be set once
   for all (parameter value of 0), or every time the op is called
   (parameter value of 1). Setting the output mode once for all should
   be safe unless someoene is also reading from the same pin. */
void mint_pi_dcmotor( mint_nodes n, int, int, float * );

/** An op to read from a sensor attached to a GPIO pin. If the pin is
    on, the input to all nodes is incremented by an amount specified
    by the second op parameter (default = 1), times the value read
    from the pin. The first parameter is the pin number.

    NOTE: A GPIO might change level at a time when the network is not
    being updated. This is taken care of (read pi.c if you want the
    details), so whenever the nodes are updated they will see in input
    all increments accrued since the last time they have been updated.
 */  
void mint_pi_gpiosensor( mint_nodes n, int, int, float * );


#endif /* MINT_PI_H */

