/*
 * button.c
 *
 *  Created on: 10 de out de 2016
 *      Author: patrizio
 */

#include <FreeRTOS.h>
#include <task.h>
#include <sirene.h>

#define BUTTON_D6_PRESSED() !(PIND & _BV(PORTD6))

static unsigned char processButtonState;
static unsigned char buttonState = 0;

enum {
	BUTTON_READ,
	BUTTON_DEBOUNCE
};

static void prvButtonTask(void *arg);

void xStartButtonTask(void) {
	// Pin D6 as INPUT and Pull-up ON.
	DDRD &= ~_BV(DDD6);
	PORTD |= _BV(PORTD6);

	processButtonState = BUTTON_READ;

	xTaskCreate(prvButtonTask, (signed portCHAR *) "BUT1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

static void prvButtonTask(void *arg) {
	for (;;) {
		switch (processButtonState) {
			case BUTTON_READ:
				if (BUTTON_D6_PRESSED()) {	// Input LOW --> Button pressed
					processButtonState = BUTTON_DEBOUNCE;
				} else {
					vTaskDelay(1); // 1ms de atraso, faz o polling do botão com frequência de 1KHz
				}
				break;

			case BUTTON_DEBOUNCE:
				vTaskDelay(18);
				if (BUTTON_D6_PRESSED() && buttonState == 0) {
					toggleSirene();
					buttonState = 1;
				} else if (!BUTTON_D6_PRESSED()) {
					buttonState = 0;
				}
				processButtonState = BUTTON_READ;
				break;
		}
	}
}
