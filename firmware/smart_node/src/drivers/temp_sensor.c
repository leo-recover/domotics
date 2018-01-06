/**
 * @file
 *
 * @date 26/12/2017
 * @author Leonardo Ricupero
 */ 

#include "onewire.h"
#include "temp_sensor.h"

#define SCRATCHPAD_SIZE		9

// ROM Commands
#define SEARCH_ROM			0xF0
#define READ_ROM			0x33
#define MATCH_ROM			0x55
#define SKIP_ROM			0xCC
#define ALARM_SEARCH		0xEC

// Function Commands
#define CONVERT_T			0x44
#define WRITE_SCRATCHPAD	0x4E
#define READ_SCRATCHPAD		0xBE
#define COPY_SCRATCHPAD		0x48
#define RECALL_E2			0xB8
#define READ_POWER_SUPPLY	0xB4

// Alarms and Configuration
#define T_ALARM_HIGH		0x32 // +50
#define T_ALARM_LOW			0x85 // -5 1000 0101b
#define RES_CONFIG			0x7F // 12 bit 0111 1111b


typedef enum {
	STATE_INIT = 0,
	STATE_IDLE,
	STATE_DETECT_PRESENCE,
	STATE_SKIP_ROM,
	STATE_CONFIG_PRE,
	STATE_CONFIG_T_ALARM0,
	STATE_CONFIG_T_ALARM1,
	STATE_CONFIG_RESOLUTION,
	STATE_CONVERT_TEMPERATURE,
	STATE_POLL_CONVERSION,
	STATE_READ_SCRATCHPAD,
	STATE_ACQUIRING_SCRATCHPAD,
	STATE_ERROR_FOUND,
} TEMP_SENSOR_STATE_T;

typedef enum {
	EVENT_NO_EVENT,
	EVENT_CONFIG_REQUESTED,
	EVENT_READ_TEMP_REQUESTED,
	EVENT_CONVERSION_FINISHED,
	EVENT_TEMPERATURE_READ,
	EVENT_TIMEOUT_EXPIRED,
} TEMP_SENSOR_EVENT_T;


static TEMP_SENSOR_STATE_T TempSensor_State;
static TEMP_SENSOR_EVENT_T TempSensor_Event;
static uint8_t Scratchpad[SCRATCHPAD_SIZE];
static uint8_t Scratchpad_Read_Index;

/**
 * @brief Initialize the module
 * 
 */
void TempSensor__Initialize(void)
{
	uint8_t i;

	Onewire__Initialize();

	TempSensor_State = STATE_INIT;
	TempSensor_Event = EVENT_NO_EVENT;
	for (i=0; i<SCRATCHPAD_SIZE; i++)
	{
		Scratchpad[i] = 0;
	}
	Scratchpad_Read_Index = 0;
}

void TempSensor__Configure(void)
{
	TempSensor_Event = EVENT_CONFIG_REQUESTED;
}

void TempSensor__StartAcquisition(void)
{
	if (TempSensor_Event == EVENT_NO_EVENT)
	{
		TempSensor_Event = EVENT_READ_TEMP_REQUESTED;
	}
}

/**
 * @brief 	Get the last measured temperature
 *
 * @details The temperature is given in fixed point
 * 			format Q12.4
 *
 */
int16_t TempSensor__GetTemperature(void)
{
	int16_t result;

	result = Scratchpad[1] << 8;
	result += Scratchpad[0];
	return result;
}

uint8_t TempSensor__IsTemperatureReady(void)
{
	uint8_t result = 0;
	if (TempSensor_Event == EVENT_TEMPERATURE_READ)
	{
		TempSensor_Event = EVENT_NO_EVENT;
		result = 1;
	}
	return result;
}

void TempSensor__1msTask(void)
{
	TEMP_SENSOR_STATE_T next_state;
	uint8_t temp;
	// todo implementation of a timer for timeout
	switch(TempSensor_State)
	{
		case STATE_INIT:
		{
			next_state = STATE_IDLE;
			break;
		}
		case STATE_IDLE:
		{
			if (TempSensor_Event == EVENT_CONFIG_REQUESTED ||
				TempSensor_Event == EVENT_READ_TEMP_REQUESTED)
			{
				Onewire__DetectPresence();
				next_state = STATE_DETECT_PRESENCE;
			}
			break;
		}
		case STATE_DETECT_PRESENCE:
		{
			next_state = STATE_DETECT_PRESENCE;

			if (Onewire__IsIdle())
			{
				temp = Onewire__GetPresence();
				if (temp == ONEWIRE_PRESENCE_OK)
				{
					Onewire__WriteByte(SKIP_ROM);
					next_state = STATE_SKIP_ROM;
				}
				else
				{
					next_state = STATE_ERROR_FOUND;
				}
			}
			break;
		}
		case STATE_SKIP_ROM:
		{
			next_state = STATE_SKIP_ROM;

			if (Onewire__IsIdle())
			{
				if (TempSensor_Event == EVENT_CONFIG_REQUESTED)
				{
					Onewire__WriteByte(T_ALARM_HIGH);
					next_state = STATE_CONFIG_T_ALARM0;
				}
				else if (TempSensor_Event == EVENT_READ_TEMP_REQUESTED)
				{
					Onewire__WriteByte(CONVERT_T);
					next_state = STATE_CONVERT_TEMPERATURE;
				}
				else if (TempSensor_Event == EVENT_CONVERSION_FINISHED)
				{
					Onewire__WriteByte(READ_SCRATCHPAD);
					next_state = STATE_READ_SCRATCHPAD;
				}
			}
			break;
		}
		case STATE_CONVERT_TEMPERATURE:
		{
			next_state = STATE_CONVERT_TEMPERATURE;

			if (Onewire__IsIdle())
			{
				Onewire__StartReadBit();
				next_state = STATE_POLL_CONVERSION;
			}
			break;
		}
		case STATE_POLL_CONVERSION:
		{
			next_state = STATE_POLL_CONVERSION;

			if (Onewire__IsIdle())
			{
				temp = Onewire__GetLastSample();
				if (temp == 1)
				{
					TempSensor_Event = EVENT_CONVERSION_FINISHED;
					next_state = STATE_DETECT_PRESENCE;
				}
				else
				{
					// poll again
					next_state = STATE_CONVERT_TEMPERATURE;
				}
			}
			break;
		}
		case STATE_READ_SCRATCHPAD:
		{
			next_state = STATE_READ_SCRATCHPAD;

			if (Onewire__IsIdle())
			{
				Onewire__StartReadByte();
				next_state = STATE_ACQUIRING_SCRATCHPAD;
			}
			break;
		}
		case STATE_ACQUIRING_SCRATCHPAD:
		{
			next_state = STATE_ACQUIRING_SCRATCHPAD;

			if (Scratchpad_Read_Index < SCRATCHPAD_SIZE)
			{
				if (Onewire__IsIdle())
				{
					Scratchpad[Scratchpad_Read_Index] = Onewire__GetLastByte();
					Scratchpad_Read_Index++;
					Onewire__StartReadByte();
				}
			}
			else
			{
				TempSensor_Event = EVENT_TEMPERATURE_READ;
				next_state = STATE_IDLE;
			}
			break;
		}
		case STATE_CONFIG_T_ALARM0:
		{
			next_state = STATE_CONFIG_T_ALARM0;

			if (Onewire__IsIdle())
			{
				Onewire__WriteByte(T_ALARM_LOW);
				next_state = STATE_CONFIG_T_ALARM1;
			}
			break;
		}
		case STATE_CONFIG_T_ALARM1:
		{
			next_state = STATE_CONFIG_T_ALARM1;

			if (Onewire__IsIdle())
			{
				Onewire__WriteByte(RES_CONFIG);
				next_state = STATE_CONFIG_RESOLUTION;
			}
			break;
		}
		case STATE_CONFIG_RESOLUTION:
		{
			next_state = STATE_CONFIG_RESOLUTION;

			if (Onewire__IsIdle())
			{
				next_state = STATE_IDLE;
				TempSensor_Event = EVENT_NO_EVENT;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	TempSensor_State = next_state;
}

