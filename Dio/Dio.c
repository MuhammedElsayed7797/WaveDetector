/*
 * Dio.c
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 *
 */



#include "Dio.h"
#include "Dio_Private.h"



/*This function initialize the DIO as user configration*/
void Dio_Init( void )
{
	ASSIGN_PORT( DDRA_REG, CONC(PINDA0,PINDA1,PINDA2,PINDA3,PINDA4,PINDA5,PINDA6,PINDA7) ) ;
	ASSIGN_PORT( DDRB_REG, CONC(PINDB0,PINDB1,PINDB2,PINDB3,PINDB4,PINDB5,PINDB6,PINDB7) ) ;
	ASSIGN_PORT( DDRC_REG, CONC(PINDC0,PINDC1,PINDC2,PINDC3,PINDC4,PINDC5,PINDC6,PINDC7) ) ;
	ASSIGN_PORT( DDRD_REG, CONC(PINDD0,PINDD1,PINDD2,PINDD3,PINDD4,PINDD5,PINDD6,PINDD7) ) ;
	ASSIGN_PORT( PORTA_REG, CONC(PINA0,PINA1,PINA2,PINA3,PINA4,PINA5,PINA6,PINA7) ) ;
	ASSIGN_PORT( PORTB_REG, CONC(PINB0,PINB1,PINB2,PINB3,PINB4,PINB5,PINB6,PINB7) ) ;
	ASSIGN_PORT( PORTC_REG, CONC(PINC0,PINC1,PINC2,PINC3,PINC4,PINC5,PINC6,PINC7) ) ;
	ASSIGN_PORT( PORTD_REG, CONC(PIND0,PIND1,PIND2,PIND3,PIND4,PIND5,PIND6,PIND7) ) ;
	
	return ;
}

void DIO_SetPortDirection(uint8 Port_number,uint8 Direction)
{
	switch (Port_number)
	{
	case PORT0:
		if (Direction == DIO_INPUT)
		{
			DDRA_REG = 0x0;
		}
		else
		{
			DDRA_REG =0xff;
		}
		break;
	case PORT1:
		if (Direction == DIO_INPUT)
		{
			DDRB_REG = 0x0;
		}
		else
		{
			DDRB_REG =0xff;
		}
		break;
	case PORT2:
		if (Direction == DIO_INPUT)
		{
			DDRC_REG = 0x0;
		}
		else
		{
			DDRC_REG =0xff;
		}
		break;
	case PORT3:
		if (Direction == DIO_INPUT)
		{
			DDRD_REG = 0x0;
		}
		else
		{
			DDRD_REG =0xff;
		}
		break;
	default:
		break;
	}
}

void DIO_SetPortValue(uint8  Port_number,uint8  Value)
{
	switch (Port_number)
	{
	case PORT0:
		PORTA_REG = Value;
		break;
	case PORT1:
		PORTB_REG = Value;
		break;
	case PORT2:
		PORTC_REG = Value;
		break;
	case PORT3:
		PORTD_REG = Value;
		break;
	default:
		break;
	}
}

void Dio_SetPinDir(uint8 LOC_uint8_PinNum,uint8 LOC_uint8_PinDir)
{
	uint8 port = LOC_uint8_PinNum / 8;
	uint8 LOC_uint8_PinNumPortPer = LOC_uint8_PinNum % 8;

	switch(port)
	{
		case (0) :
		{
			ASSIGN_BIT(DDRA_REG, LOC_uint8_PinNumPortPer, LOC_uint8_PinDir);
			break;
		}
		case (1) :
		{
			ASSIGN_BIT(DDRB_REG, LOC_uint8_PinNumPortPer, LOC_uint8_PinDir);
			break;
		}
		case (2) :
		{
			ASSIGN_BIT(DDRC_REG, LOC_uint8_PinNumPortPer, LOC_uint8_PinDir);
			break;
		}
		case (3) :
		{
			ASSIGN_BIT(DDRD_REG, LOC_uint8_PinNumPortPer, LOC_uint8_PinDir);
			break;
		}

	}

	return ;
}


void Dio_SetPinValue(uint8 LOC_uint8_PinNum,uint8 LOC_uint8_Pinvalue)
{
	uint8 port = LOC_uint8_PinNum / 8;
	uint8 LOC_uint8_PinNumPortPer = LOC_uint8_PinNum % 8;


	switch(port)
	{
		case (0) :
		{
			ASSIGN_BIT(PORTA_REG, LOC_uint8_PinNumPortPer, LOC_uint8_Pinvalue);
			break;
		}
		case (1) :
		{
			ASSIGN_BIT(PORTB_REG, LOC_uint8_PinNumPortPer, LOC_uint8_Pinvalue);
			break;
		}
		case (2) :
		{
			ASSIGN_BIT(PORTC_REG, LOC_uint8_PinNumPortPer, LOC_uint8_Pinvalue);
			break;
		}
		case (3) :
		{
			ASSIGN_BIT(PORTD_REG, LOC_uint8_PinNumPortPer, LOC_uint8_Pinvalue);
			break;
		}

	}
	return ;

}
uint8 Dio_GetPinValue(uint8 LOC_uint8_PinNum)
{
	uint8 port = LOC_uint8_PinNum / 8;
	uint8 LOC_uint8_PinNumPortPer = LOC_uint8_PinNum % 8;
	uint8 LOC_uint8_RetPin = 0;


	switch(port)
	{
		case (0) :
		{
			LOC_uint8_RetPin = GET_BIT(PINA_REG ,LOC_uint8_PinNumPortPer);
			break;
		}
		case (1) :
		{
			LOC_uint8_RetPin = GET_BIT(PINB_REG ,LOC_uint8_PinNumPortPer);
			break;
		}
		case (2) :
		{
			LOC_uint8_RetPin = GET_BIT(PINC_REG ,LOC_uint8_PinNumPortPer);
			break;
		}
		case (3) :
		{
			LOC_uint8_RetPin = GET_BIT(PIND_REG ,LOC_uint8_PinNumPortPer);
			break;
		}

	}
	return LOC_uint8_RetPin;
}



