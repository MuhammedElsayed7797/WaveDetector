/*
 * AnalogComp.c
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */


#include "AnalogComp.h"
#include "AnalogComp_Private.h"


void ( * AnalogComp_Interrupt_FunctPtr ) (void) ;

/*****************************************************************************/



void AnalogCompartor_Init (void)
{
	AnalogComp_ICU_ACO_InputSelect ( ANALOG_COMP_ICU_ACO_INPUT ) ;

	AnalogComp_ADC_MUX_Input ( ANALOG_COMP_ADC_MUX_INPUT ) ;

	AnalogComp_ConstanInput ( ANALOG_COMP_CONSTANT_INPUT ) ;

	AnalogComp_Interrupt_TriggerOption ( ANALOG_COMP_INTERRUPT_TRIGGER ) ;

	AnalogComp_Clear_InterruptFlag () ;

	ASSIGN_BIT (ACSR_REG, 3, ANALOG_COMP_INTERRUPT_ENABLE ) ;

	AnalogComp_SwitchOff ( ANALOG_COMP_SWITCH_OFF ) ;

	return ;
}




/*****************************************************************************/



void AnalogComp_Interrupt_TriggerOption ( uint8 TriggerOption )
{
	ACSR_REG &= 0xFC ;
	ACSR_REG |= TriggerOption ;

	return ;
}


void AnalogComp_Interrupt_CallBack ( void (* FuncPtr) (void) )
{
	AnalogComp_Interrupt_FunctPtr = FuncPtr ;
	return ;
}

/*****************************************************************************/

ISR( ACMP_ISR )
{
	AnalogComp_Interrupt_FunctPtr () ;
	
	return ;
}

/*****************************************************************************/
