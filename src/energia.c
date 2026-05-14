#include <avr/io.h>
#include <avr/iom2560.h>
#include "energia.h"
#include "interface.h"
#include "maquina.h"
#include "teclado.h"
#include "timers.h"
#include "lcd.h"
// contador de tempo de quanto o bot�o ta sendo precionado
static unsigned int contador_tempo = 0;
// na biblioteca foi definido que desligado � 0 e ligado � 1, facilitar a logica em vez de ficar usando numeros.
static unsigned char estado_sistema = DESLIGADO;
unsigned char sistema_ja_ligado = 0;
// define o estado inicial do arduino mega
void energia_inicializar(void) {
	estado_sistema = DESLIGADO;
	contador_tempo = 0;
	DDRB |= (1<<LED_fora_do_ar) | (1<<LED_pagamento_pendente); //ativa os leds para sem resp e pagamento
}
// fun��o que retorna se o sistema esta ativo ou n�o 1 � ligado 0 � desligado
unsigned char energia_sistema_ativo(void) {
	return estado_sistema;
}

// verifica o pressionamento longo dos botoes d e b.
void energia_gerenciar(void) {
// le o teclado sem o debounce pra verificar o pressionamento continuo
// fun��o teclado_ler_bruto � a fun��o pertencente ao teclado.h
// a logica abaixo usa milli segundos em vez de segundos pois usamos o timer 0. 
// timer 0 usamos para o debounce da tecla e para os alarmes.
	unsigned char tecla = teclado_ler_bruto();
	// a logica abaixo faz a verifica��o a cada 1 milli segundo com a flag_1ms
	if (flag_1ms) {
		// aqui � onde fica a logica caso o sistema esteja desligado
		if (estado_sistema == DESLIGADO) {
			if (tecla == 'D') {  
				contador_tempo++;  // incrementa 1 a cada 1 milli segundo
				if (contador_tempo >= 3000) {  // quando bater 3 segundos o sistema aplica estas fun��es.
					estado_sistema = LIGADO;  // liga o sistema
					contador_tempo = 0;
					PORTB |= (1 << PB2); // liga o backlight
					lcd_inicializar();  // fun��oes da biblioteca lcd.h
					lcd_limpar();  
					sistema_ja_ligado =0;
					estado_atual = BLOQUEADO;
				}
		} else {
				contador_tempo = 0; 
			}
			}
			// aqui � o cenario onde ta ligado o sistema 
		else if (estado_sistema == LIGADO) {
			if (tecla == 'B') {  
				contador_tempo++;  // mesma logica acima , conta 1 a cada 11 milli segundo.
				if (contador_tempo >= 4000) { 
					estado_sistema = DESLIGADO; // desliga o sistema
					contador_tempo = 0; 
					PORTB &= ~(1 << PB2); // desliga o backlight
					lcd_limpar(); 
					sistema_ja_ligado = 0;        // reseta login
					estado_atual = BLOQUEADO;     // reseta estado
					contadores_tempo[0] = 0;      // reseta timers
					contadores_tempo[1] = 0;
					// Aqui o sistema "morre" visualmente
					// colocar aqui o comando UART para desligar o sistema
				}
				} else {
				contador_tempo = 0; // se soltar o b antes de 4 segundos da um reset
			}
		}
	}
}