/*
 * AnalogComp_Cfg.h
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */

#ifndef ANALOGCOMP_ANALOGCOMP_CFG_H_
#define ANALOGCOMP_ANALOGCOMP_CFG_H_

/* following parameters determine Analog Comparator setting initialized by AnalogCompartor_Init Function */


/* enable ACO to be input for ICU */
#define ANALOG_COMP_ICU_ACO_INPUT		FALSE

/* make on of ADC inputs is negative input for analog comparator */
#define ANALOG_COMP_ADC_MUX_INPUT		FALSE

/* set positive input for analog comparator to constant DC value */
#define ANALOG_COMP_CONSTANT_INPUT		FALSE

/* choose which logic of ACO set analog comparator interrupt flag */
#define ANALOG_COMP_INTERRUPT_TRIGGER	ANALOG_COMP_RISING_INTERRUPT

/* enable/disable analog comparator interrupt */
#define ANALOG_COMP_INTERRUPT_ENABLE	FALSE

/* switch on/off analog comparator  TRUE to switch off */
#define ANALOG_COMP_SWITCH_OFF			TRUE

#endif /* ANALOGCOMP_ANALOGCOMP_CFG_H_ */
