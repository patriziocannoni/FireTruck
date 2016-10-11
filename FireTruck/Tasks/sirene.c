/*
 * sirene.c
 *
 *  Created on: 21/07/2016
 *      Author: patrizio
 */

#include <FreeRTOS.h>
#include <task.h>

#define FREQUENCY 650
#define TONE_LOW 160
#define TONE_HIGH 215
#define TONE_OFF 0

static unsigned char processSireneState;
static uint8_t sireneState = 0;

enum {
	SIRENE_HIGH,
	SIRENE_LOW,
	SIRENE_OFF
};

static void prvSireneTask(void *arg);

void xStartSireneTask(void) {
	// Pin D5 as OUTPUT.
	DDRD |= _BV(DDD5);

	// Configure PWM.
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << COM0B1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01);

	OCR0B = TONE_OFF;

	processSireneState = SIRENE_OFF;

	xTaskCreate(prvSireneTask, (signed portCHAR *) "SIR1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

void toggleSirene(void) {
	sireneState ^= 1;
	if (!sireneState) OCR0B = TONE_OFF;
}

static void prvSireneTask(void *arg) {
	for (;;) {
		switch (processSireneState) {
			case SIRENE_HIGH:
				OCR0B = TONE_HIGH;
				vTaskDelay(FREQUENCY);
				if (sireneState) {
					processSireneState = SIRENE_LOW;
				} else {
					processSireneState = SIRENE_OFF;
				}
				break;

			case SIRENE_LOW:
				OCR0B = TONE_LOW;
				vTaskDelay(FREQUENCY);
				if (sireneState) {
					processSireneState = SIRENE_HIGH;
				} else {
					processSireneState = SIRENE_OFF;
				}
				break;

			case SIRENE_OFF:
				OCR0B = TONE_OFF;
				vTaskDelay(5);
				if (sireneState) {
					processSireneState = SIRENE_HIGH;
				}
				break;
		}
	}
}
