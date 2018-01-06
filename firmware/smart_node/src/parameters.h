/*
 * \file configuration.h
 *
 * \date: 08/11/2014 17:23:43
 * \author: Leonardo Ricupero
 */ 


#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <avr/io.h>

#define SUMMER	0
#define WINTER	1
#define PLUS	0
#define MINUS	1
#define ON		1
#define OFF		0

#define THERMOSTAT_TEMPERATURE_SET 400 // 25.0 C deg
#define THERMOSTAT_TEMPERATURE_HISTERESYS 24 // 1.5 C deg

typedef struct
{
	uint8_t		active		: 1;
	uint8_t 	tempSign	: 1;
	int16_t		temp100;
	uint16_t	tempRaw;
} state_s;

typedef struct
{
	uint8_t		mode		: 1;
	uint8_t					: 7;
	int16_t		tempSet100;
	uint16_t	hist100;
	state_s state;
} config_thermostat_s;

typedef union
{
	config_thermostat_s thermostat;
	uint8_t data[sizeof(config_thermostat_s)];  	
} PARAM_T;

extern PARAM_T config;

void configLoadDefault(void);

#endif /* CONFIGURATION_H_ */
