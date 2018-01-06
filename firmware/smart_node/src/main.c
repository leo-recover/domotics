/**
 * \file SMART_NODE.c
 *
 * \brief SMART NODE v0.1
 *
 * Firmware of the radio module SMART NODE, based on MCU ATMEGA328P
 * and radio module nRF24L01+, with temperature sensor DS18B20
 *
 * @todo Need to update the while loop to get rid of the delays.
 * Could use the WDT or timer in order to wait for acquisition to start.
 * This could be done once the 1 wire com driver gets updated.
 *	
 * \date: 22/09/2014 12:40:22
 * \author: Leonardo Ricupero
 */ 


#include "micro.h"
#include "usart.h"
#include "spi.h"
#include <util/delay.h>
#include "radio.h"
#include "temp_sensor.h"
#include "thermostat.h"
#include "parameters.h"
#include "events.h"
#include "relays.h"
#include "timer.h"
#include "main.h"

// temporary variable to store register
uint8_t volatile reg;

// Temperature conversion variables
//config.thermostat.state.tempRaw = 0;
//uint8_t tempSign = 0;
//uint8_t tempWhole = 0;
//uint8_t tempFract = 0;
uint16_t temp100 = 0;

int main(void)
{
	/* Initialization routines */
	Timer__Initialize();
	Relays__Initialize();
	SPIInitMaster();
	Usart__Initialize();
	INT0_interrupt_init();
	Radio__Initialize();
	TempSensor__Initialize();
	Micro__EnableInterrupts();

#if 0
	// Radio Diagnostics
	// LED blink if we read the correct STATUS
	if (RF24GetReg(STATUS) == 0x0E)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			PORTB |= (1 << PB0);
			_delay_ms(500);
			PORTB &= ~(1 << PB0);
			_delay_ms(500);
		}
	}
	
#endif
	// Loads the default configuration data
	configLoadDefault();
	
	// Send STATUS to console -- DEBUG
	USART__TransmitChar(RF24GetReg(FIFO_STATUS));
	//! Main loop
	while(1)
    {
#if 0
		// Listens for packets for 1 sec
		RF24ReceivePayload();
		RF24ResetIRQ();
#endif
    }
}

/**
 * Timer 0 compare match ISR
 *
 * This shall be triggered every 1 ms
 */
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
    uint8_t prescaler;

	// Increment the base counter
    prescaler = Timer__GetCounter()++;

	// Execute the 1ms tasks
    TempSensor__1msTask();
    Relays__1msTask();

	// Execute the 100ms tasks
	if (prescaler == 100)
	{
	    Timer__ResetCounter();
	    Thermostat__100msTask();
	}

}
