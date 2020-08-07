/*
 * 	Dio.h
 *
 *  Created on: Apr 18, 2020
 *  Author: Bebo
 */

#ifndef DIO_H_
#define DIO_H_


#include "../Common/Utils.h"
#include "../Dio/Dio_Cfg.h"


#define DIO_INPUT	0
#define DIO_OUTPUT 	1

#define DIO_LOW 	0
#define DIO_HIGH 	1

#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3

void Dio_Init(void);


void DIO_SetPortDirection(uint8 Port_number,uint8 Direction);

void DIO_SetPortValue(uint8 Port_number,uint8 Value);

void Dio_SetPinDir(uint8 copy_uint8_PinNum,uint8 copy_uint8_PinDir);


void Dio_SetPinValue(uint8 copy_uint8_PinNum,uint8 copy_uint8_Pinvalue);


uint8 Dio_GetPinValue(uint8 copy_uint8_PinNum);


#endif /* DIO_H_ */
