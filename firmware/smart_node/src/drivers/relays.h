/*
 * relays.h
 *
 * Created: 21/11/2014 13:02:43
 *  Author: Leo
 */ 


#ifndef RELAYS_H_
#define RELAYS_H_

#include <avr/io.h>
#include <util/delay.h>

void relaysInit(void);

/**
 * \brief Puts the relay 0 in the set state
 * 
 * \return void
 */
static inline void relay0Set(void)
{
	PORTD |= (1 << PD3);
	_delay_ms(4);
	PORTD &= ~(1 << PD3);
}

/**
 * \brief Puts the relay 1 in the set state
 * 
 * \return void
 */
static inline void relay1Set(void)
{
	PORTD |= (1 << PD5);
	_delay_ms(4);
	PORTD &= ~(1 << PD5);
}

/**
 * \brief Puts the relay 0 in the reset state
 * 
 * \return void
 */
static inline void relay0Reset(void)
{
	PORTD |= (1 << PD4);
	_delay_ms(4);
	PORTD &= ~(1 << PD4);
}

/**
 * \brief Puts the relay 1 in the reset state
 * 
 * \return void
 */
static inline void relay1Reset(void)
{
	PORTD |= (1 << PD6);
	_delay_ms(4);
	PORTD &= ~(1 << PD6);
}

#endif /* RELAYS_H_ */