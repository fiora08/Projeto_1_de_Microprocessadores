#ifndef TIMERS_H
#define TIMERS_H

# include <avr/io.h>

// variavel de flag 1 mili segundo do timer 0
extern volatile unsigned char flag_1ms; 
// função de inicialização do timer 0
void timer0_init(void);


extern volatile unsigned char flag_2s;        // ativa quando atinge 2 segundos
extern volatile unsigned int contador_serial;  //  milissegundos para o timeout
extern volatile unsigned long contador_timeout; //  milissegundos para disparar a o led fora do ar

// variaveis para o relogio do sistema
extern volatile int dia, mes, h, min, seg;
// inicializa o  timer 1;
void timer1_init(void);
void atraso_ms(unsigned int milissegundos);
unsigned char tempo_n_bloqueante(unsigned int tempo);
void mostrar_hora();
void mostrar_data();
unsigned char alterar_hora(unsigned char tecla);
unsigned char alterar_minuto(unsigned char tecla);
unsigned char alterar_mes(unsigned char tecla);
unsigned char alterar_dia(unsigned char tecla);
//unsigned char alterar_minuto(void);

#endif