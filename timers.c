#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
// volatile avisa ao compilador que esta variavel pode mudar a qualquer momento
// ideal para interrupções
volatile unsigned char flag_1ms = 0;
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
// interrupção do timer 0
ISR(TIMER0_COMPA_vect){
	flag_1ms = 1;
}