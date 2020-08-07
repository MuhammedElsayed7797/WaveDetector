/*
 * 	Timer0_Private.h
 *
 *  Created on: Apr 13, 2020
 *  Author: Bebo
 *
 */

#ifndef TIMER0_PRIVATE_H_
#define TIMER0_PRIVATE_H_


/*****************************************************************************/


#define ICU1_ISR			__vector_5
#define CMPA1_ISR			__vector_6
#define CMPB1_ISR			__vector_7
#define OVF1_ISR			__vector_8


/* the interrupt function depend on vector sent to it go back to vector table   */
#define	ISR( vector )			void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
								void vector (void)


/*****************************************************************************/



#define TCCR1A_REG	*( ( volatile uint8 * ) 0x4F )
#define TCCR1B_REG	*( ( volatile uint8 * ) 0x4E )
#define TIMSK_REG	*( ( volatile uint8 * ) 0x59 )
#define TIFR_REG	*( ( volatile uint8 * ) 0x58 )

#define TCNT1_REG	*( ( volatile uint16 * ) 0x4C )
#define OCR1A_REG	*( ( volatile uint16 * ) 0x4A )
#define OCR1B_REG	*( ( volatile uint16 * ) 0x48 )
#define ICR1_REG	*( ( volatile uint16 * ) 0x46 )


/*****************************************************************************/

#endif /* TIMER0_PRIVATE_H_ */
