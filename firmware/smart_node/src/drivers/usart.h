/*
 * \file USART.h
 *
 * Created: 02/10/2014 13:52:21
 * \author: Leonardo Ricupero
 */ 


#ifndef USART_H_
#define USART_H_

#include "micro.h"

#define USART_BAUDRATE 9600

void USART__TransmitChar(char c);
void Usart__Initialize(void);

#endif /* USART_H_ */
