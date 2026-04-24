#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
// volatile avisa ao compilador que esta variavel pode mudar a qualquer momento
// ideal para interrupções
volatile unsigned char flag_1ms = 0;
volatile unsigned char flag_2s = 0;
volatile unsigned int contador_serial = 0;
volatile unsigned long contador_timeout = 0;
volatile int h = 0, min = 0, seg = 0;
// inicializaçaõ retirada de um laboratorio. 
// pre escala 64 
void timer0_init(void){
	TCCR0A = (1 << WGM01); // modo CTC
	TCCR0B = (1 << CS01) | (1 << CS00); // Define a preescala em 64 
	// calculo feito = (16MHz / (64 * 1000Hz)) - 1 = 249
	OCR0A = 249;
	//Habilita a interrupção por comparação no canal a  Timer 0
	TIMSK0 |= (1 << OCIE0A);
}

void timer1_init(void){
	TCCR1B |= (1 << WGM12); // Modo CTC
	OCR1A = 249; // Interrupção a cada 1ms
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
}

// interrupção do timer 0
ISR(TIMER0_COMPA_vect){
	flag_1ms = 1;
}

ISR(TIMER1_COMPA_vect) {
	// Controle do Timeout de tentativa 
	contador_serial++;
	if (contador_serial >= 2000) {
		flag_2s = 1;
	}
	contador_timeout++;
	// logica para relogio , conferir se da certo.
	static int milis = 0;
	if (++milis >= 1000) {
		milis = 0;
		if (++seg >= 60) {
			seg = 0;
			if (++min >= 60) {
				min = 0;
				if (++h >= 24)
				 h = 0;
			}
		}
	}
}