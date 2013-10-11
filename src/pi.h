/* functions to interface mint with Raspberry PI */
#ifndef MINT_PI_H
#define MINT_PI_H

#include "nodes.h"

/** Initialize communication with the Raspberry Pi Hardware. Call
    before using any mint_pi feature (including creating a network
    from an architecture file that contains mint_pi ops). Calling more
    than once does no harm. */
void mint_pi_init( void );

/** An op to have mint nodes control a servo motor on the Raspberry
   Pi. Average node activity (assumed between 0 and 1) is used to
   control servo position. An activity of 0 means complete
   anticlockwise, 0.5 means center, and 1 complete clockwise. 

   Parameters: 0: GPIO pin number for the servo motor control

               1: Minimum pulse value (complete anticlockwise)

	       2: Maximum pulse value (complete clockwise)

	       3: Whether to set the GPIO mode once for all (0) or
	          just once (1, default 0).

	       4,5: GPIO pins to enable a motor controller (parameter
	            4) and to give power to the servo (parameter
	            5). These parameters default to -1, which means no
	            motor controller is being used. */
void mint_pi_servomotor( mint_nodes n, int, int, float * );

/** An op to have mint nodes control a dc motor on the Raspberry Pi,
    using a motor controller such as the L293D or SN754410. 

    Parameters: 0: GPIO pin number to enable the motor controller.

               1,2: GPIO pin numbers for the output pins

	       3: A value denoting the zero point (node activity above
                  or below this value will drive the motor in one or
                  ther other direction, default 0.5)

	       4: Activity threshold (departures from the zero point
                  below threshold are ignored, default 0.1). 

	       5: Whether mode of GPIO pins should be set once for all
                  (parameter value of 0), or every time the op is
                  called (parameter value of 1, default 0). Setting
                  the output mode once for all should be safe unless
                  someoene is also reading from the same pin.

     NOTES: For exmaple, node activity between 0.6 and 1 will drive
     the motor in one direction (at increasing speed), activity below
     0.4 will drive it in the other direction (at increasing speed),
     and activity between 0.4 and 0.6 will leave the motor off.*/
void mint_pi_dcmotor( mint_nodes n, int, int, float * );

/** An op to read from a sensor attached to a GPIO pin. 

    Parameters: 0: GPIO pin number to read from.

                1: Scale factor (default 1). Node input is increment
                by the value read from the GPIO pin times this scale
                factor.

		2: Node state variable to which increments are added
		(default 0, i.e., the node input variable).
 
    NOTE: A GPIO might change level at a time when the network is not
    being updated. This is taken care of (read pi.c if you want the
    details), so whenever the nodes are updated they will see in input
    all increments accrued since the last time they have been updated.
 */  
void mint_pi_gpiosensor( mint_nodes n, int, int, float * );


#endif /* MINT_PI_H */

