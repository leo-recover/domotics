/*
 * \file OW.c
 *
 * The functions in this file 
 * 
 * @todo complete rewrite the driver in order to be without use
 * of delays. A FSM could be used, in conjunction with a TC.
 *
 * Created: 21/10/2014 13:18:52
 * \author Leonardo Ricupero
 */ 

#include <util/delay_basic.h>
#include "onewire.h"

// 'tick' values
int A,B,C,D,E,F,G,H,I,J;

// Ticks for a delay of 1 microsecond using _delay_loop_2 (clock cycles)
#define TICKS_PER_MICROSECOND 4 // 4 * 0.25 us = 1 us at 16 MHz

#define DELAY_A (6 * TICKS_PER_MICROSECOND)
#define DELAY_B (64 * TICKS_PER_MICROSECOND)
#define DELAY_C (60 * TICKS_PER_MICROSECOND)
#define DELAY_D (10 * TICKS_PER_MICROSECOND)
#define DELAY_E (9 * TICKS_PER_MICROSECOND)
#define DELAY_F (55 * TICKS_PER_MICROSECOND)
#define DELAY_G (0 * TICKS_PER_MICROSECOND)
#define DELAY_H (480 * TICKS_PER_MICROSECOND)
#define DELAY_I (70 * TICKS_PER_MICROSECOND)
#define DELAY_J (410 * TICKS_PER_MICROSECOND)


/**
 * \brief Generate a 1-Wire reset
 * 
 * \return 1 if no presence detect was found, return 0 otherwise.
 *
 */
uint8_t Onewire__DetectPresence(void)
{
	uint8_t result;
	cli();
	_delay_loop_2(G);
	// Drives DQ low
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PORTD7);
	_delay_loop_2(H);
	// Releases the bus
	DDRD &= ~(1 << DDD7);
	_delay_loop_2(I);
	// Sample for presence pulse from slave
	result = PIND & (1 << PIND7); 
	// Complete the reset sequence recovery
	_delay_loop_2(J);
	sei();
	// Return sample presence pulse result 
	return result; 
}

/**
 * \brief Send a 1-Wire write bit. Provide 10us recovery time.
 * 
 * \param bit
 * 
 * \return void
 */
void Onewire__WriteBit(uint8_t bit)
{
	cli();
	if (bit)
	{
		// Write '1' bit
		// Drives DQ low
		DDRD |= (1 << DDD7);
		PORTD &= ~(1 << PORTD7);
		_delay_loop_2(A);
		// Releases the bus
		DDRD &= ~(1 << DDD7);
		_delay_loop_2(B); // Complete the time slot and 10us recovery
	}
	else
	{
		// Write '0' bit
		// Drives DQ low
		DDRD |= (1 << DDD7);
		PORTD &= ~(1 << PORTD7);
		_delay_loop_2(C);
		// Releases the bus
		DDRD &= ~(1 << DDD7);
		_delay_loop_2(D);
	}
	sei();
}

/**
 * \brief Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
 * 
 * \param 
 * 
 * \return uint8_t
 */
uint8_t Onewire__ReadBit(void)
{
	uint8_t result;
	cli();
	// Drives DQ low
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PORTD7);
	_delay_loop_2(A);
	// Releases the bus
	DDRD &= ~(1 << DDD7);
	_delay_loop_2(E);
	// Sample the bit value from the slave
	result = PIND & (1 << PIND7);
	_delay_loop_2(F); // Complete the time slot and 10us recovery
	sei();
	return result;
}

/**
 * \brief Write 1-Wire data byte.
 * 
 * \param data
 * 
 * \return void
 */
void OWWriteByte(uint8_t data)
{
	uint8_t loop;
	// Loop to write each bit in the byte, LS-bit first
	for (loop = 0; loop < 8; loop++)
	{
		Onewire__WriteBit(data & 0x01);
		// shift the data byte for the next bit
		data >>= 1;
	}
}

/**
 * \brief Read 1-Wire data byte and return it.
 * 
 * \param 
 * 
 * \return uint8_t
 */
uint8_t OWReadByte(void)
{
	uint8_t loop, result=0;
	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;
		// if result is one, then set MS bit
		if (Onewire__ReadBit())
		result |= 0x80;
	}
	return result;
}
