#ifndef TIMERS_H
#define TIMERS_H

# include <avr/io.h>

// variavel de flag 1 mili segundo do timer 0
extern volatile unsigned char flag_1ms; 
// fun��o de inicializa��o do timer 0
void timer0_init(void);


extern volatile unsigned char flag_2s;        // ativa quando atinge 2 segundos
extern volatile unsigned int contador_serial;  //  milissegundos para o timeout
extern volatile unsigned long contador_timeout; //  milissegundos para disparar a o led fora do ar

// variaveis para o rel�gio do sistema
extern volatile int h, min, seg;
// inicializa o  timer 1;
void timer1_init(void);
void atraso_ms(unsigned int milissegundos);
unsigned char tempo_n_bloqueante(unsigned int tempo);
void mostrar_hora();
unsigned char alterar_hora(unsigned char tecla);
unsigned char alterar_minuto(unsigned char tecla);
//unsigned char alterar_minuto(void);

#endif