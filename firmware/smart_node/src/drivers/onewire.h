/*
 * \file OW.h
 *
 * Created: 21/10/2014 13:19:47
 * \author Leonardo Ricupero
 */ 


#ifndef OW_H_
#define OW_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void OWtickDelay (uint16_t tick);
void OWSetSpeed(uint8_t standard);
uint8_t Onewire__DetectPresence(void);
void Onewire__WriteBit(uint8_t bit);
uint8_t Onewire__ReadBit(void);
void OWWriteByte(uint8_t data);
uint8_t OWReadByte(void);
/*
uint8_t OWTouchByte(uint8_t data);
void OWBlock(unsigned char *data, uint8_t data_len);
uint8_t OWOverdriveSkip(unsigned char *data, uint8_t data_len);
*/
#endif /* OW_H_ */
