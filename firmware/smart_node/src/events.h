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
#include <avr/interrupt.h>
#include <radio.h>
#include <usart.h>

void INT0_interrupt_init(void);

#endif /* EVENTS_H_ */