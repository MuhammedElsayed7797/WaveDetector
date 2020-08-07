/*
 * Adc_Cfg.h
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */

#ifndef ADC_ADC_CFG_H_
#define ADC_ADC_CFG_H_

/* following parameters determine ADC setting initialized by ADC_Init Function */

/* Choose Reference voltage for ADC */
#define ADC_REF_VOLTAGE			0x00

/* Resolution of ADC read */
#define ADC_RESOLUTION_8		FALSE

/* I/P for ADC pin */
#define ADC_INPUT_PIN			0x00

#define ADC_AUTO_TRIGGER		FALSE

/* ADC prescaler */
#define ADC_PRESCALER			0x00

/* ADC Trigger Source */
#define ADC_TRIGGER_SOURCE  	ADC_ANALOG_COM_TRIGGER

/* Enable/Disable ADC interrupt */
#define ADCE_INTERRUPT_ENABLE 	FALSE


#endif /* ADC_ADC_CFG_H_ */
