/**
 * @file onewire.h
 *
 * @date 21/10/2014 13:19:47
 * @author Leonardo Ricupero
 */ 


#ifndef OW_H_
#define OW_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define ONEWIRE_DRIVE_BUS_LOW() {DDRD |= (1 << DDD7); PORTD &= ~(1 << PORTD7);}
#define ONEWIRE_RELEASE_BUS() {DDRD &= ~(1 << DDD7);}

typedef enum {
	ONEWIRE_BIT_0 = 0,
	ONEWIRE_BIT_1 = 1,
	ONEWIRE_PRESENCE_OK = 2,
	ONEWIRE_PRESENCE_NOK = 3,
	ONEWIRE_DATA_NOT_READY = 0xFF,
} ONEWIRE_SAMPLE_T;

extern ONEWIRE_SAMPLE_T Last_Sample;

void Onewire__Initialize(void);
void Onewire__StartDetectPresence(void);
ONEWIRE_SAMPLE_T Onewire__GetPresence(void);
void Onewire__StartWriteBit(uint8_t bit);
void Onewire__StartReadBit(void);
uint8_t Onewire__IsIdle(void);

#define Onewire__GetLastSample() Last_Sample

#endif /* OW_H_ */
