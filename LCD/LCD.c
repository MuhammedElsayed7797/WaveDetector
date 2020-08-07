/*
 * LCD.c
 *
 *  Created on: Sep 7, 2019
 *      Author: ELMASREYA
 */
 
 # define F_CPU 16000000UL

#include "../Common/Platform_Types.h"
#include "../Dio/Dio.h"
#include "LCD.h"
#include <util/delay.h>

#define LCD_DELAY	5
void LCD_INIT(void)
{
	DIO_SetPortDirection(LCDPort,DIO_OUTPUT);
	Dio_SetPinDir(LCDRS,DIO_OUTPUT);
	Dio_SetPinDir(LCDRW,DIO_OUTPUT);
	Dio_SetPinDir(LCDEnable,DIO_OUTPUT);
	LCD_SendCommand(DisplayWithCursorAndBlinking);
	LCD_SendCommand(ReturnHome);
	LCD_SendCommand(FunctionSet);
	LCD_SendCommand(ClearLCD);
}

void LCD_SendCommand(uint8 Command)
{
	Dio_SetPinValue(LCDRS,DIO_LOW);
	Dio_SetPinValue(LCDRW,DIO_LOW);
	_delay_ms(LCD_DELAY);
	Dio_SetPinValue(LCDEnable,DIO_HIGH);
	_delay_ms(LCD_DELAY);
	DIO_SetPortValue(LCDPort,Command);
	_delay_ms(LCD_DELAY);
	Dio_SetPinValue(LCDEnable,DIO_LOW);
	_delay_ms(LCD_DELAY);
}

void LCD_SendData(char Char)
{
	Dio_SetPinValue(LCDRS,DIO_HIGH);
	Dio_SetPinValue(LCDRW,DIO_LOW);
	_delay_ms(LCD_DELAY);
	Dio_SetPinValue(LCDEnable,DIO_HIGH);
	_delay_ms(LCD_DELAY);
	DIO_SetPortValue(LCDPort,Char);
	_delay_ms(LCD_DELAY);
	Dio_SetPinValue(LCDEnable,DIO_LOW);
	_delay_ms(LCD_DELAY);
}

void LCD_PrintString(char *String)
{
	for (; (*String)!='\0'; String++)
	{
		LCD_SendData(*String);
	}
}

void LCD_PrintNumber(long int Number)
{
	int i,j,sign=0;
	uint16 arr[100];
	if (Number<0)
	{
		Number*= -1;
		sign = 1;
	}
	for (i=0; Number!=0; i++)
	{
		arr[i]=(Number%10)+'0';
		Number/=10;
	}
	if (sign==1)
	{
		LCD_SendData('-');
	}
	for (j=(i-1); j>=0; j--)
	{
		LCD_SendData(arr[j]);
	}
}
