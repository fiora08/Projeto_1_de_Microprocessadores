
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timers.h"
#include "serial.h"
#include "lcd.h"
#include "teclado.h"
#include "energia.h"
#include "interface.h"

int main(void) {
	// Inicializa��o dos perif�ricos
	serial_inicializar(51); //-> formula para dar o 119200 com 16 mhz foi definido o 51 para a velocidade baud. 
	timer0_init();
	timer1_init();
	teclado_inicializar();
	lcd_inicializar();
	energia_inicializar();
	
	sei(); // Habilita interrup��es

	char tecla;
	char buffer_lcd[17]; // 16 caracteres + '\0'
	unsigned char index = 0;
	unsigned char sistema_ja_ligado = 0;

	while (1) {
		energia_gerenciar(); //

		if (energia_sistema_ativo() == LIGADO) {
			
			// --- INICIALIZA��O VISUAL ---
			if (!sistema_ja_ligado) {
				lcd_limpar(); //
				lcd_posicionar(0, 2); // Centraliza levemente "MicPay 2026"
				lcd_escrever_string("MicPay 2026");
				lcd_posicionar(1, 1);
				lcd_escrever_string("Sistema Ativo");
				//_delay_ms(1500);
				atraso_ms(1500);
				lcd_limpar();
				lcd_escrever_string("Bloqueado:");
				//_delay_ms(1500);
				sistema_ja_ligado = 1;


			}

			// --- ESCUTA SERIAL (LINHA 0) ---
			if (serial_disponivel()) {
				char recebido = serial_ler();
				
				// Trata Enter ou limite de 16 colunas do LCD
				if (recebido == '\n' || recebido == '\r' || index >= 16) {
					buffer_lcd[index] = '\0';
					
					// Limpa apenas a linha 0 para novos dados da serial
					lcd_posicionar(0, 0);
					lcd_escrever_string("� � � � � � � � "); // 16 espa�os para limpar
					lcd_posicionar(0, 0);
					lcd_escrever_string(buffer_lcd);
					index = 0;
					} else {
					buffer_lcd[index++] = recebido;
				}
			}


			// --- FALA TECLADO (LINHA 1) ---
			teclado_atualizar(); //
			tecla = teclado_obter_tecla();
			
			if (tecla != 0) {
				serial_transmitir(tecla);
				
				// Limpa a parte de dados da linha 1 (coluna 9 em diante)
				lcd_posicionar(1, 0);
				lcd_escrever_string("Teclado:� � � � "); // Mant�m o r�tulo e limpa o resto
				lcd_posicionar(1, 9);
				lcd_caractere(tecla); //
			}
			

			/*// --- FALA TECLADO (LINHA 1) ---
			teclado_atualizar(); //
			if(estado_atual == BLOQUEADO ){
				unsigned char senha_login[4];
				mascarar_senha(senha_login, 4);
			}
			else if (tecla != 0 && estado_atual == LIBERADO) {
				serial_transmitir(tecla);
				tecla = teclado_obter_tecla();
				// Limpa a parte de dados da linha 1 (coluna 9 em diante)
				lcd_posicionar(1, 0);
				lcd_escrever_string("Teclado: "); // Mant�m o r�tulo e limpa o resto
				lcd_posicionar(1, 9);
				lcd_caractere(tecla); 
			}
			else{estado_atual = ERRO};*/
			
			
			
			flag_1ms = 0; //
			} else {
			sistema_ja_ligado = 0;
		}
	}
	return 0;
}