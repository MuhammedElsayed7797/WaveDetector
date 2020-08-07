/*
 * Dio_Private.h
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 *
 */

#ifndef DIO_PRIVATE_H_
#define DIO_PRIVATE_H_


#define CONC_HELPER(A0,A1,A2,A3,A4,A5,A6,A7) 		0b##A7##A6##A5##A4##A3##A2##A1##A0
#define CONC(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0) 		CONC_HELPER(PA7,PA6,PA5,PA4,PA3,PA2,PA1,PA0)


#define PORTA_REG 	*( (volatile uint8 *)(0x3B) )
#define PORTB_REG 	*( (volatile uint8 *)(0x38) )
#define PORTC_REG	*( (volatile uint8 *)(0x35) )
#define PORTD_REG 	*( (volatile uint8 *)(0x32) )

#define PINA_REG 	*( (volatile uint8 *)(0x39) )
#define PINB_REG 	*( (volatile uint8 *)(0x36) )
#define PINC_REG 	*( (volatile uint8 *)(0x33) )
#define PIND_REG 	*( (volatile uint8 *)(0x30) )

#define DDRD_REG 	*( (volatile uint8 *)(0x31) )
#define DDRC_REG 	*( (volatile uint8 *)(0x34) )
#define DDRB_REG 	*( (volatile uint8 *)(0x37) )
#define DDRA_REG 	*( (volatile uint8 *)(0x3A) )


#endif /* DIO_PRIVATE_H_ */
