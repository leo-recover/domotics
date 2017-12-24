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

static volatile ONEWIRE_STATE_T Onewire_State;
static uint16_t Remaining_Delay;


void Onewire__Initialize(void)
{
	// Disable the timer for programming
	GTCCR |= (1 << TSM | 1 << PSRASY);
	// Select CTC mode
	TCCR2A |= (0 << WGM22) | (1 << WGM21) | (0 << WGM20);
	// Set compare register to the maximum value
	OCR2A = 255;
	// Set the timer prescaler to 8 and enable the counter
	TCCR2B |= (0 << CS02) | (1 << CS01) | (0 << CS00);

	Onewire_State = ONEWIRE_IDLE;
	Last_Sample = ONEWIRE_DATA_NOT_READY;
}


void Onewire__StartDetectPresence(void)
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


void Onewire__StartWriteBit(uint8_t bit)
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


uint8_t Onewire__IsIdle(void)
{
	uint8_t result = 0;
	if (Onewire_State == ONEWIRE_IDLE)
	{
		result = 1;
	}
	return result;
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
	// Enable the counter
	GTCCR &= ~(1 << TSM);
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
			// Sample for presence pulse from slave
			Last_Sample = PIND & (1 << PIND7);
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
			// Sample for presence pulse from slave
			Last_Sample = PIND & (1 << PIND7);
			Onewire_State = ONEWIRE_READBIT_RECOVERY;
			StartTimer(DELAY_55_US);
			break;
		}
		case ONEWIRE_PRESENCE_RECOVERY:
		case ONEWIRE_WRITE1_RECOVERY:
		case ONEWIRE_WRITE0_RECOVERY:
		case ONEWIRE_READBIT_RECOVERY:
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
		default:
		{
			break;
		}
	}
}
