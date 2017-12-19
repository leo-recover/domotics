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

#include <onewire.h>

// 'tick' values
int A,B,C,D,E,F,G,H,I,J;

#define DELAY_A (6 * 4)
#define DELAY_B (64 * 4)
#define DELAY_C (60 * 4)
#define DELAY_D (10 * 4)
#define DELAY_E (9 * 4)
#define DELAY_F (55 * 4)
#define DELAY_G (0 * 4)
#define DELAY_H (480 * 4)
#define DELAY_I (70 * 4)
#define DELAY_J (410 * 4)


/**
 * \brief Delays a time multiple of 0.25us
 * 
 * \param tick: number of multiples
 * 
 * \return void
 */
void OWtickDelay(uint16_t tick)
{
	// If we are at 16MHz then _delay_loop_2 (4 clock cycles) call delays of 0.25us
	_delay_loop_2(tick);
}


/**
 * \brief Set the 1-Wire timing to 'standard' (standard=1) or 'overdrive' (standard=0).
 * 
 * \param standard
 * 
 * \return void
 */
void OWSetSpeed(uint8_t standard)
{
	// Adjust tick values depending on speed
	if (standard)
	{
		// Standard Speed
		A = 6 * 4;
		B = 64 * 4;
		C = 60 * 4;
		D = 10 * 4;
		E = 9 * 4;
		F = 55 * 4;
		G = 0;
		H = 480 * 4;
		I = 70 * 4;
		J = 410 * 4;
	}
	else
	{
		// Overdrive Speed
		A = 1.5 * 4;
		B = 7.5 * 4;
		C = 7.5 * 4;
		D = 2.5 * 4;
		E = 0.75 * 4;
		F = 7 * 4;
		G = 2.5 * 4;
		H = 70 * 4;
		I = 8.5 * 4;
		J = 40 * 4;
	}
}

/**
 * \brief Generate a 1-Wire reset
 * 
 * \return 1 if no presence detect was found, return 0 otherwise.
 *
 */
uint8_t OWTouchReset(void)
{
	uint8_t result;
	cli();
	OWtickDelay(G);
	// Drives DQ low
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PORTD7);
	OWtickDelay(H);
	// Releases the bus
	DDRD &= ~(1 << DDD7);
	OWtickDelay(I);
	// Sample for presence pulse from slave
	result = PIND & (1 << PIND7); 
	// Complete the reset sequence recovery
	OWtickDelay(J);
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
void OWWriteBit(uint8_t bit)
{
	cli();
	if (bit)
	{
		// Write '1' bit
		// Drives DQ low
		DDRD |= (1 << DDD7);
		PORTD &= ~(1 << PORTD7);
		OWtickDelay(A);
		// Releases the bus
		DDRD &= ~(1 << DDD7);
		OWtickDelay(B); // Complete the time slot and 10us recovery
	}
	else
	{
		// Write '0' bit
		// Drives DQ low
		DDRD |= (1 << DDD7);
		PORTD &= ~(1 << PORTD7);
		OWtickDelay(C);
		// Releases the bus
		DDRD &= ~(1 << DDD7);
		OWtickDelay(D);
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
uint8_t OWReadBit(void)
{
	uint8_t result;
	cli();
	// Drives DQ low
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PORTD7);
	OWtickDelay(A);
	// Releases the bus
	DDRD &= ~(1 << DDD7);
	OWtickDelay(E);
	// Sample the bit value from the slave
	result = PIND & (1 << PIND7);
	OWtickDelay(F); // Complete the time slot and 10us recovery
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
		OWWriteBit(data & 0x01);
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
		if (OWReadBit())
		result |= 0x80;
	}
	return result;
}


/**
 * \brief Write a 1-Wire data byte and return the sampled result.
 * 
 * \param data
 * 
 * \return uint8_t
 */
/*
uint8_t OWTouchByte(uint8_t data)
{
	uint8_t loop, result=0;
	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;
		// If sending a '1' then read a bit else write a '0'
		if (data & 0x01)
		{
			if (OWReadBit())
			result |= 0x80;
		}
		else
		OWWriteBit(0);
		// shift the data byte for the next bit
		data >>= 1;
	}
	return result;
}
*/

/**
 * \brief Write a block 1-Wire data bytes and return the sampled result in the same buffer.
 * 
 * \param data
 * \param data_len
 * 
 * \return void
 */
/*
void OWBlock(unsigned char *data, uint8_t data_len)
{
	uint8_t loop;
	for (loop = 0; loop < data_len; loop++)
	{
		data[loop] = OWTouchByte(data[loop]);
	}
}
*/

/**
 * \brief Set all devices on 1-Wire to overdrive speed. Return '1' if at least one
 *  overdrive capable device is detected.
 * 
 * \param data
 * \param data_len
 * 
 * \return uint8_t
 */
/*
uint8_t OWOverdriveSkip(unsigned char *data, uint8_t data_len)
{
	// set the speed to 'standard'
	OWSetSpeed(1);
	// reset all devices
	if (OWTouchReset()) // Reset the 1-Wire bus
	return 0; // Return if no devices found
	// overdrive skip command
	OWWriteByte(0x3C);
	// set the speed to 'overdrive'
	OWSetSpeed(0);
	// do a 1-Wire reset in 'overdrive' and return presence result
	return OWTouchReset();
}
*/
