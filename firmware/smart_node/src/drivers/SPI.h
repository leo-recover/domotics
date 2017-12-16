/*
 * SPI.h
 *
 * Created: 22/09/2014 17:18:32
 *  Author: Leo
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

void SPIInitMaster(void);
char SPIWriteByte(char cData);


#endif /* SPI_H_ */
