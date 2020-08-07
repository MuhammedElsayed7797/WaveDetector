/*
 * Adc.c
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */



#include "Adc.h"
#include "Adc_Private.h"


void ( * Adc_Interrupt_FunctPtr ) (void) ;


/*****************************************************************************/

void Adc_Init (void)
{

	/* reference voltage */
	ADMUX_REG &= 0xCF ;
	ADMUX_REG |= ADC_REF_VOLTAGE ;

	/* ADC resolution */
	ASSIGN_BIT( ADMUX_REG, 5, ADC_RESOLUTION_8 ) ;

	/* ADC pin Select */
	ADMUX_REG &= 0xE0 ;
	ADMUX_REG |= ADC_INPUT_PIN ;

	Adc_AutoTrigger ( ADC_AUTO_TRIGGER ) ;

	/* ADC Prescaler */
	ADCSRA_REG &= 0xF8 ;
	ADCSRA_REG |= ADC_PRESCALER ;

	/* set Trigger Source */
	Adc_AutoTrigger_Source ( ADC_TRIGGER_SOURCE) ;


	Adc_Clear_InterruptFlag () ;

	/* enable/disable interrupt */
	ASSIGN_BIT( ADCSRA_REG, 3, ADCE_INTERRUPT_ENABLE ) ;




	return ;
}

/*****************************************************************************/


void Adc_Interrupt_CallBack ( void (* FuncPtr) (void) )
{
	Adc_Interrupt_FunctPtr = FuncPtr ;
	return ;
}


/*****************************************************************************/


void Adc_AutoTrigger_Source ( uint8 SourceOption )
{

	SFIOR_REG &=  0x1F ;
	SFIOR_REG |= SourceOption << 5 ;

	return ;
}

/*****************************************************************************/


ISR( ADC_ISR )
{
	Adc_Interrupt_FunctPtr () ;

	return ;
}
/*****************************************************************************/
