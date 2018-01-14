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
#include "timer.h"
#include "usart.h"
#include "spi.h"
#include "radio.h"
#include "temp_sensor.h"
#include "thermostat.h"
#include "parameters.h"
#include "events.h"
#include "relays.h"
#include "ui.h"
#include "main.h"

uint8_t Debug_Cntr1 = 0;
int16_t Temperature;

int main(void)
{
	/* Initialization routines */
	//Timer__Initialize();
	//Relays__Initialize();
	Ui__Initialize();
	//Usart__Initialize();
	//Radio__Initialize();
	TempSensor__Initialize();
	Micro__EnableInterrupts();

	TempSensor__Configure();
	Ui__LedBlink500ms(5);

	Temperature = 0;
		
	//! Main loop
	while(1)
    {
        TempSensor__1msTask();
        
        TempSensor__StartAcquisition();
        if (TempSensor__IsTemperatureReady())
        {
            Temperature = TempSensor__GetTemperature();
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
    uint8_t prescaler;
    
    Debug_Cntr1 = TCNT0;
    
	// Increment the base counter
    prescaler = Timer__GetCounter()++;

	// Execute the 1ms tasks
    
    //Relays__1msTask();
    

	// Execute the 100ms tasks
	if (prescaler == 100)
	{
	    Timer__ResetCounter();
        // Test code
	    
        //Thermostat__100msTask();
	    Ui__100msTask();
	}

}
