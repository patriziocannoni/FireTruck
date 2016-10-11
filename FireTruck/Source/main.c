/*
 * main.c
 *
 *  Created on: 29/08/2015
 *      Author: Patrizio
 */

#include <util/delay.h>
#include <task.h>
#include <sirene.h>
#include <button.h>

int main(void) {
	xStartButtonTask();
	xStartSireneTask();

	_delay_ms(100);

	// Inicia o scheduler.
	vTaskStartScheduler();

	return 0;
}
