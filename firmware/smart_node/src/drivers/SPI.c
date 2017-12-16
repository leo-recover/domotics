/*
 * SPI.c
 *
 * Created: 22/09/2014 17:18:08
 *  \author: Leonardo Ricupero
 */ 

#include "SPI.h"

/**
 * \brief Initialize SPI in master mode
 * 
 * Method implemented as shown on the data sheet with
 * CE and CSN control for nRFL01
 * 
 * \return void
 */
void SPIInitMaster(void)
{
	/* Set MOSI ,SCK, CE and CSN as output, all other pin as input*/
	DDRB = (1<<DDB5) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	// Set CSN high to start with, because nothing has to be transmitted
	PORTB |= 1 << PORTB2;
	// Set CE low to start with, because nothing has to be transmitted
	PORTB &= ~(1<<PORTB1);
}

/**
 * \brief Byte transmission via SPI
 * 
 * \param cData char to transmit
 * 
 * \return char
 */
char SPIWriteByte(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	// Return what's been received from the module
	return SPDR;
}