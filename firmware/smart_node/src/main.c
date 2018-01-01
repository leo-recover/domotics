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

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <radio.h>
#include <temp_sensor.h>
#include <usart.h>
#include "parameters.h"
#include "spi.h"
#include "events.h"
#include "relays.h"
#include "timer.h"
#include "main.h"
#include "onewire.h"

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
	Onewire__Initialize();
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
		
		// Temperature conversion
		config.thermostat.state.tempSign = config.thermostat.state.tempRaw & (0x8000); // test sign (MSb)
		if (config.thermostat.state.tempSign)
		{
			config.thermostat.state.tempRaw = (config.thermostat.state.tempRaw ^ 0xFFFF) + 1; // 2's complement
		}
		config.thermostat.state.temp100 = (6 * config.thermostat.state.tempRaw) + config.thermostat.state.tempRaw / 4; // multiply by (100 * 0.0625) or 6.25
		if (config.thermostat.state.tempSign)
		{
			config.thermostat.state.temp100 = -1 * config.thermostat.state.temp100;
		}
		
		//config.thermostat.state.tempWhole = temp100 / 100;
		//config.thermostat.state.tempFract = temp100 % 100;
		
		// WINTER Thermostat
		if (config.thermostat.mode == WINTER)
		{
			if (config.thermostat.state.temp100 <= (config.thermostat.tempSet100))
			{
				if (config.thermostat.state.active == ON) {}
				else
				{
					//relay0Set();
					config.thermostat.state.active = ON;
				}
			}
			else if (config.thermostat.state.temp100 >= (config.thermostat.tempSet100 + config.thermostat.hist100))
			{
				if (config.thermostat.state.active == OFF) {}
				else
				{
					//relay0Reset();
					config.thermostat.state.active = OFF;
				}
			}
		}
		
		// SUMMER Thermostat
		else if (config.thermostat.mode == SUMMER)
		{
			if (config.thermostat.state.temp100 >= (config.thermostat.tempSet100))
			{
				if (config.thermostat.state.active == ON) {}
				else
				{
					//relay1Set();
					config.thermostat.state.active = ON;
				}
			}
			else if (config.thermostat.state.temp100 <= (config.thermostat.tempSet100 - config.thermostat.hist100))
			{
				if (config.thermostat.state.active == OFF) {}
				else
				{
					//relay1Reset();
					config.thermostat.state.active = OFF;
				}
			}
		}
    }
}

/**
 * Timer 0 compare match ISR
 *
 * This shall be triggered every 1 ms
 */
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	// Increment the base counter
	Timer__GetCounter()++;

	// Execute the tasks
	TempSensor__Handler();
	Relays__Handler();
}
