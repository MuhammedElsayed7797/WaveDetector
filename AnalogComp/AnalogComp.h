/*
 * AnalogComp.h
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */


#ifndef ANALOGCOMP_ANALOGCOMP_H_
#define ANALOGCOMP_ANALOGCOMP_H_


#include "../Common/Utils.h"
#include "../AnalogComp/AnalogComp_Cfg.h"



#define ACSR_REG	*( ( volatile uint8 * ) 0x28 )
#define SFIOR_REG	*( ( volatile uint8 * ) 0x50 )




/*****************************************************************************/

/* inputs for function AnalogComp_Interrupt_TriggerOption */
#define ANALOG_COMP_TOGGLE_INTERRUPT	0x00u
#define ANALOG_COMP_FALLING_INTERRUPT	0x02u
#define ANALOG_COMP_RISING_INTERRUPT	0x03u


/*****************************************************************************/

/* when option is TRUE Analog Comparator is disabled if FALSE then enabled*/
#define AnalogComp_SwitchOff( Option )			ASSIGN_BIT( ACSR_REG, 7,Option )

/*****************************************************************************/

/* when option is TRUE Analog Comparator is disabled if FALSE then enabled*/
#define AnalogComp_ConstanInput( Option )	ASSIGN_BIT( ACSR_REG, 6, Option )

/*****************************************************************************/

/* when Option is TRUE and ADC is OFF, ADC MUX select i/p of negative pin of analog comparator */
#define AnalogComp_ADC_MUX_Input( Option )		ASSIGN_BIT( SFIOR_REG, 3, Option )

/*****************************************************************************/

/* when option is TRUE Analog Comparator is disabled if FALSE then enabled*/
#define AnalogComp_ICU_ACO_InputSelect( Option )	ASSIGN_BIT( ACSR_REG, 2, Option )

/*****************************************************************************/

#define AnalogComp_OutputValue()				GET_BIT( ACSR_REG, 5 )

/*****************************************************************************/

#define AnalogComp_Clear_InterruptFlag()		ACSR_REG = ACSR_REG

/*****************************************************************************/

#define AnalogComp_Read_InterruptFlag()			GET_BIT( ACSR_REG, 4 )

/*****************************************************************************/

#define AnalogComp_InterruptEnable()			SET_BIT( ACSR_REG, 3 )

/*****************************************************************************/
#define AnalogComp_InterruptDisable()			CLR_BIT( ACSR_REG, 3 )

/*****************************************************************************/


void AnalogCompartor_Init (void) ;

/*****************************************************************************/

void AnalogComp_Interrupt_TriggerOption ( uint8 TriggerOption ) ;

/*****************************************************************************/

void AnalogComp_Interrupt_CallBack ( void (* FuncPtr) (void) ) ;

/*****************************************************************************/


#endif /* ANALOGCOMP_ANALOGCOMP_H_ */
