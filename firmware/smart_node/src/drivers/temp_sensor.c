/*
 * \file DS18B20.c
 *
 * \date 22/10/2014 20:50:42
 * \author Leonardo Ricupero
 */ 

#include <temp_sensor.h>

/**
 * \brief Initialize the DS18B20 sensor
 * 
 * \param 
 * 
 * \return uint8_t
 */
uint8_t TempSensor__Initialize(void)
{
	uint8_t res;
	OWSetSpeed(1);
	// Send reset to initiate transaction
	res = Onewire__DetectPresence();
	// SKIP_ROM Command to address the device
	OWWriteByte(SKIP_ROM);
	// WRITE_SCRATCHPAD to configure the device
	OWWriteByte(WRITE_SCRATCHPAD);
	OWWriteByte(T_ALARM_HIGH);
	OWWriteByte(T_ALARM_LOW);
	OWWriteByte(RES_CONFIG);
	return res;
}

/**
 * \brief Start a temperature conversion
 * 
 * \param 
 * 
 * \return void
 */
uint8_t DSStartConversion(void)
{
	uint8_t res;
	// Send reset to initiate transaction
	res = Onewire__DetectPresence();
	// SKIP_ROM Command to address the device
	OWWriteByte(SKIP_ROM);
	// CONVERT_T Command to start the acquisition
	OWWriteByte(CONVERT_T);
	return res;
}

/**
 * \brief Read the temperature value
 * 
 * \param tries
 * 
 * \return uint16_t
 */
uint16_t DSReadTemperature(uint8_t tries)
{
	//uint8_t res;
	uint16_t rawTemperature;
	// Buffer to store the scratchpad
	uint8_t scratchpad[9];
	// Error: tries must be > 0!
	if (tries == 0) return 1;
	// Read time slot
	while(!Onewire__ReadBit() && tries != 0)
		tries--;
	// The sensor is still converting. Exit with no result.
	if (tries == 0) return 1;
	
	// Catch the value
	Onewire__DetectPresence();
	OWWriteByte(SKIP_ROM);
	OWWriteByte(READ_SCRATCHPAD);
	// Read and store the scartchpad
	for (uint8_t i = 0; i < 9; i++)
		scratchpad[i] = OWReadByte();
	Onewire__DetectPresence();
	// Extract temperature bytes and return
	memcpy(&rawTemperature, scratchpad, 2);
	return rawTemperature;
}
