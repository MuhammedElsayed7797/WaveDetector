/*
 * 	Timer1.h
 *
 *  Created on: Apr 13, 2020
 *  Author: Bebo
 *
 */

#ifndef Timer1_H_
#define Timer1_H_


#include "../Common/Utils.h"
#include "../Timer1/Timer1_Cfg.h"


#define TIMSK_REG	*( ( volatile uint8 * ) 0x59 )
#define TIFR_REG	*( ( volatile uint8 * ) 0x58 )
#define TCCR1B_REG	*( ( volatile uint8 * ) 0x4E )


/* used to keep value of parameter passed to Timer1_SetClock to set Clock source when calling Timer1_Start*/
uint8 Timer1_ClockSource  ;

/*****************************************************************************/

/* used as inputs for Timer1_MatchSignal_A & Timer1_MatchSignal_B functions
 * if input other than TIMER1_STOP_MATCH_SIGNAL is passed then OC1A or OC1B will be
 * derived as output compare pin its value depend on timer operation
 * and will lose its DIO functionality, but still controlled as i/p or o/p
 * if i/p then no Timer value will appear on it
 * */

/* this value will make OC1x pin disconnected form timer operation, and will return to its DIO functionality */
#define TIMER1_STOP_MATCH_SIGNAL	0b00u

/* this value used in non-PWM modes except with following PWM modes with just OC1A
 * Timer1_PHASE_FREQUENCY_PWM_2
 * Timer1_PHASE_PWM_VARIABLE_2
 * Timer1_FAST_PWM_VARIABLE_2
 * if used with PWM mode other than this modes  for OC1A then it pin will be disconnected as TIMER1_STOP_MATCH_SIGNAL option
 *
 * this option Toggle value of OC1x when ever match happen
 * */
#define	TIMER1_TOGGLE_ON_MATCH		0b01u

/* this option clear OC1x when match happen for non PWM modes
 * and for PWM modes this option used to make non inverted signal
 * where OC1x will be cleared when match happen as counter is incremented
 * and set when match happen as counter is decremented */
#define	TIMER1_CLEAR_ON_MATCH		0b10u

/* this option set OC1x when match happen for non PWM modes
 * and for PWM modes this option used to make inverted signal
 * where OC1x will be set when match happen as counter is incremented
 * and cleared when match happen as counter is decremented */
#define	TIMER1_SET_ON_MATCH			0b11u

/*****************************************************************************/

/* this used as i/p for Timer1_SetClock function */

/* Timer1 clock = CPU clock */
#define	Timer1_1_PRESCALER			0x01u

/* Timer1 clock = CPU clock / 8 */
#define	Timer1_8_PRESCALER			0x02u

/* Timer1 clock = CPU clock / 64 */
#define	Timer1_64_PRESCALER			0x03u

/* Timer1 clock = CPU clock / 256 */
#define	Timer1_256_PRESCALER		0x04u

/* Timer1 clock = CPU clock / 1024 */
#define	Timer1_1024_PRESCALER		0x05u

/* Timer1 clock = external clock applied to T1, counter increment/decrement  on falling edge
 * increment will happen after delay of 2.5~3.5 CPU cycles */
#define	Timer1_EXTERNAL_FALLING		0x06u

/* Timer1 clock = external clock applied to T1, counter increment/decrement  on rising edge
 *  * increment will happen after delay of 2.5~3.5 CPU cycles */
#define	Timer1_EXTERNAL_RISING		0x07u

/*****************************************************************************/

/* used as i/p for Timer1_InterruptEnable, Timer1_Clear_InterruptFlag & Timer1_Read_InterruptFlag functions  */

/* for overflow interrupt of timer1 */
#define Timer1_OVERFLOW_INTERRUPT		0x02u

/* for compare match A of timer1 */
#define Timer1_COMPARE_A_INTERRUPT		0x04u

/* for compare match B of timer1 */
#define Timer1_COMPARE_B_INTERRUPT		0x03u

/* for iCU interrupt of timer 1 */
#define Timer1_INPUT_CAPTURE_INTERRUPT	0x05u

/*****************************************************************************/


/* used as inputs for function Timer1_Set_Mode to determine operation mode */



/* for this mode TOP value is 0xFFFF
 * overflow interrupt is fired when counter turn from 0xFFFF to 0x00
 * could use both compare match A/B to fire interrupts when Compare value set by Timer1_SetCompare_A/B_Value
 *
 * could use ICU in this mode, not recommended to generate signals in this mode just used to calculate time and ICU
 * */
#define Timer1_NORMAL						0x00u




/* phase correct PWM with fixed TOP values 0xFF,0x1FF, 0x3FF
 * phase correct PWM modes is recommended  to control motors
 * equation for PWM frequency is set by equation :-
 * TimerClock frequency / (2 * N * TOP ) : where N is prescaler value
 *
 * duty cycle is determined by compare value set by Timer1_SetCompare_A/B_Value functions
 * in non inverted mode as compare value increase duty cycle increase and in inverted mode signal is inverted
 * for compare value = 0 duty cycle = 0%
 * and for compare value = TOP duty cycle = 100%
 *
 * could use ICU in this mode but note that counter value is incremented till TOP then decrement till bottom
 *
 * overflow interrupt is fired when signal reach bottom and compare A/B interrupt fired when counter = compare value
 *
 * */
#define Timer1_PHASE_PWM_FIXED_8			0x01u
#define Timer1_PHASE_PWM_FIXED_9			0x02u
#define Timer1_PHASE_PWM_FIXED_10			0x03u




/*
 * just like normal mode but counter is cleared when counter reach TOP value
 *
 * TOP value is determined by Timer1_SetTopValue function and cannot use Timer1_SetCompare_A_Value
 *
 * overflow interrupt and compare A interrupt is fired at same moment
 * compare B interrupt fired when counter = compare value set by Timer1_SetCompare_B_Value
 *
 * could generate signal on OC1A/B according to setting of Timer1_MatchSignal_A/B
 *
 * this mode generate square wave with 50% duty cycle and frequency determined by equation :-
 * TimerClock frequency / (2 * N * (TOP + 1 ) ) : where N is prescaler value
 *
 *
 * could use ICU in this mode
 *
 * */
#define Timer1_CTC_1						0x04u



/* like phase correct PWM with fixed value but here duty cycle at compare value = 0 is 1/TOP * 100 % for non inverted mode
 * and counter is incremented till TOP then cleared to 0x00 and start over
 *
 * if compare value set by Timer1_SetCompare_A/B_Value = to TOP then match signal is ignored and no interrupt will be triggered
 *
 * frequency for fast PWM determined by equation :-
 * TimerClock frequency / ( N * (TOP + 1 ) ) : where N is prescaler value
 *
 * */
#define Timer1_FAST_PWM_FIXED_8				0x05u
#define Timer1_FAST_PWM_FIXED_9				0x06u
#define Timer1_FAST_PWM_FIXED_10			0x07u



/* this is phase and frequency correct PWM but with variable TOP value determined by Timer1_SetTopValue function
 * same as phase correct PWM but is used when TOP value is not fixed and changed during run time  */

/* in this mode can't use ICU
 * and ICU interrupt is happened after one timer cycle form counter value = TOP
 * here change of TOP will be immediate */
#define Timer1_PHASE_FREQUENCY_PWM_1		0x08u

/* in this mode could use ICU but can't use Timer1_SetCompare_A_Value and compare A interrupt
 * is happened after one timer cycle form counter value = TOP
 * change of TOP will happen when counter = old TOP value
 *
 * in this mode could generate signal like CTC mode but update of TOP won't be immediate
 * this illustrated in Timer1_SetTopValue */

#define Timer1_PHASE_FREQUENCY_PWM_2		0x09u



/* this is phase and frequency correct PWM but with variable TOP value determined by Timer1_SetTopValue function
 * same as phase correct PWM but is recommended to use when TOP value is not fixed and not changed during run time  */

/* in this mode can't use ICU
 * and ICU interrupt is happened after one timer cycle form counter value = TOP
 * if TOP value changed during runtime then update will be immediate */
#define Timer1_PHASE_PWM_VARIABLE_1			0x0Au

/* in this mode could use ICU but can't use Timer1_SetCompare_A_Value and compare A interrupt
 * is happened after one timer cycle form counter value = TOP
 * if value changed during runtime then update will be when counter value = old TOP value
 *
 * in this mode could generate signal like CTC mode but update of TOP won't be immediate
 * this illustrated in Timer1_SetTopValue */
#define Timer1_PHASE_PWM_VARIABLE_2			0x0Bu

/* like CTC_1 but could use Timer1_SetCompare_A_Value to set compare value for Compare A */
#define Timer1_CTC_2						0x0Cu



/* like fast PWM with fixed TOP values but here TOP is variable and determined by Timer1_SetTopValue
 * recommend to use when TOP value is not fixed and not changed during run time */


/* in this mode can't use ICU
 * and ICU interrupt is happened after one timer cycle form counter value = TOP
 * if TOP value changed during runtime then update will be immediate */
#define Timer1_FAST_PWM_VARIABLE_1			0x0Eu

/* in this mode could use ICU but can't use Timer1_SetCompare_A_Value and compare A interrupt
 * won't be triggered
 * if value changed during runtime then update will be when counter value = old TOP value
 *
 * in this mode could generate signal like CTC mode but update of TOP won't be immediate
 * this illustrated in Timer1_SetTopValue */
#define Timer1_FAST_PWM_VARIABLE_2			0x0Fu


/*****************************************************************************/



/* this function used to set interrupt disable 
 * InterruptType determine interrupt to be enabled or disables
 * */
 
#define Timer1_InterruptDisable( InterruptType ) 		CLR_BIT (TIMSK_REG, InterruptType) ;


/****************************************************************************/


/* this function used to set interrupt enable 
 * InterruptType determine interrupt to be enabled or disables
 * */
 
#define Timer1_InterruptEnable( InterruptType ) 		SET_BIT (TIMSK_REG, InterruptType) ;


/****************************************************************************/


/* this function used to read interrupt flag by software
 * InterruptType determine interrupt to be enabled or disables
 * */

#define Timer1_Read_InterruptFlag( InterruptType ) 	GET_BIT (TIFR_REG, InterruptType) 


/****************************************************************************/

/* this function used to clear interrupt flag by software
 * all Timer1 interrupts is cleared automatically after interrupts execution
 * but could be cleared by software using this function
 * InterruptType determine interrupt to be enabled or disables
 * */

#define Timer1_Clear_InterruptFlag( InterruptType ) 	TIFR_REG = 1 << InterruptType 

/****************************************************************************/

/* called to start operations of counter, counter won't increment or decrement */
#define Timer1_Stop()	TCCR1B_REG &= 0xF8u 

/****************************************************************************/


/* called to start operating of Timer1 with last clock selected with Timer1_SetClock function
 * note that if external clock was selected by Timer1_SetClock then signal on T1 must be stable
 * for at least one CPU clock cycle
 */
#define Timer1_Start()	TCCR1B_REG |= Timer1_ClockSource 


/****************************************************************************/


/* function used to select clock source of Timer1
 * effect of selected clock source won't appear except calling Timer1_Start function
 * clock source could be CPUclock/prescaler or external clock on T1 pin
 * if external clock is selected then increment or decrement of counter will be delayed
 * 2.5~3.5 CPU clock cycles after rising or falling edge is applied to T1
 * external clock could be used to calculate external events which applied on T1 pin
 */

void Timer1_SetClock ( uint8 PreScaler ) ;

/*****************************************************************************/



/* if input other than TIMER1_STOP_MATCH_SIGNAL is passed then OC1A will be
* derived as output compare pin its value depend on timer operation
* and will lose its DIO functionality, but still controlled as i/p or o/p
* if i/p then no Timer value will appear on it
*
* to see effect of i/p passed to this function on OC1A pin then OC1A must
* be o/p
*
* setting for OC1A signal should be done while OC1A pin is as i/p to avoid wrong signals
* on OC1A pin
*
*/

void Timer1_MatchSignal_A ( uint8 MatchMode) ;

/****************************************************************************/



/* if input other than TIMER1_STOP_MATCH_SIGNAL is passed then OC1B will be
* derived as output compare pin its value depend on timer operation
* and will lose its DIO functionality, but still controlled as i/p or o/p
* if i/p then no Timer value will appear on it
*
* to see effect of i/p passed to this function on OC1A pin then OC1A must
* be o/p
*
* setting for OC1B signal should be done while OC1A pin is as i/p to avoid wrong signals
* on OC1B pin
*
*/

void Timer1_MatchSignal_B ( uint8 MatchMode) ;


/****************************************************************************/


/*
 * this function enable noise canceler of input capture unit if i/p is TRUE
 *
 * noise canceler will increase noise immunity but will increase delay of captured signal
 * and increase restrictions on signal minimum period
 *
 * noise canceler will increase delay of signal applied to ICP/ACO pin by 4 CPU cycles
 * this delay added to delay caused by sampling and edge detection signal which is 2.5~3.5 CPU cycles
 *
 * so if noise canceler enabled value captured by ICU is delayed by 7 CPU cycles (not timer cycles)
 *
 * noise canceler to change its o/p need ICP/ACO signal to be with same value for 4 CPU cycles
 * which put restrictions on i/p signal to have
 * HIGH state for more than 4 CPU cycles and LOW state for more than CPU cycles at least
 *
 * note that i/p for noise canceler and ICU is always enabled except if following modes are used
 * Timer1_FAST_PWM_VARIABLE_1
 * Timer1_CTC_2
 * Timer1_PHASE_PWM_VARIABLE_1
 * Timer1_PHASE_FREQUENCY_PWM_1
 *
 * ICP pin could be configured to be O/P to generate signal on it by DIO and operate ICU by software
 * */

void Timer1_ICU_NoiseCanceler_Enable ( boolean Enable_Option ) ;

/****************************************************************************/


/* this function change the edge which trigger operation of ICU and set ICU flag which will generate interrupt if enabled
 * if i/p is true then Rising Edge will trigger ICU
 * during this function ICU interrupt is disabled and ICU flag is cleared to prevent wrong interrupt generation
 * */

void Timer1_ICU_Select_RisingEdge ( boolean TriggerEdge_Option) ;


/****************************************************************************/


/* this function used to set operation mode according to ModeType
 *
 * every mode has its recommended environment to work in with specific advantages, restrictions, interrupts and equations
 * this parameters illustrated in Timer1.h file in modes I/Ps #defines
 *
 * for some modes could change TOP value
 * combination between mode and match signal set by Timer1_MatchSignal_A/B functions will determine shape of o/p signal on OC1X pins
 *
 * */

void Timer1_Set_Mode ( uint8 ModeType ) ;

/****************************************************************************/


/* this function used to set TOP value for modes that have variable TOP values
 * if function called while mode set isn't use variable TOP value function will return E_NOT_OK
 *
 * in following PWM modes the TOP value won't has effect immediately due double buffer feature
 *
 * Timer1_PHASE_FREQUENCY_PWM_2 	TOP value will changed when counter = 0x00
 * Timer1_PHASE_PWM_VARIABLE_2		TOP value will changed when counter = previous TOP value
 * Timer1_FAST_PWM_VARIABLE_2		TOP value will changed when counter = previous TOP value
 *
 * also in previous modes according to Timer1_MatchSignal_A if TIMER1_TOGGLE_ON_MATCH is selected
 * then OC1A will be toggled when counter = TOP which will be like CTC with same option of Timer1_MatchSignal_A
 * but difference here is that change compare value which is determined by TOP value just updated when
 * counter = TOP, but in CTC mode update of Compare value will be immediate
 *
 * minimum TOP value is 3
 *
 * */

StatusType Timer1_SetTopValue ( uint16 TopValue ) ;

/****************************************************************************/


/* this function used to set the value  when the timer is equal to it, match signal is issued
 * which could use to clear counter or to generate interrupt  or set specific signal on OC1A pin
 *
 *	in following PWM modes the function  won't be available and will return E_NOT_OK
 *
 * Timer1_PHASE_FREQUENCY_PWM_2 	TOP value will changed when counter = 0x00
 * Timer1_PHASE_PWM_VARIABLE_2		TOP value will changed when counter = previous TOP value
 * Timer1_FAST_PWM_VARIABLE_2		TOP value will changed when counter = previous TOP value
 *
 *
 * in non PWM modes this function set value which will used to generate interrupt at specific timer value
 * and set/clear/toggle OC1A pin if wanted according to Timer1_MatchSignal_A options
 *
 * in PWM modes this function used to determine duty cycle of PWM and to generate interrupt at specific counter value = CompareValue
 *
 * according to Timer1_MatchSignal_A determine signal is inverted or non inverted
 * if non inverted then as CompareValue increase duty cycle increase, and when CompareValue = TOP then duty cycle =100%
 * and for CompareValue = 0, duty cycle = 100/TOP %
 * and opposite for inverted mode CompareValue = TOP then duty cycle = 0%
 *
 * in PWM modes Compare value won't has effect immediately due double buffer feature
 * update will have effect in TOP for all PWM modes except for following PWM modes update will be in BOTTOM
 * Timer1_PHASE_FREQUENCY_PWM_1
 * Timer1_PHASE_FREQUENCY_PWM_2
 *
 *
 * */
StatusType Timer1_SetCompare_A_Value ( uint16 CompareValue ) ;

/****************************************************************************/


/* this function used to set the value  when the timer is equal to it, match signal is issued
 * which could use to clear counter or to generate interrupt  or set specific signal on OC1A pin
 *
 * in non PWM modes this function set value which will used to generate interrupt at specific timer value
 * and set/clear/toggle OC1A pin if wanted according to Timer1_MatchSignal_A options
 *
 * in PWM modes this function used to determine duty cycle of PWM and to generate interrupt at specific counter value = CompareValue
 *
 * according to Timer1_MatchSignal_B determine signal is inverted or non inverted
 * if non inverted then as CompareValue increase duty cycle increase, and when CompareValue = TOP then duty cycle =100%
 * and for CompareValue = 0, duty cycle = 100/TOP %
 * and opposite for inverted mode CompareValue = TOP then duty cycle = 0%
 *
 * in PWM modes Compare value won't has effect immediately due double buffer feature
 * update will have effect in TOP for all PWM modes except for following PWM modes update will be in BOTTOM
 * Timer1_PHASE_FREQUENCY_PWM_1
 * Timer1_PHASE_FREQUENCY_PWM_2
 *
 * */
void Timer1_SetCompare_B_Value ( uint16 CompareValue ) ;

/****************************************************************************/


/* this function used to get current counter value this value won't be correct if counter is running in high speed */

uint16 Timer1_ReadCounterValue (void) ;

/****************************************************************************/


/* this function will set counter value
 * update will have immediate effect
 * if updated value = to compare value set by Timer1_MatchSignal_A/B value
 * then match will be ignored and no update for OC1A/B pin or trigger for interrupt
 * */

void Timer1_SetCounterValue ( uint16 CounterValue ) ;


/****************************************************************************/


/* this function used to pass callback functions of interrupt specified by  InterruptType
 * callback function must be void (void) function
 * */

void Timer1_Interrupt_CallBack ( uint8 InterruptType , void (* FuncPtr) (void) ) ;

/****************************************************************************/



/* this function is not necessary to use it just call combination of Timer1 function to ease initialization
 * process according to Timer1_Cfg.h file
 *
 * user must start timer operation after this function using Timer1_Start and initialize call back functions ;
 * */

void Timer1_Init (void) ;

/****************************************************************************/

/* this function clear all Timer1_register and stop its operation */
void Timer1_deInit (void) ;

/****************************************************************************/


#endif /* Timer1_H_ */





