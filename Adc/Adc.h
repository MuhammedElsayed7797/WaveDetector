/*
 * Adc.h
 *
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_


#include "../Common/Utils.h"
#include "../Adc/Adc_Cfg.h"



#define ADCSRA_REG	*( ( volatile uint8 * ) 0x26 )
#define ADCH_REG	*( ( volatile uint8 * ) 0x25 )
#define ADCL_REG	*( ( volatile uint8 * ) 0x24 )
#define ADCLH_REG	*( ( volatile uint16 * ) 0x24 )


/*****************************************************************************/

#define ADC_ANALOG_COM_TRIGGER		0x01
#define ADC_TIMER1_COM_B_TRIGGER	0x05


/*****************************************************************************/

#define Adc_SwitchOn( Option )			ASSIGN_BIT( ADCSRA_REG, 7, Option )

/*****************************************************************************/

#define Adc_StartConversion( Option)	SET_BIT( ADCSRA_REG, 6 )

/*****************************************************************************/

#define Adc_AutoTrigger( Option )		ASSIGN_BIT( ADCSRA_REG, 5, Option )

/*****************************************************************************/

#define Adc_Clear_InterruptFlag()		ADCSRA_REG = ADCSRA_REG

/*****************************************************************************/

#define Adc_Read_InterruptFlag()		GET_BIT( ADCSRA_REG, 4 )

/*****************************************************************************/

#define Adc_InterruptEnable()			SET_BIT( ADCSRA_REG, 3 )

/*****************************************************************************/

#define Adc_InterruptDisable()			CLR_BIT( ADCSRA_REG, 3 )


/*****************************************************************************/

#define ADC_Read_8()					ADCH_REG

/*****************************************************************************/

#define ADC_Read_10()					ADCLH_REG

/*****************************************************************************/

void Adc_Init (void) ;


/*****************************************************************************/


void Adc_AutoTrigger_Source ( uint8 SourceOption ) ;

/*****************************************************************************/

void Adc_Interrupt_CallBack ( void (* FuncPtr) (void) ) ;


/*****************************************************************************/


#endif /* ADC_ADC_H_ */
