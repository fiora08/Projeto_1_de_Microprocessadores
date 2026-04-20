#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "teclado.h"
#include "lcd.h"
#include "energia.h"

int main(void) {
	// 1. Inicializações de Hardware
	teclado_inicializar(); // Configura PORTA
	timer0_init();         // Configura base de 1ms
	energia_inicializar(); // Define estado inicial como DESLIGADO
	
	// Habilita interrupções globais para o Timer 0
	sei();

	while (1) {
		// A função energia_gerenciar cuida do cronômetro de 3s e 4s
		energia_gerenciar();

		// Só processamos o eco do teclado se o sistema estiver ativo
		if (energia_sistema_ativo() == LIGADO) {
			
			// O teclado_atualizar processa o debounce de 20ms
			teclado_atualizar();
			
			unsigned char tecla_pressionada = teclado_obter_tecla();
			
			if (tecla_pressionada != 0) {
				// Escreve qualquer tecla pressionada no LCD
				lcd_caractere(tecla_pressionada);
			}
		}

		// LIMPEZA DA FLAG: Essencial para que energia e teclado vejam o mesmo "tique"
		if (flag_1ms) {
			flag_1ms = 0;
		}
	}
}