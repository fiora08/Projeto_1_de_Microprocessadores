#ifndef F_CPU
#define F_CPU 16000000UL // Garante a base de tempo para o delay.h
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
// aqui usamos a biblioteca delay para garantir a leitura correta dos caracteres.
// em vez de um timer especifico.
// todos os delays usados aqui � para o sistema possa identificar as mudan�as fisicamente. 
// como mudan�a na tens�o.
// LCD_DADOS_PORT � o portc
// LCD_CONTR_PORT � o portb
// aqui � o controle do enable que valida o envio de informa��o pro lcd
static void lcd_pulso_enable(void) {
	LCD_CONTR_PORT |= (1 << EN); // ativa o enable
	_delay_us(10); 
	LCD_CONTR_PORT &= ~(1 << EN); // desativa o enable
	_delay_us(100); 
}

// Envia 4 bits para os pinos PC4-PC7 com atraso entre nibbles
static void lcd_enviar_bits(unsigned char dado) {
	// Atualiza apenas os bits altos (PC4-PC7)
	LCD_DADOS_PORT = (LCD_DADOS_PORT & 0x0F) | (dado & 0xF0);
	lcd_pulso_enable(); // dispara a leitura
	_delay_us(100); // Pequena pausa extra para o simulador processar o dado
}

void lcd_comando(unsigned char comando) {
	LCD_CONTR_PORT &= ~(1 << RS);    // RS em 0 (Instru��o)
	lcd_enviar_bits(comando);       // Parte alta
	lcd_enviar_bits(comando << 4);  // Parte baixa
	_delay_ms(2);                    // Delay para comandos lentos como 'limpar'
}

void lcd_caractere(unsigned char dado) {
	LCD_CONTR_PORT |= (1 << RS);     // RS em 1
	lcd_enviar_bits(dado);          // Parte alta
	lcd_enviar_bits(dado << 4);     // Parte baixa
	_delay_us(100);
}

void lcd_inicializar(void) {
	// Configura pinos como sa�da
	LCD_DADOS_DDR |= 0xF0;
	LCD_CONTR_DDR |= (1 << RS) | (1 << EN) | (1 << BACKLIGHT);
	//LCD_CONTR_DDR |= (1 << RS) | (1 << EN);
	
	_delay_ms(100); // Aguarda o LCD estabilizar a tens�o

	// Sequ�ncia de inicializa��o for�ada para modo 4 bits
	lcd_enviar_bits(0x30);
	_delay_ms(10);
	lcd_enviar_bits(0x30);
	_delay_ms(1);
	lcd_enviar_bits(0x30);
	_delay_ms(1);
	lcd_enviar_bits(0x20); // Define interface de 4 bits
	_delay_ms(1);

	lcd_comando(0x28); // 2 linhas, matriz 5x8
	lcd_comando(0x0C); // Display ligado, cursor desligado
	lcd_comando(0x06); // Incremento autom�tico
	lcd_limpar();
}

// escreve uma frase no display
// 
void lcd_escrever_string(char* texto) {
	while (*texto) { // enquanto n�o chega o final '\0' da string 
		lcd_caractere(*texto++); // envia o caracter atual e aponta o proximo.
	}
}

// move o cursor para a posi��o desejada
void lcd_posicionar(unsigned char linha, unsigned char coluna) {
	unsigned char posicao = (linha == 0) ? (0x80 + coluna) : (0xC0 + coluna); // // 0x80 � o endere�o da linha 1, 0xC0 o da linha 2
	lcd_comando(posicao); // move o cursor para a posi��o calculada
}

void lcd_limpar(void) {
	lcd_comando(0x01);  // // Comando 0x01: Clear Display
	_delay_ms(5); // Delay maior para a limpeza da mem�ria interna
}

