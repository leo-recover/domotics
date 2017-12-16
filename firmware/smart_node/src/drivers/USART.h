/*
 * \file USART.h
 *
 * Created: 02/10/2014 13:52:21
 * \author: Leonardo Ricupero
 */ 


#ifndef USART_H_
#define USART_H_

#include "SMART_NODE.h"

#define BAUDRATE 9600
#define BAUD_PRESCALE ((F_CPU / (16 * BAUDRATE)) -1)

void USARTTransmitChar(char c);
void USARTInit(void);

#endif /* USART_H_ */
