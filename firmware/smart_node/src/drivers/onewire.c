/**
 * @file
 *
 * @brief 1-Wire driver for single device
 *
 * @details 	This driver provides support for 1-wire communication with
 * 				a single device in a non-blocking way.
 * 				The driver uses the TC2 in CTC mode and a state machine in order
 * 				to implement the required delays without blocking the SW execution.
 *
 * @date 24/12/2017
 * @author Leonardo Ricupero
 */ 

#include "onewire.h"

// Ticks for a delay of 1 microsecond timer clocked at 2 MHz
#define TICKS_PER_MICROSECOND 2 // 2 * 0.5 us = 1 us

#define DELAY_6_US (6 * TICKS_PER_MICROSECOND)
#define DELAY_64_US (64 * TICKS_PER_MICROSECOND)
#define DELAY_60_US (60 * TICKS_PER_MICROSECOND)
#define DELAY_10_US (10 * TICKS_PER_MICROSECOND)
#define DELAY_9_US (9 * TICKS_PER_MICROSECOND)
#define DELAY_55_US (55 * TICKS_PER_MICROSECOND)
#define DELAY_480_US (480 * TICKS_PER_MICROSECOND)
#define DELAY_70_US (70 * TICKS_PER_MICROSECOND)
#define DELAY_410_US (410 * TICKS_PER_MICROSECOND)

#define TIMER2__START() {TCCR2B |= (0 << CS02) | (1 << CS01) | (0 << CS00);}
#define TIMER2__STOP() {TCCR2B &= 0b11111000;}

typedef enum {
	ONEWIRE_IDLE = 0,
	ONEWIRE_PRESENCE_SAMPLE,
	ONEWIRE_PRESENCE_DRIVE_LOW,
	ONEWIRE_PRESENCE_RECOVERY,
	ONEWIRE_WRITE1_DRIVE_LOW,
	ONEWIRE_WRITE1_RECOVERY,
	ONEWIRE_WRITE0_DRIVE_LOW,
	ONEWIRE_WRITE0_RECOVERY,
	ONEWIRE_READBIT_DRIVE_LOW,
	ONEWIRE_READBIT_SAMPLE,
	ONEWIRE_READBIT_RECOVERY,
} ONEWIRE_STATE_T;

static void StartTimer(uint16_t delay);

ONEWIRE_SAMPLE_T Last_Sample;
uint8_t Byte_Read;

static volatile ONEWIRE_STATE_T Onewire_State;
static uint16_t Remaining_Delay;
static uint8_t Byte_To_Write;
static uint8_t Last_Byte;
static uint8_t Remaining_Bits;

void Onewire__Initialize(void)
{
	// Select CTC mode
	TCCR2A |= (1 << WGM21) | (0 << WGM20);
	// Set compare register to the maximum value
	OCR2A = 255;
	// Enable the ISR
	TIMSK2 |= (1 << OCIE2A);

	Onewire_State = ONEWIRE_IDLE;
	Last_Sample = ONEWIRE_DATA_NOT_READY;
	Last_Byte = ONEWIRE_DATA_NOT_READY;
	Remaining_Bits = 0;
	Remaining_Delay = 0;
	Byte_To_Write = 0xFF;
	Byte_Read = 0xFF;
}


void Onewire__DetectPresence(void)
{
	Onewire_State = ONEWIRE_PRESENCE_DRIVE_LOW;
	// Drives DQ low
	DDRD |= (1 << DDD7);
	PORTD &= ~(1 << PORTD7);
	StartTimer(DELAY_480_US);
}


ONEWIRE_SAMPLE_T Onewire__GetPresence(void)
{
	uint8_t result = ONEWIRE_PRESENCE_NOK;
	if (Last_Sample == 0)
	{
		result = ONEWIRE_PRESENCE_OK;
	}

	Last_Sample = ONEWIRE_DATA_NOT_READY;

	return result;
}


void Onewire__WriteBit(uint8_t bit)
{
	cli();
	if (bit)
	{
		Onewire_State = ONEWIRE_WRITE1_DRIVE_LOW;
		ONEWIRE_DRIVE_BUS_LOW();
		StartTimer(DELAY_6_US);
	}
	else
	{
		Onewire_State = ONEWIRE_WRITE0_DRIVE_LOW;
		ONEWIRE_DRIVE_BUS_LOW();
		StartTimer(DELAY_60_US);
	}
	sei();
}


void Onewire__StartReadBit(void)
{
	cli();
	Onewire_State = ONEWIRE_READBIT_DRIVE_LOW;
	ONEWIRE_DRIVE_BUS_LOW();
	StartTimer(DELAY_6_US);
	sei();
}

void Onewire__WriteByte(uint8_t data)
{
	Byte_To_Write = data;
	Remaining_Bits = 7;
	Onewire__WriteBit(Byte_To_Write & 0x1);
	Byte_To_Write = Byte_To_Write >> 1;
}

void Onewire__StartReadByte(void)
{
	Remaining_Bits = 8;
	Onewire__StartReadBit();
}


uint8_t Onewire__IsIdle(void)
{
	uint8_t result = 0;
	if (Onewire_State == ONEWIRE_IDLE)
	{
		result = 1;
	}
	return result;
}


/**
 * Timer 2 compare match ISR
 *
 */
ISR(TIMER2_COMPA_vect)
{
	// Stop the counter
	GTCCR |= (1 << TSM | 1 << PSRASY);

	switch(Onewire_State)
	{
		case ONEWIRE_IDLE:
		{
		    TIMER2__STOP();
			break;
		}
		case ONEWIRE_PRESENCE_DRIVE_LOW:
		{
			if (Remaining_Delay != 0)
			{
				StartTimer(Remaining_Delay);
			}
			else
			{
				ONEWIRE_RELEASE_BUS();
				Onewire_State = ONEWIRE_PRESENCE_SAMPLE;
				StartTimer(DELAY_70_US);
			}
			break;
		}
		case ONEWIRE_PRESENCE_SAMPLE:
		{
			Last_Sample = ONEWIRE_SAMPLE_BUS();
			Onewire_State = ONEWIRE_PRESENCE_RECOVERY;
			StartTimer(DELAY_410_US);
			break;
		}
		case ONEWIRE_WRITE1_DRIVE_LOW:
		{
			ONEWIRE_RELEASE_BUS();
			Onewire_State = ONEWIRE_WRITE1_RECOVERY;
			StartTimer(DELAY_64_US); // Complete the time slot and 10us recovery
			break;
		}
		case ONEWIRE_WRITE0_DRIVE_LOW:
		{
			ONEWIRE_RELEASE_BUS();
			Onewire_State = ONEWIRE_WRITE0_RECOVERY;
			StartTimer(DELAY_10_US);
			break;
		}
		case ONEWIRE_READBIT_DRIVE_LOW:
		{
			ONEWIRE_RELEASE_BUS();
			Onewire_State = ONEWIRE_READBIT_SAMPLE;
			StartTimer(DELAY_9_US);
			break;
		}
		case ONEWIRE_READBIT_SAMPLE:
		{
			Last_Sample = ONEWIRE_SAMPLE_BUS();
			if (Remaining_Bits != 0)
			{
				Byte_Read |= (Last_Sample << (8 - Remaining_Bits));
			}
			Onewire_State = ONEWIRE_READBIT_RECOVERY;
			StartTimer(DELAY_55_US);
			break;
		}
		case ONEWIRE_PRESENCE_RECOVERY:
		{
			if (Remaining_Delay != 0)
			{
				StartTimer(Remaining_Delay);
			}
			else
			{
				Onewire_State = ONEWIRE_IDLE;
			}
			break;
		}
		case ONEWIRE_WRITE1_RECOVERY:
		case ONEWIRE_WRITE0_RECOVERY:
		{
			if (Remaining_Bits != 0)
			{
				Remaining_Bits--;
				Onewire__WriteBit(Byte_To_Write & 0x1);
				Byte_To_Write = Byte_To_Write >> 1;
			}
			else
			{
				Onewire_State = ONEWIRE_IDLE;
			}
			break;
		}
		case ONEWIRE_READBIT_RECOVERY:
		{
			if (Remaining_Bits != 0)
			{
				Remaining_Bits--;
				Onewire__StartReadBit();
			}
			else
			{
				Onewire_State = ONEWIRE_IDLE;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

static void StartTimer(uint16_t delay)
{
	if (delay > 255)
	{
		Remaining_Delay = delay - 255;
		OCR2A = 255;
	}
	else
	{
		Remaining_Delay = 0;
		OCR2A = delay;
	}

	TIMER2__START();
}

