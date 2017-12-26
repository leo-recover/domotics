/*
 * \file temp_sensor.h
 *
 * \date 22/10/2014 20:50:57
 * \author Leonardo Ricupero
 */ 


#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

#include <avr/io.h>

void TempSensor__Initialize(void);
void TempSensor__Configure(void);
void TempSensor__StartAcquisition(void);
uint8_t TempSensor__IsTemperatureReady(void);
int16_t TempSensor__GetTemperature(void);
void TempSensor__Handler(void);


#endif /* TEMP_SENSOR_H_ */
