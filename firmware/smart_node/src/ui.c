/**
 * @file ui.c
 *
 * @date 17 dic 2017
 * @author Leonardo Ricupero
 */

#include "micro.h"
#include "ui.h"

void Ui__Initialize(void)
{
	DDRB |= (1 << DDB0);
}

