/*
 * LCD.h
 *
 *  Created on: Sep 7, 2019
 *      Author: ELMASREYA
 */

#ifndef LCD_H_
#define LCD_H_

#define LCDPort 	PORT2
#define LCDEnable 	13
#define LCDRW 		14
#define LCDRS 		15

//Commands
#define ClearLCD 0x01

#define ReturnHome 0x02

//choose mode from the following
#define NOshiftPlusINC 0x06
#define NOshiftPlusDEC 0x04
#define shiftPlusINC 0x07
#define shiftPlusDEC 0x05

//choose display control from the following
#define DisplayWithCursorAndBlinking 0x0f
#define DisplayWithCursorAndNOBlinking 0x0e
#define DisplayWithOUTCursorORBlinking 0x0c
#define NODisplay 0x08

//choose from the following
#define ShiftRight 0x1c
#define ShiftLeft 0x18
#define MoveCursor 0x10

#define FirstLine 0x80
#define SecondLine (0x80|0x40)

#define FunctionSet 0x38


void LCD_INIT(void);

//write command name as defined above
void LCD_SendCommand(uint8 Command);

//write the char between single quotes
void LCD_SendData(char Char);

void LCD_PrintString(char *String);

void LCD_PrintNumber(long int Number);

#endif /* LCD_H_ */
