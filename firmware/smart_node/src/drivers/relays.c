/**
 * \file relays.c
 *
 * \date 21/11/2014 13:02:09
 * \author Leonardo Ricupero
 */ 

#include "relays.h"

/**
 * \brief Initializes the relays to a known state, as reported on the
 * silkscreen of the board, providing a 4ms pulse on the reset coil
 * 
 * \return void
 */
void relaysInit(void)
{
	//PORTD &= ~(1 << PD3) & ~(1 << PD4) & ~(1 << PD5) & ~(1 << PD6);
	// Control pin as outputs
	DDRD |= (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6);
	// Init relay 0 to reset state
	PORTD |= (1 << PD4);
	_delay_ms(4);
	PORTD &= ~(1 << PD4);
	// Init relay 1 to reset state
	PORTD |= (1 << PD6);
	_delay_ms(4);
	PORTD &= ~(1 << PD6);
}

