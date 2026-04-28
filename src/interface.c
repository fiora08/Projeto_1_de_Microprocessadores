 
	#ifndef F_CPU
	#define F_CPU 16000000UL // Garante a base de tempo para o delay.h
	#endif

	#include "interface.h"
	#include "teclado.h"
	#include "lcd.h"



	unsigned char mascarar_senha(unsigned char *senha_out, unsigned char tamanho_senha) {
		
		unsigned char tecla = teclado_obter_tecla();
		
		static unsigned char senha[6];
		static unsigned char pos = 0;
		
		if (tecla != 0) {
			if (pos < tamanho_senha) {
				senha[pos++] = tecla;
				lcd_caractere('*');
			}
		}
		
		if (pos == tamanho_senha) {
			for (unsigned char i = 0; i < tamanho_senha; i++) {
				senha_out[i] = senha[i];
			}
			pos = 0;
			senha_out[tamanho_senha]='\0';
			return 1;
		}
		
		return 0;
	}