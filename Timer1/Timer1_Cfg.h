/*
 * 	Timer1_Cfg.h
 *
 *  Created on: Apr 13, 2020
 *  Author: Bebo
 *
 */


#ifndef Timer1_CFG_H_
#define Timer1_CFG_H_

/* used to determine if user need to call back function of ICU interrupt if TRUE then call back is enabled */
#define TIMER1_ICU_USER_ISR_ENABLE TRUE

/* initial value for counter */
#define TIMER1_COUNTER_VALUE		(0x0000)

/* initial clock source */
#define TIMER1_CLOCK_SOURCE			(Timer1_1_PRESCALER)

/* initial mode of operation */
#define TIMER1_MODE_SELECT			(Timer1_NORMAL)

/* initial TOP variable if mode of operation need TOP value */
#define TIMER1_TOP_VALUE			(0xFFFF)

/* initial compare match value if mode of operation allow of use compare match */
#define TIMER1_COMPARE_VALUE_A		(0x0001)
#define TIMER1_COMPARE_VALUE_B		(0x0001)

/* action to be taken on OC1A/B  when match signal is issued */
#define TIMER1_MATCH_SIGNAL_A		(TIMER1_CLEAR_ON_MATCH)
#define TIMER1_MATCH_SIGNAL_B		(TIMER1_CLEAR_ON_MATCH)

/* enable noise canceler for ICU if TRUE */
#define TIMER1_ICU_NOISE_CANCELER	(TRUE)

/* trigger edge for ICU, TRUE for rising edge */
#define TIMER1_ICU_RISING_EDGE		(TRUE)

/* TRUE to enable overflow interrupt */
#define TIMER1_OVER_FLOW_INTERRUPT	(FALSE)

/* TRUE to enable compare A interrupt */
#define TIMER1_COMPARE_A_INTERRUPT	(FALSE)

/* TRUE to enable compare B interrupt */
#define TIMER1_COMPARE_B_INTERRUPT	(FALSE)

/* TRUE to enable ICU interrupt */
#define TIMER1_ICU_INTERRUPT		(FALSE)



#endif /* Timer1_CFG_H_ */
