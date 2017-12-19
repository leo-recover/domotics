/*
 * \file USART.c
 *
 * Created: 02/10/2014 13:52:03
 * \author: Leonardo Ricupero
 */ 

#include <avr/io.h>
#include <usart.h>

#define BAUD_PRESCALE ((F_CPU / (16.0f * USART_BAUDRATE))UL -1)

/**
 * \brief Initializes the USART
 * 
 * Interrupt disabled
 *
 * \return void
 */
void Usart__Initialize(void)
{
	/*
	//! Double speed to reduce error
	UCSR0A = (1 << U2X0);
	*/
	//! Baud rate setting
	/*
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;
	*/
	UBRR0H = 0;
	UBRR0L = 15;
	//! Enable transmitter
	UCSR0B = (1 << TXEN0);
	
	//! Set frame format: 8 data bit, no parity, 1 stop bit
	UCSR0C = (3 << UCSZ00);
}

/**
 * \brief Transmit a char
 * 
 * \param c char to transmit
 * 
 * \return void
 */
void USART__TransmitChar(char c)
{
	//! Wait until the transmit buffer is ready
	while ((UCSR0A & (1 << UDRE0)) == 0) {};
	//! Send the char to transmit
	UDR0 = c;
}
