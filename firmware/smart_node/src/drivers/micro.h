/**
 * @file micro.h
 *
 * @date 17 dic 2017
 * @author Leonardo Ricupero
 */

#ifndef SRC_DRIVERS_MICRO_H_
#define SRC_DRIVERS_MICRO_H_

#include <avr/interrupt.h>

// Frequency of the CPU
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz - External crystal 16MHz
#endif

void Micro__Initialize(void);

#define Micro__GetClockFrequency() F_CPU
#define Micro__EnableInterrupts() sei()

#endif /* SRC_DRIVERS_MICRO_H_ */
