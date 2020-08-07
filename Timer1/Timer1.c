/*
 * 	Timer1_Cfg.h
 *
 *  Created on: Apr 13, 2020
 *  Author: Bebo
 *
 */



#include "Timer1.h"
#include "Timer1_Private.h"

/*****************************************************************************/

/* used to prevent using Timer1_SetTopValue function if TOP is fixed value  */

boolean Timer1_Variable_Top_Flag = FALSE ;

/* used to prevent writing to OCR1A register as a compare value in case OCR1A used to define TOP value */
boolean Timer1_ORC1A_Top_Flag = FALSE ;

/* used to contain ICU value user shall read it when ICU interrupt is fired and save its value
 * as soon as possible in temporary variable before value override */
uint16 Timer1_ICU_Value = 0x00u ;

/* array of function pointers used to get value of callback functions of Timer1 interrupts */
void ( * Timer1_FunctPtr_Array [4] ) (void ) ;


/*****************************************************************************/

/* function used to select clock source of Timer1
 * effect of selected clock source won't appear except calling Timer1_Start function
 * clock source could be CPUclock/prescaler or external clock on T1 pin
 *
 * if external clock is selected then increment or decrement of counter will be delayed
 * 2.5~3.5 CPU clock cycles after rising or falling edge is applied to T1
 * external clock could be used to calculate external events which applied on T1 pin
 *
 * for external clock maximum frequency is CPU clock frequency / 2 and it's recommended to be / 2.5
 * with duty cycle 50% for high frequencies
 *
 */

void Timer1_SetClock ( uint8 PreScaler )
{
	Timer1_ClockSource = PreScaler ;
	return ;
}



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

void Timer1_MatchSignal_A ( uint8 MatchMode)
{
	/* clear COM1A bits */
	TCCR1A_REG &= 0x3Fu ;

	/* set COM1A bits */
	TCCR1A_REG |= MatchMode << 6 ;
	return ;
}


/*****************************************************************************/


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

void Timer1_MatchSignal_B ( uint8 MatchMode)
{
	/* clear COM1B bits */
	TCCR1A_REG &= 0xCFu ;

	/* set COM1B bits */
	TCCR1A_REG |= MatchMode << 4 ;
	return ;
}

/*****************************************************************************/

/*
 * used this function to force match effecT on non PWM modes
 * this force will show effect of a match on just OC1A pin
 * OC1A pin will has immediate toggle, clear, set when force happen
 * force has no effect on OCF1 flag and won't trigger interrupt
 *
 * this function mustn't be used in PWM modes, if used then it will return E_NOT_OK
 * */

void Timer1_ForceMatch_A (void)
{
	/* set force bit for match A */
	TCCR1A_REG |= 0x08u ;
	return ;
}

/*****************************************************************************/

/*
 * used this function to force match effecT on non PWM modes
 * this force will show effect of a match on just OC1A pin
 * OC1A pin will has immediate toggle, clear, set when force happen
 * force has no effect on OCF1 flag and won't trigger interrupt
 *
 * this function mustn't be used in PWM modes
 * */

void Timer1_ForceMatch_B (void)
{
	/* set force bit for match B */
	TCCR1A_REG |= 0x04u ;
	return ;
}


/*****************************************************************************/

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

void Timer1_ICU_NoiseCanceler_Enable ( boolean Enable_Option )
{
	/* clear noise canceler enable bit */
	TCCR1B_REG &= 0x7F ;

	/* set noise canceler enable bit */
	TCCR1B_REG |=  Enable_Option << 7 ;
	return ;
}

/*****************************************************************************/

/* this function change the edge which trigger operation of ICU and set ICU flag which will generate interrupt if enabled
 * if i/p is true then Rising Edge will trigger ICU
 * during this function ICU interrupt flag could be set so ICU interrupt should be disabled before, this function clear ICU flag 

 * */

void Timer1_ICU_Select_RisingEdge ( boolean TriggerEdge_Option)
{

	/* clear trigger edge select bit */
	TCCR1B_REG &= 0xBF ;

	/* set trigger edge select bit */
	TCCR1B_REG |=  TriggerEdge_Option << 6 ;

	/* clear ICU interrupt flag */
	TIFR_REG = 0x20 ;


	return ;
}

/*****************************************************************************/

/* this function used to set operation mode according to ModeType
 *
 * every mode has its recommended environment to work in with specific advantages, restrictions, interrupts and equations
 * this parameters illustrated in Timer1.h file in modes I/Ps #defines
 *
 * for some modes could change TOP value
 * combination between mode and match signal set by Timer1_MatchSignal_A/B functions will determine shape of o/p signal on OC1X pins
 *
 * */

void Timer1_Set_Mode ( uint8 ModeType )
{
	/* clear mode select bits */
	TCCR1A_REG &= 0xF0u ;
	TCCR1B_REG &= 0xE7u ;

	/* set mode select bit to i/p option*/
	TCCR1A_REG |= ( ModeType & 0b11u ) ;
	TCCR1B_REG |= ( ModeType & 0b1100u ) << 1 ;

	/* clear TOP flags */
	Timer1_Variable_Top_Flag = FALSE ;
	Timer1_ORC1A_Top_Flag = FALSE ;

	/* check if mode selected use variable TOP variable or not */
	if( ( ( ModeType >= Timer1_PHASE_FREQUENCY_PWM_1 ) || (Timer1_CTC_1 == ModeType) )  )
	{
		/* set variable TOP flag to allow use of Timer1_SetTopValue function */
		Timer1_Variable_Top_Flag = TRUE ;

		/* check if OCR1A is used to determine TOP flag */
		if( ( 1 == (ModeType % 2) )  || (Timer1_CTC_1 == ModeType) )
		{
			/* set OCR1A TOP flag to prevent use of Timer1_SetCompare_A_Value function */
			Timer1_ORC1A_Top_Flag = TRUE ;
		}
		else
		{

		}
	}
	else
	{

	}

	return ;
}

/*****************************************************************************/

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
 *
 * */

StatusType Timer1_SetTopValue ( uint16 TopValue )
{
	StatusType ReturnResult = E_OK ;

	if( TRUE ==  Timer1_Variable_Top_Flag  )
	{
		if( TRUE == Timer1_ORC1A_Top_Flag )
		{
			OCR1A_REG = TopValue ;
		}
		else
		{
			ICR1_REG = TopValue ;
		}

	}
	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return ReturnResult ;
}

/*****************************************************************************/

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
StatusType Timer1_SetCompare_A_Value ( uint16 CompareValue )
{
	StatusType ReturnResult = E_OK ;

	if( FALSE == Timer1_ORC1A_Top_Flag  )
	{
		OCR1A_REG = CompareValue ;
	}
	else
	{
		ReturnResult = E_NOT_OK ;
	}

	return ReturnResult ;
}


/*****************************************************************************/


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
void Timer1_SetCompare_B_Value ( uint16 CompareValue )
{
	OCR1B_REG = CompareValue ;
}

/*****************************************************************************/

/* this function used to get current counter value this value won't be correct if counter is running in high speed */

uint16 Timer1_ReadCounterValue (void)
{
	return TCNT1_REG ;
}

/*****************************************************************************/

/* this function will set counter value
 * update will have immediate effect
 * if updated value = to compare value set by Timer1_MatchSignal_A/B value
 * then match will be ignored and no update for OC1A/B pin or trigger for interrupt
 * */

void Timer1_SetCounterValue ( uint16 CounterValue )
{
	TCNT1_REG = CounterValue ;
	return ;
}
/*****************************************************************************/

/* this function used to pass callback functions of interrupt specified by  InterruptType
 * callback function must be void (void) function
 * */

void Timer1_Interrupt_CallBack ( uint8 InterruptType , void (* FuncPtr) (void) )
{
	Timer1_FunctPtr_Array[ InterruptType - 2 ] = FuncPtr ;
	return ;
}

/*****************************************************************************/

/* this function is not necessary to use it just call combination of Timer1 function to ease initialization
 * process according to Timer1_Cfg.h file
 *
 * user must start timer operation after this function using Timer1_Start and initialize call back functions ;
 * */

void Timer1_Init (void)
{
	Timer1_ClockSource = Timer1_1_PRESCALER  ;
	
	/* disable all interrupts */
	TIMSK_REG &= 0x00 ;

	/* stop timer operation */
	Timer1_Stop() ;

	/* initiate counter value */
	Timer1_SetCounterValue( TIMER1_COUNTER_VALUE ) ;

	/* initiate clock source */
	Timer1_SetClock ( TIMER1_CLOCK_SOURCE ) ;

	/* initiate mode of operation  */
	Timer1_Set_Mode( TIMER1_MODE_SELECT ) ;

	/* initiate TOP value if mode don't use TOP value function won't affect operation*/
	Timer1_SetTopValue( TIMER1_TOP_VALUE ) ;

	/* initiate compare A value if mode don't use compare A value function won't affect operation*/
	Timer1_SetCompare_A_Value( TIMER1_COMPARE_VALUE_A ) ;

	/* initiate compare B value*/
	Timer1_SetCompare_B_Value( TIMER1_COMPARE_VALUE_B ) ;

	/* initiate match A signal */
	Timer1_MatchSignal_A( TIMER1_MATCH_SIGNAL_A ) ;

	/* initiate match B signal */
	Timer1_MatchSignal_B( TIMER1_MATCH_SIGNAL_B ) ;

	/* initiate noise canceler for ICU */
	Timer1_ICU_NoiseCanceler_Enable( TIMER1_ICU_NOISE_CANCELER ) ;

	/* initiate trigger edge  for ICU */
	Timer1_ICU_Select_RisingEdge( TIMER1_ICU_RISING_EDGE ) ;

	/* clear all interrupt flags*/
	TIFR_REG |= 0x3C ;

	/* initiate Timer1 interrupts */
	TIMSK_REG |= TIMER1_ICU_INTERRUPT << 5 ;
	TIMSK_REG |= TIMER1_COMPARE_A_INTERRUPT << 4 ;
	TIMSK_REG |= TIMER1_COMPARE_B_INTERRUPT << 3 ;
	TIMSK_REG |= TIMER1_OVER_FLOW_INTERRUPT << 2 ;



	return ;
}

/*****************************************************************************/

/* this function clear all Timer1_register and stop its operation */
void Timer1_deInit (void)
{
	/* clear all Timer1 register and stop its operation */

	TCCR1A_REG &= 0x00 ;
	TCCR1B_REG &= 0x00 ;
	TCNT1_REG &= 0x00 ;
	OCR1A_REG &= 0x00 ;
	OCR1B_REG &= 0x00 ;
	TIMSK_REG &= 0x00 ;
	TIFR_REG |= 0xFF ;

	return ;
}

/*****************************************************************************/

/*
ISR ( ICU1_ISR )
{
	Timer1_ICU_Value = ICR1_REG ;

	#if TIMER1_ICU_USER_ISR_ENABLE == TRUE

	( Timer1_FunctPtr_Array[3] ) () ;

	#endif

	return ;
}

*/
/*****************************************************************************/



ISR ( CMPA1_ISR )
{
	( Timer1_FunctPtr_Array[2] ) () ;

	return ;
}


/*****************************************************************************/



ISR ( CMPB1_ISR )
{
	( Timer1_FunctPtr_Array[1] ) () ;
	return ;
}


/*****************************************************************************/



ISR ( OVF1_ISR )
{
	( Timer1_FunctPtr_Array[0] ) () ;
	return ;
}


/*****************************************************************************/

