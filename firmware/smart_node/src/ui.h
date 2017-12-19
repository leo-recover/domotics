/**
 * @file ui.h
 *
 * @date 17 dic 2017
 * @author Leonardo Ricupero
 */

#ifndef SRC_UI_H_
#define SRC_UI_H_

#include <avr/io.h>

#define LED_PORT PORTB
#define LED_PIN PB0

#define Ui__LedOn() {LED_PORT |= (1 << LED_PIN);}
#define Ui__LedOff() {LED_PORT &= ~(1 << LED_PIN);}

void Ui__Initialize(void);

#endif /* SRC_UI_H_ */
