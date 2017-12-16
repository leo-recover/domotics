/*
 * \file events.h
 *
 * \date 08/11/2014 15:10:32
 * \author Leonardo Ricupero
 */ 


#ifndef EVENTS_H_
#define EVENTS_H_

#include <avr/io.h>
#include <util/delay.h>
#include "USART.h"
#include "nRF24L01.h"
#include <avr/interrupt.h>

void INT0_interrupt_init(void);

#endif /* EVENTS_H_ */