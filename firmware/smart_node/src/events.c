/**
 * @file events.c
 *
 * @date 08/11/2014 15:10:21
 * @author Leonardo Ricupero
 */ 

#include "events.h"

// temporary variable to store data
uint8_t *data;

/**
 * \brief Initialize external IRQ on PD2
 *
 * 
 * \return void
 */
void INT0_interrupt_init(void)
{
	DDRD &= ~(1 << DDD2);
	// INT0 falling edge PD2
	EICRA |=  (1<<ISC01);
	EICRA  &=  ~(1<<ISC00);
	// Enable IRQ INT0
	EIMSK |=  (1<<INT0);
}

/**
 * @brief ISR on INT0
 *
 * This is called when successful data receive or transmission
 * happened
 *
 * @todo Avoid 0.5 sec delay
 * 
 * @return void
 */
ISR(INT0_vect)
{
	PORTB &= ~(1 << PORTB1);
	
	// LED to show success
	PORTB |= (1 << PORTB0);
	_delay_ms(500);
	PORTB &= ~(1 << PORTB0);
	// Read data from RX FIFO
	data = RF24ReadWrite(R, R_RX_PAYLOAD, data, 32);
	// Print data to console
	for (int i = 0; i < 32; i++)
	{
		USART__TransmitChar(data[i]);
	}
	// Clear IRQ masks in STATUS register
	RF24ResetIRQ();
}