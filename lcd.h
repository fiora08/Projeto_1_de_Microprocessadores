#ifndef DISPLAY_H
#define DISPLAY_H

// Definições de hardware para o arduino mega
#define LCD_DADOS_PORT PORTC
#define LCD_DADOS_DDR  DDRC

#define LCD_CONTR_PORT PORTB
#define LCD_CONTR_DDR  DDRB

#define BACKLIGHT PB2 // pino 51 do Mega
#define RS PB0 // Pino 53 do Mega
#define EN PB1 // Pino 52 do Mega

// Protótipos traduzidos
void lcd_inicializar(void);
void lcd_comando(unsigned char comando);
void lcd_caractere(unsigned char dado);
void lcd_escrever_string(char* texto);
void lcd_posicionar(unsigned char linha, unsigned char coluna);
void lcd_limpar(void);

#endif