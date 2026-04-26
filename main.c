#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "teclado.h"
#include "lcd.h"
#include "energia.h"
#include "interface.h"

int main(void) {
	// 1. Inicializa��es de Hardware
	teclado_inicializar(); // Configura PORTA
	timer0_init();         // Configura base de 1ms
	energia_inicializar(); // Define estado inicial como DESLIGADO
	
	// Habilita interrup��es globais para o Timer 0
	sei();

	while (1) {
		// A fun��o energia_gerenciar cuida do cron�metro de 3s e 4s
		energia_gerenciar();

		// S� processamos o eco do teclado se o sistema estiver ativo
		if (energia_sistema_ativo() == LIGADO) {
			
			// O teclado_atualizar processa o debounce de 20ms
			teclado_atualizar();
			mascarar_senha();
			
			/*if(estado_atual == BLOQUEADO){

				mascarar_senha();

			}else if(estado_atual == LIBERADO){

				unsigned char tecla_pressionada = teclado_obter_tecla();
				lcd_caractere(tecla_pressionada);

			}
			lcd_limpar();
			else(lcd_string("DEU ERRO"));*/
			
			/*unsigned char tecla_pressionada = teclado_obter_tecla();
			
			if (tecla_pressionada != 0) {
				// Escreve qualquer tecla pressionada no LCD
				
			}*/
		}

		// LIMPEZA DA FLAG: Essencial para que energia e teclado vejam o mesmo "tique"
		if (flag_1ms) {
			flag_1ms = 0;
		}
	}
}