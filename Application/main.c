/*
 * 	Timer0_main.c
 *
 *  Created on: Apr 13, 2020
 *  Author: Bebo
 *
 */

# define F_CPU 16000000UL


#include "../Dio/Dio.h"
#include "../Adc/Adc.h"
#include "../AnalogComp/AnalogComp.h"
#include "../Timer1/Timer1.h"
#include "../ExtInterrupt/ExtInterrupt.h"
#include "../LCD/LCD.h"
#include <util/delay.h>


#define ICR1_REG	*( ( volatile uint16 * ) 0x46 )
#define PORTC_REG	*( (volatile uint8 *)(0x35) )


/* the interrupt function depend on vector sent to it go back to vector table   */
#define	ISR( vector )			void vector (void) __attribute__ ((signal,used, externally_visible)) ; \
								void vector (void)

#define ICU1_ISR			__vector_5


/*****************************************************************************/
/* number of ticks per 1 second */
#define WAV_DET_TIMER1_FREQUENCY	16000000u

/* determine number of samples taken by ADC */
#define WAV_DET_SAMPLES_NUMBER	40

/* determine number of samples which will be available to study */
#define WAV_DET_SAMPLES_ACTIVE	18

/* maximum number of ticks per signal cycle to be in medium frequency rang */
#define WAV_DET_SMALL_RANGE 600

/* number of ticks per signal cycle to be in high frequency rang */
#define WAV_DET_MED_RANGE 17000

/* ADC margin between samples to be considered as equal */
#define WAV_DET_SAMPLE_MARGIN	2


/*****************************************************************************/


/* used to save captured value by ICU */
uint8 WavDet_CapturedIndex = 0 ;
uint32 WavDet_CapturedValues [2] ;

/* used to save number of Timer1 overflows per cycle */
uint16 WavDet_PeriodOverflow = 0 ;

/* used to detect whether current process if for calculate period or duty cycle */
boolean WavDet_DutyCycle_Flag = FALSE ;

/* used to store number of ticks per signal cycle */
uint32 WavDet_SignalTicks = 0 ;

/* used to store number of tick which signal stayed in high state in per cycle */
uint32 WavDet_DC_Temp = 0  ;
uint32 WavDet_DC_Ticks = 0 ;


/*****************************/
/*****************************/


/* flag used with sample processing of high frequecy signals  */
boolean WavDet_HighFrequency = FALSE ;

/* number of ticks between samples*/
uint32 WavDet_SampleTicks = 0 ;

/* number of ticks after final overflow between samples  */
uint16 WavDet_Alarm_Compare = 0 ;

/* number of overflow per sample*/
uint16 WavDet_Alarm_Overflows = 0 ;
uint16 WavDet_Alarm_Overflows_Temp = 0 ;


/* used to store sample of input signal during 1 cycle */
uint16 WavDet_Samples_Array [WAV_DET_SAMPLES_NUMBER*2] ;
uint8 WavDet_Samples_Index = 0 ;


/*****************************/
/*****************************/

/* used with variable WavDet_SignalShape and function WavDet_ShapeDetect*/
#define WAV_DET_DC				0x00u
#define WAV_DET_SQUARE			0x01u
#define WAV_DET_RISING_SAW		0x02u
#define WAV_DET_FALLING_SAW		0x03u
#define WAV_DET_TRIANGLE		0x04u
#define WAV_DET_SIN				0x05u
#define WAV_DET_UNDEF			0x06u

/* variables to hold signal parameters */
uint8 WavDet_SignalShape ;
uint8 WavDet_SignalDutyCycle ;
uint32 WavDet_SignalPeak ;
uint32 WavDet_SignalSlope ;
uint32 WavDet_SignalFrequency ;

/* used to save number of samples under test after discard others */
uint8 WavDet_ActiveSamples ;

/*****************************************************************************/

char *Lcd_FirstLine = "Sh Fr    Pe  D/S";

/*****************************************************************************/

/* call back for interrupt 0, start wave detection process */
void WavDet_StartDetection (void) ;

/* used to calculate ticks per signal cycle */
void WavDet_Period (void) ;

/*  used to calculate ticks signal stay at logical HIGH per cycle  */
void WavDet_DutyCycle (void) ;

/* callback for Timer 1 overflow interrupt to calculate number of overflow per 1 cycle */
void WaveDet_Period_DC_OVF (void) ;


/*****************************/
/*****************************/


/* call back for interrupt 1, start and initialize wave sampling process */
void WavDet_StartSampling ( void ) ;

/* this function  will trigger Timer1_CompB flag after TickTime Ticks to trigger ADC conversion */
void WavDet_Alarm_TickTime ( uint32 TickTime ) ;

/* callback for Timer 1 overflow interrupt used to calculate time left for triggering ADC conversion  */
void WaveDet_Alarm_OVF (void) ;

/* callback of analog comparator interrupt used to convert ADC conversion triggering form Analog comparator to Timer1 Compare B */
void WavDet_AnalogComp (void) ;

/* callback for ADC  interrupt */
void WavDet_ADC_Conversion ( void ) ;

/* callback for timer1 Compare A interrupt , used in Sampling with CTC mode to reset counter to start from 0 after every sample */
void WavDet_Sample_Reset (void) ;


/*****************************/
/*****************************/

/* function to detect shape of signal from its samples and to calculate signal parameters */
void WavDet_Calculations (void) ;


/* used to indicate return of CheckSlope function */
#define WAV_DET_INCREASE_CHANGE 	0x00u
#define WAV_DET_DECREASE_CHANGE 	0x01u
#define WAV_DET_NO_CHANGE			0x02u
uint8 CheckSlope ( uint16 Num1, uint16 Num2 ) ;


/* this function used to detect Shape of signal under test
 * take i/p parameter pointer to first sample under test and return Shape Value */
uint8 WavDet_ShapeDetect ( uint16 * Sample_Ptr ) ;


/* this function evaluate parameters of signal with first sample of Sample_Ptr and has shape of Shape */
void WavDet_EvaluteParameter ( uint8 Shape , uint16 * Sample_Ptr ) ;

/* approximate results and display them on LCD */
void WavDet_DisplayResult (void) ;

/*****************************************************************************/


int main ( void )
{

	/* initiate used driver */
	Dio_Init () ;
	Adc_Init () ;
	AnalogCompartor_Init () ;
	Timer1_Init () ;
	interrupt_initialization () ;
	LCD_INIT() ;
	


	/* initiate used callback functions  */
	interrupt_external0( WavDet_StartDetection ) ;
	interrupt_external1( WavDet_StartSampling ) ;
	Timer1_Interrupt_CallBack ( Timer1_OVERFLOW_INTERRUPT, WaveDet_Period_DC_OVF ) ;
	Timer1_Interrupt_CallBack ( Timer1_COMPARE_A_INTERRUPT, WavDet_Sample_Reset ) ;

	AnalogComp_Interrupt_CallBack ( WavDet_AnalogComp ) ;
	Adc_Interrupt_CallBack (WavDet_ADC_Conversion) ;
	
	interrupt_ex0LocalState(INTERRUPT_ENABLE);
	
	LCD_PrintString(Lcd_FirstLine);

	
	while (1)
	{
		Dio_SetPinValue( 31, DIO_LOW ) ;
		Dio_SetPinValue( 26, DIO_LOW ) ;
		__asm("NOP") ;
		__asm("NOP") ;
		__asm("NOP") ;
		_delay_ms(125);
		Dio_SetPinValue( 31, DIO_HIGH ) ;
		Dio_SetPinValue( 26, DIO_HIGH ) ;
		__asm("NOP") ;
		__asm("NOP") ;
		__asm("NOP") ;
		_delay_ms(125);

	}

	
	return 0 ;
}


/*****************************************************************************/

/* call back for interrupt 0, start and initialize wave detection process */
void WavDet_StartDetection (void)
{
	/* disable user i/p till end of current signal detection */
	interrupt_ex0LocalState ( INTERRUPT_DISABLE ) ;

	/* initialize variables used during calculations */
	WavDet_PeriodOverflow = 0 ;
	WavDet_CapturedIndex = 0 ;
	WavDet_DutyCycle_Flag = FALSE ;
	WavDet_HighFrequency = FALSE ;
	WavDet_Alarm_Overflows = 0 ;
	WavDet_Samples_Index =  0 ;

	/* initialize ICU and Timer for detect number of Ticks per one Cycle */
	Timer1_SetCounterValue( 0x0000 ) ;
	Timer1_Set_Mode ( Timer1_NORMAL ) ;
	Timer1_SetCompare_B_Value ( 0x01 ) ;		/* to set compare B flag for ADC conversion  */
	Timer1_ICU_Select_RisingEdge (TRUE) ;

	/* Start timer operation */
	Timer1_Start () ;


	/* initialize and enable Analog Comparator to be i/p for ICU */
	AnalogComp_ICU_ACO_InputSelect ( TRUE ) ;
	AnalogComp_SwitchOff ( FALSE) ;

	/* enable overflow interrupt and clear its flag */
	Timer1_InterruptEnable( Timer1_INPUT_CAPTURE_INTERRUPT ) ;

	Timer1_Clear_InterruptFlag (Timer1_INPUT_CAPTURE_INTERRUPT) ;

	return ;
}

/*****************************************************************************/

/* call back for ICU interrupt, inside it find Period in ticks and duty cycle */
ISR (ICU1_ISR)
{
	while(1);
	/* store captured value and toggle index */
	WavDet_CapturedValues[WavDet_CapturedIndex] = ICR1_REG ;
	WavDet_CapturedIndex ^= 1 ;
	
	Timer1_InterruptEnable ( Timer1_OVERFLOW_INTERRUPT ) ;
	Timer1_Clear_InterruptFlag ( Timer1_OVERFLOW_INTERRUPT ) ;

	/* check for current process whether calculation for period or duty cycle ticks */
	if( ( 0 == WavDet_CapturedIndex ) && ( FALSE == WavDet_DutyCycle_Flag ) )
	{
		WavDet_Period () ;
	}
	else if ( TRUE == WavDet_DutyCycle_Flag )
	{
		WavDet_DutyCycle () ;
	}
	else
	{
	}
	
	return ;
}


/*****************************************************************************/

/* used to calculate ticks per signal cycle */
void WavDet_Period (void)
{
	/* check for timer1 overflow to calculate ticks per cycle */
	if ( WavDet_PeriodOverflow > 0)
	{
		WavDet_SignalTicks = (WavDet_CapturedValues[1] - WavDet_CapturedValues[0] ) + ( 0x10000 * WavDet_PeriodOverflow ) ;
	}
	else
	{
		WavDet_SignalTicks = WavDet_CapturedValues[1] - WavDet_CapturedValues[0] ;
	}
	/* store value used to calculate */
	WavDet_DC_Temp = WavDet_PeriodOverflow ;

	/* set duty cycle flag to start calculate duty cycle ticks */
	WavDet_DutyCycle_Flag = TRUE ;

	/* change ICU trigger edge to falling edge */
	Timer1_ICU_Select_RisingEdge (FALSE) ;

	return ;
}

/*****************************************************************************/

/*  used to calculate ticks signal stay at logical HIGH per cycle  */
void WavDet_DutyCycle (void)
{
	
	WavDet_DutyCycle_Flag = FALSE ;
		
	if ( WavDet_PeriodOverflow > 0)
	{
		WavDet_DC_Ticks = (WavDet_CapturedValues[0] - WavDet_CapturedValues[1] ) + ( 0x10000 * ( WavDet_PeriodOverflow - WavDet_DC_Temp ) ) ;
	}
	else
	{
		WavDet_DC_Ticks = WavDet_CapturedValues[0] - WavDet_CapturedValues[1] ;
	}
	
	while ( WavDet_DC_Ticks >= WavDet_SignalTicks) 
	{
		WavDet_DC_Ticks -= WavDet_SignalTicks ;
	}
	
	/* disable used interrupt during calculations */
	Timer1_InterruptDisable( Timer1_INPUT_CAPTURE_INTERRUPT ) ;
	Timer1_InterruptDisable( Timer1_OVERFLOW_INTERRUPT ) ;

	/* de-initialize and disable Analog Comparator to be i/p for ICU */
	AnalogComp_ICU_ACO_InputSelect ( FALSE ) ;
	AnalogComp_SwitchOff ( TRUE) ;


	/* stop timer operation */
	Timer1_Stop () ;

	interrupt_ex1LocalState(INTERRUPT_ENABLE);

	/* trigger software interrupt to begin sampling of signal */
	Dio_SetPinValue( 27, DIO_LOW ) ;
	Dio_SetPinValue( 27, DIO_HIGH ) ;


	return ;
}

/*****************************************************************************/

/* callback for Timer 1 overflow interrupt to calculate number of overflow per 1 cycle */
void WaveDet_Period_DC_OVF (void)
{
	WavDet_PeriodOverflow ++ ;
	return ;
}



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* call back for interrupt 1, start and initialize wave sampling process */
void WavDet_StartSampling ( void )
{

	interrupt_ex1LocalState(INTERRUPT_DISABLE);

	/* calculate number of ticks between samples */

	if ( WavDet_SignalTicks <= WAV_DET_SMALL_RANGE )
	{

		WavDet_SampleTicks = (WavDet_SignalTicks * 4) + (WavDet_SignalTicks / WAV_DET_SAMPLES_NUMBER ) ;
		WavDet_Alarm_TickTime ( WavDet_SampleTicks ) ;

	}
	else if ( WavDet_SignalTicks <= WAV_DET_MED_RANGE )
	{

		WavDet_SampleTicks = (WavDet_SignalTicks ) + (WavDet_SignalTicks / WAV_DET_SAMPLES_NUMBER ) ;
		WavDet_Alarm_TickTime ( WavDet_SampleTicks ) ;

	}
	else
	{

		WavDet_SampleTicks = (WavDet_SignalTicks / WAV_DET_SAMPLES_NUMBER ) ;
		WavDet_Alarm_TickTime ( WavDet_SampleTicks ) ;

	}
	/* enable and initialize Analog Comparator to trigger First ADC conversion */
	AnalogComp_SwitchOff ( FALSE) ;
	AnalogComp_Clear_InterruptFlag () ;
	AnalogComp_InterruptEnable () ;

	/* enable ADC and ADC interrupt and initialize  to auto trigger by analog comparator */
	Adc_SwitchOn (TRUE ) ;
	Adc_Clear_InterruptFlag () ;
	Adc_InterruptEnable () ;
	Adc_AutoTrigger_Source ( ADC_ANALOG_COM_TRIGGER ) ;

	/* ADC conversion will start when trigger happen */
	Adc_AutoTrigger ( TRUE) ;



	return ;
}


/*****************************************************************************/


/* this function  will trigger Timer1_CompB flag after TickTime Ticks to trigger ADC conversion */

void WavDet_Alarm_TickTime ( uint32 TickTime )
{

	/* calculate number of overflow and number of ticks after last overflow */
	WavDet_Alarm_Overflows = TickTime / 0x10000 ;
	WavDet_Alarm_Compare = ( TickTime -( WavDet_Alarm_Overflows * 0x10000 ) ) - 1 ;

	/* initialize timer1 setting
	 * don't clear compare B flag to prevent trigger of ADC conversion */
	Timer1_SetCounterValue ( 0x0000) ;
	Timer1_SetCompare_B_Value ( WavDet_Alarm_Compare ) ;

	WavDet_Alarm_Overflows_Temp = WavDet_Alarm_Overflows ;

	if ( 0 == WavDet_Alarm_Overflows)
	{
		/* initialize CTC mode to use of calculating samples case no overflow */
		Timer1_Set_Mode ( Timer1_CTC_1 ) ;
		Timer1_SetTopValue ( WavDet_Alarm_Compare ) ;

		/* clear Compare B flag to trigger ADC when match happen */
		Timer1_Clear_InterruptFlag( Timer1_COMPARE_B_INTERRUPT ) ;

		WavDet_HighFrequency = TRUE ;

	}
	else
	{
		/* initialize overflow interrupt setting to use of calculating samples */
		Timer1_Interrupt_CallBack( Timer1_OVERFLOW_INTERRUPT, WaveDet_Alarm_OVF ) ;
		Timer1_Clear_InterruptFlag( Timer1_OVERFLOW_INTERRUPT ) ;
		Timer1_InterruptEnable(Timer1_OVERFLOW_INTERRUPT) ;

		WavDet_HighFrequency = FALSE ;
	}


	return ;
}


/*****************************************************************************/

/* callback of analog comparator interrupt used to convert ADC conversion triggering form Analog comparator to Timer1 Compare B */
void WavDet_AnalogComp (void)
{
	/* start Timer to trigger ADC conversion */
	Timer1_Start() ;

	Adc_AutoTrigger_Source ( ADC_TIMER1_COM_B_TRIGGER ) ;

	/* disable AnalogCompartor and its interrupt */
	AnalogComp_InterruptDisable () ;
	AnalogComp_SwitchOff ( TRUE) ;

	return ;
}

/*****************************************************************************/


/* callback for Timer 1 overflow interrupt used to calculate time left for triggering ADC conversion  */
void WaveDet_Alarm_OVF (void)
{
	if ( 0 == (-- WavDet_Alarm_Overflows) )
	{
		/* clear CompareB flag to trigger ADC when next match happen */
		Timer1_Clear_InterruptFlag( Timer1_COMPARE_B_INTERRUPT ) ;
		WavDet_Alarm_Overflows = WavDet_Alarm_Overflows_Temp ;
		Timer1_Set_Mode ( Timer1_CTC_1 ) ;
		Timer1_SetTopValue ( WavDet_Alarm_Compare ) ;
		Timer1_Clear_InterruptFlag( Timer1_COMPARE_A_INTERRUPT ) ;
		Timer1_InterruptEnable(Timer1_COMPARE_A_INTERRUPT) ;
	}
	else
	{
	}
	return ;
}

/*****************************************************************************/


/* callback for ADC  interrupt */
void WavDet_ADC_Conversion ( void )
{
	/* read sample */
	WavDet_Samples_Array [ WavDet_Samples_Index ++  ] = ADC_Read_10() ;

	PORTC_REG ^= 1 ;

	if ( TRUE == WavDet_HighFrequency )
	{
		Timer1_Clear_InterruptFlag( Timer1_COMPARE_B_INTERRUPT ) ;

	}
	else
	{

	}

	/* check for number of samples taken */
	if( WavDet_Samples_Index >= WAV_DET_SAMPLES_NUMBER * 2 )
	{
		/* clear first sample which triggered by Analog Comparator */
		WavDet_Samples_Array [ 0 ] = 0 ;

		/* switch off ADC and disable Timer1 overflow interrupt */
		Adc_SwitchOn ( FALSE ) ;
		Adc_InterruptDisable () ;
		Timer1_InterruptDisable(Timer1_OVERFLOW_INTERRUPT) ;
		WavDet_Calculations () ;

	}
	else
	{

	}
	return ;
}



/*****************************************************************************/

/* callback for timer1 Compare A interrupt , used in Sampling with CTC mode to reset counter to start from 0 after every sample */
void WavDet_Sample_Reset (void)
{
	Timer1_Set_Mode ( Timer1_NORMAL ) ;
	Timer1_Clear_InterruptFlag( Timer1_OVERFLOW_INTERRUPT ) ;
	Timer1_InterruptDisable(Timer1_COMPARE_A_INTERRUPT) ;

	return ;
}



/*****************************************************************************/
/*****************************************************************************/


/* function to detect shape of signal from its samples and to calculate signal parameters */
void WavDet_Calculations (void)
{
	uint8 SampleIndex = 0 ;
	uint16  *Sample_Ptr = WavDet_Samples_Array    ;

	/* calculate frequency in Hz */
	WavDet_SignalFrequency = WAV_DET_TIMER1_FREQUENCY / WavDet_SignalTicks ;

	/* calculate Duty	 Cycle in 100%  */
	WavDet_SignalDutyCycle = (WavDet_DC_Ticks * 100) / WavDet_SignalTicks ;



	/* detect if signal is DC */
	for( SampleIndex = 0 ; SampleIndex < WAV_DET_SAMPLES_NUMBER*2 ; SampleIndex ++ )
	{
		/* detect first signal above margin */
		if ( (*Sample_Ptr)   <  WAV_DET_SAMPLE_MARGIN )
		{
			Sample_Ptr ++ ;
		}
		else
		{
			SampleIndex =  WAV_DET_SAMPLES_NUMBER * 2 ;
		}
	}

	SampleIndex = 0 ;

	/* get first active sample and determine its active samples number */
	while( Sample_Ptr [SampleIndex++] )
	{
	}
	if( SampleIndex > WAV_DET_SAMPLES_ACTIVE)
	{
	}
	else
	{
		Sample_Ptr = Sample_Ptr + SampleIndex ;
		while( Sample_Ptr [SampleIndex++] )
		{
		}
		while( ! (Sample_Ptr [SampleIndex++]) )
		{
		}
		Sample_Ptr = Sample_Ptr + SampleIndex - 1 ;

	}

	WavDet_ActiveSamples = WAV_DET_SAMPLES_ACTIVE ;

	/* determine shape of signal */
	WavDet_SignalShape = WavDet_ShapeDetect(Sample_Ptr) ;


	WavDet_EvaluteParameter ( WavDet_SignalShape, Sample_Ptr ) ;

	interrupt_ex0LocalState ( INTERRUPT_ENABLE ) ;

	__asm( "NOP" );
	
	WavDet_DisplayResult() ;

	return ;


}


/*****************************************************************************/


uint8 CheckSlope ( uint16 Num1, uint16 Num2 )
{
	uint8 SlopeState = 0 ;

	/* signal is rising up (SAW,TRI,SIN) */
	if ( Num1 > ( Num2 + WAV_DET_SAMPLE_MARGIN ) )
	{
		SlopeState = 0 ;
	}
	/* sample is falling down (SAW) */
	else if ( Num1 < ( Num2 - WAV_DET_SAMPLE_MARGIN ) )
	{
		SlopeState =  1 ;
	}
	/* signal is constant (SQUARE) */
	else
	{
		SlopeState =  2 ;
	}

	return SlopeState ;
}


/*****************************************************************************/

/* this function used to detect Shape of signal under test
 * take i/p parameter pointer to first sample under test and return Shape Value */
uint8 WavDet_ShapeDetect ( uint16 * Sample_Ptr )
{

	uint8 Shape = WAV_DET_UNDEF, SlopeState = 0, SlopeState2 = 0 , SampleIndex = 0 ;

	/* detect Shape of signal */
	if( Sample_Ptr == (WavDet_Samples_Array +  WAV_DET_SAMPLES_NUMBER - 1 ) ) /* if 1*/
	{
		/*  signal is a DC */
		Shape = WAV_DET_DC ;
	}
	else /* else 1 */
	{

		/* determine initial slope of the signal */
		SlopeState = CheckSlope( Sample_Ptr [1], Sample_Ptr [0] ) ;

		/* detect if signal has logical values  and its shape */
		for( SampleIndex = 0 ; SampleIndex < WavDet_ActiveSamples-1 ; SampleIndex ++ )
		{
			/* all active signals must be above margin 	*/
			if ( (Sample_Ptr [SampleIndex+1] > WAV_DET_SAMPLE_MARGIN)) /* if 2 */
			{
				/* signal is rising up (SAW,TRI,SIN) */
				if ( WAV_DET_INCREASE_CHANGE == SlopeState ) /* if 3 */
				{
					/* check if signal has positive slope  */
					if ( ( WAV_DET_INCREASE_CHANGE == CheckSlope( Sample_Ptr [SampleIndex+1], Sample_Ptr [SampleIndex] ) )  && ( WAV_DET_INCREASE_CHANGE == SlopeState2) )	/* if 4 */
					{
						/* if signal has just positive slope in all of its parts then it will be SAW signal */
						Shape = WAV_DET_RISING_SAW ;

					}
					/* signal has negative slope after positive slope then it might be triangle or SIN */
					else if ( WAV_DET_DECREASE_CHANGE == CheckSlope( Sample_Ptr [SampleIndex+1], Sample_Ptr [SampleIndex] ) ) /* else if 4 */
					{
						/* to check if signal has positive scope after negative one then it will be undefined signal */
						SlopeState2 = WAV_DET_DECREASE_CHANGE ;

						if ( WAV_DET_INCREASE_CHANGE == CheckSlope( Sample_Ptr[1] - Sample_Ptr [0], Sample_Ptr [3] - Sample_Ptr [2] ) ) /* if 5*/
						{
							/* signal has increasing slope (accelerate) then it will be sin */
							Shape = WAV_DET_SIN ;
						}
						else
						{
							/* signal has constant negative slope after positive slope then it will be triangle  */
							Shape = WAV_DET_TRIANGLE ;

						} /*else 5 */

					}/* else if 4 */
					else
					{
						/* signal has 0 slope after positive on the signal  is undefined */
						SampleIndex = WavDet_ActiveSamples ;
						Shape = WAV_DET_UNDEF ;

					} /* else  4 */

				} /* if 3  */
				else if ( WAV_DET_DECREASE_CHANGE == SlopeState )  /* else if 3 */
				{
					/* signal has negative slope then if this continue for all samples then signal will be SAW */
					if ( WAV_DET_DECREASE_CHANGE == CheckSlope( Sample_Ptr [SampleIndex+1], Sample_Ptr [SampleIndex] ) ) /* if 4 */
					{
						Shape = WAV_DET_FALLING_SAW ;
					}
					else
					{
						/* signal has 0 slope after negative one then signal is undefined  */
						SampleIndex = WavDet_ActiveSamples ;
						Shape = WAV_DET_UNDEF ;

					} /* else 4 */

				} /* else if 3 */
				else
				{
					/* signal has 0 slope if  then if this continue for all samples then signal will be Square */
					if ( WAV_DET_NO_CHANGE == CheckSlope( Sample_Ptr [SampleIndex+1], Sample_Ptr [SampleIndex] ) ) /* if 4 */
					{
						Shape = WAV_DET_SQUARE ;
					}
					else
					{
						/* signal has non zero slope after zero one then signal is undefined  */
						SampleIndex = WavDet_ActiveSamples ;
						Shape = WAV_DET_UNDEF ;

					} /* else 4 */

				} /* else 3 */

			} /* if  2*/
			else
			{
				/* sample is undefined */
				SampleIndex = WavDet_ActiveSamples ;
				Shape = WAV_DET_UNDEF ;

			} /* else 2 */

		} /* for */

	} /* else 1 */


	return Shape ;
}

/*****************************************************************************/

/* this function evaluate parameters of signal with first sample of Sample_Ptr and has shape of Shape */
void WavDet_EvaluteParameter ( uint8 Shape , uint16 * Sample_Ptr )
{

	uint8 SampleIndex = 0 , SampleIndex_2 = 0 ;
	uint32 SampleAverage = 0 ;

	/* determine parameter of the wave*/
	switch ( Shape )
	{
		case WAV_DET_DC :
		{
			break ;
		}

		case WAV_DET_SQUARE :
		{
			/* take average of active samples */
			for( SampleIndex = 0 ; SampleIndex < WavDet_ActiveSamples ; SampleIndex ++ )
			{
				SampleAverage += Sample_Ptr [SampleIndex] ;
			}
			/* evaluate peak in mili volt */
			WavDet_SignalPeak = (SampleAverage * 5000 )/ (1024 * WavDet_ActiveSamples) ;

			break ;
		}

		case WAV_DET_RISING_SAW :
		{
			/* take average of positive slope of SAW */
			for( SampleIndex = 0 ; SampleIndex < WavDet_ActiveSamples-1 ; SampleIndex ++ )
			{
				SampleAverage += Sample_Ptr [SampleIndex+1] - Sample_Ptr [SampleIndex] ;
			}
			WavDet_ActiveSamples -- ;
			SampleAverage = SampleAverage *  WAV_DET_SAMPLES_NUMBER / 2 ;

			/* evaluate peak in mili volt */
			WavDet_SignalPeak = (SampleAverage * 5000 )/ (1024 * WavDet_ActiveSamples) ;

			/* evaluate slope in volt/sec */
			WavDet_SignalSlope = ( WavDet_SignalPeak * 4 * 1e6 ) / (WavDet_SignalTicks  * 125) ;

			break ;
		}

		case WAV_DET_FALLING_SAW :
		{
			/* take average of negative slope of SAW */
			for( SampleIndex = 0 ; SampleIndex < WavDet_ActiveSamples-1 ; SampleIndex ++ )
			{
				SampleAverage += Sample_Ptr [SampleIndex] - Sample_Ptr [SampleIndex+1] ;
			}
			WavDet_ActiveSamples -- ;
			SampleAverage = SampleAverage *  WAV_DET_SAMPLES_NUMBER / 2 ;

			/* evaluate peak in mili volt */
			WavDet_SignalPeak = (SampleAverage * 5000 )/ (1024 * WavDet_ActiveSamples) ;

			/* evaluate slope in volt/sec */
			WavDet_SignalSlope = ( WavDet_SignalPeak * 4 * 1e6 ) / (WavDet_SignalTicks  * 125) ;

			break ;
		}


		case WAV_DET_TRIANGLE :
		{

			/* get the first sample in negative edge to start with  */
			for( SampleIndex = 0 ; SampleIndex < WavDet_ActiveSamples -1 ; SampleIndex ++ )
			{
				if ( Sample_Ptr [SampleIndex+1] > Sample_Ptr [SampleIndex] )
				{
				}
				else
				{
					SampleIndex_2 = SampleIndex + 1 ;
					SampleIndex = WavDet_ActiveSamples ;
				}
			}

			/* take average of negative slope of triangle */
			for(  SampleIndex = SampleIndex_2 ; SampleIndex < WavDet_ActiveSamples  ; SampleIndex ++ )
			{
				SampleAverage += Sample_Ptr [SampleIndex] - Sample_Ptr [SampleIndex+1] ;
			}
			SampleAverage = SampleAverage *  WAV_DET_SAMPLES_NUMBER / 4 ;

			WavDet_ActiveSamples -= SampleIndex_2 ;
			/* evaluate peak in mili volt */
			WavDet_SignalPeak = (SampleAverage * 5000 )/ (1024 * WavDet_ActiveSamples ) ;

			/* evaluate slope in volt/sec */
			WavDet_SignalSlope = ( WavDet_SignalPeak * 8 * 1e6 ) / (WavDet_SignalTicks  * 125) ;
			break ;

		}

		case WAV_DET_SIN :
		{
			/* get the highest sample */
			for( SampleIndex = 0 ; SampleIndex < WavDet_ActiveSamples -1 ; SampleIndex ++ )
			{
				if ( Sample_Ptr [SampleIndex+1] > Sample_Ptr [SampleIndex] )
				{
					SampleAverage = Sample_Ptr [ SampleIndex+ 1 ] ;
				}
				else
				{
					SampleIndex = WavDet_ActiveSamples ;
				}
			}

			/* evaluate peak in mili volt */
			WavDet_SignalPeak = (SampleAverage * 5000 )/ 1024 ;

			break ;
		}
		case WAV_DET_UNDEF :
		{
			break ;

		} /* case */

	} /* switch */

	return ;
}


/*****************************************************************************/

void WavDet_DisplayResult (void)
{

	   	LCD_SendCommand(SecondLine);

	   	/* display shpae then frequency then peak then duty cycle  */
		switch ( WavDet_SignalShape )
		{
			case 0 :
			{
				LCD_SendData('D');
				LCD_SendData('C');
				LCD_SendData(' ');
				
				break ;
			}
			case 1 :
			{
				LCD_SendData('S');
				LCD_SendData('Q');
				LCD_SendData(' ');
				
				break ;
			}
			case 2 :
			{
				LCD_SendData('S');
				LCD_SendData('+');
				
				break ;
			}
			case 3 :
			{
				LCD_SendData('S');
				LCD_SendData('-');
				
				break ;
			}
			case 4 :
			{
				
				LCD_SendData('T');
				LCD_SendData('R');
				
				break ;
			}
			case 5 :
			{
				
				LCD_SendData('S');
				LCD_SendData('I');
				
				break ;
			}
			case 6 :
			{
				LCD_SendData('U');
				LCD_SendData('N');
				LCD_SendData(' ');
				
				break ;
			}
		}
		
		LCD_SendData(' ');

	
		if ( WavDet_SignalFrequency >= 1000 )
		{
			LCD_PrintNumber(WavDet_SignalFrequency/1000);
			LCD_SendData('.');
			LCD_PrintNumber( (WavDet_SignalFrequency%1000) / 100 );		
			LCD_SendData('K');

		}
		else
		{
			LCD_PrintNumber(WavDet_SignalFrequency);
			LCD_SendData(' ');

		}

		LCD_SendData(' ');

		LCD_PrintNumber(WavDet_SignalPeak/1000);
		LCD_SendData('.');
		LCD_PrintNumber( (WavDet_SignalPeak%1000) / 100 );
		
		LCD_SendData(' ');

	    if( WavDet_SignalShape == 0 )
		{
			LCD_PrintNumber(WavDet_SignalDutyCycle);
			LCD_SendData(' ');
	    }
	    else if ( WavDet_SignalShape < 5 )
	    {
	    	while ( WavDet_SignalSlope > 1000 )
	    	{
	    		WavDet_SignalSlope/= 10 ;
	    	}
			LCD_PrintNumber(WavDet_SignalSlope);
			LCD_SendData(' ');
	    }





	return ;
}

/*****************************************************************************/

