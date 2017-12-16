/**
 * @file configuration.c
 *
 * @date 08/11/2014 17:23:30
 * @author Leo
 */ 

#include "configuration.h"

config_t config;

void configLoadDefault(void)
{
	config.thermostat.mode			= WINTER;
	config.thermostat.tempSet100	= 2050; // 20.5 degrees
	config.thermostat.hist100		= 150; // 1.5 degrees
	config.thermostat.state.active	= OFF;
}