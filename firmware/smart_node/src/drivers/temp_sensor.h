/**
 * @file temp_sensor.h
 *
 * @date 02/01/2018
 * @author Leonardo Ricupero
 */ 


#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

#include "micro.h"

void TempSensor__Initialize(void);
void TempSensor__Configure(void);
void TempSensor__StartAcquisition(void);
uint8_t TempSensor__IsTemperatureReady(void);
int16_t TempSensor__GetTemperature(void);
void TempSensor__1msTask(void);


#endif /* TEMP_SENSOR_H_ */
