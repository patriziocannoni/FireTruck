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
#define BUTTON_D6_PRESSED() !(PIND & _BV(PORTD6))

static unsigned char processSireneState;
static unsigned char processButtonState;
static unsigned char buttonState = 0;
static uint8_t sireneState = 0;

enum {
	SIRENE_HIGH,
	SIRENE_LOW,
	SIRENE_OFF
};

enum {
	BUTTON_READ,
	BUTTON_DEBOUNCE
};

static void prvSireneTask(void *arg);
static void prvButtonTask(void *arg);
static void toggleSirene(void);

void xStartSireneTask(void) {
	// Pin D6 as INPUT and Pull-up ON.
	DDRD &= ~_BV(DDD6);
	PORTD |= _BV(PORTD6);

	// Pin D5 as OUTPUT.
	DDRD |= _BV(DDD5);

	// Configure PWM.
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << COM0B1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01);

	OCR0B = TONE_OFF;

	processSireneState = SIRENE_OFF;
	processButtonState = BUTTON_READ;

	xTaskCreate(prvSireneTask, (signed portCHAR *) "SIR1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(prvButtonTask, (signed portCHAR *) "BUT1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
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

static void toggleSirene(void) {
	sireneState ^= 1;
	if (!sireneState) OCR0B = TONE_OFF;
}
