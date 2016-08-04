/*
 * main.c
 *
 *  Created on: 29/08/2015
 *      Author: Patrizio
 */

#include <util/delay.h>
#include <task.h>
#include <sirene.h>

int main(void) {
	xStartSireneTask();

	_delay_ms(100);

	// Inicia o scheduler.
	vTaskStartScheduler();

	return 0;
}
