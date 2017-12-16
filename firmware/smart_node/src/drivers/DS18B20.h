/*
 * \file DS18B20.h
 *
 * \date 22/10/2014 20:50:57
 * \author Leonardo Ricupero
 */ 


#ifndef DS18B20_H_
#define DS18B20_H_

#include "OW.h"
#include <string.h>

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

// Method prototypes
uint8_t DSInit(void);
uint16_t DSReadTemperature(uint8_t tries);
uint8_t DSStartConversion(void);

#endif /* DS18B20_H_ */
