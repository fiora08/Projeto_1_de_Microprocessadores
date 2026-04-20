#include <avr/io.h>
#include "teclado.h"
#include "timers.h" // Necessário para a flag_1ms



// Variáveis internas da biblioteca
static unsigned char tecla_estavel = 0;
static unsigned char tecla_atual = 0;
static unsigned char contador_estabilidade = 0;

// Mapa de teclas conforme o teclado alfanumérico de 16 teclas 
static const unsigned char mapa_teclas[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

// Realiza a varredura física nos pinos do PORTA
unsigned char teclado_ler_bruto(void) {
	unsigned char linha, coluna_lida;
	for (linha = 0; linha < 4; linha++) {
		// O deslocamento 0x10 começa em PA4
		PORTA = ~(0x10 << linha) | 0x0F;
		// Pequena espera para estabilização elétrica dos pinos
		for (volatile unsigned char i = 0; i < 10; i++) asm("nop"); // "asm(nop)" é um atraso de nanosegundos para dar um respiro a função.
		// Lê o estado das colunas nos pinos PA0-PA3
		coluna_lida = PINA & 0x0F;
		// Verifica se algum bit de coluna é 0 que é tecla pressionada
		if (coluna_lida != 0x0F) {
			// verifica bit a bit se a coluna foi pressionada
			for (unsigned char col = 0; col < 4; col++) {
				if (!(coluna_lida & (1 << col))) {
					return mapa_teclas[linha][col]; // retorna o caracter pressionado.
				}
			}
		}
	}
	return 0; // Nenhuma tecla detectada
}

void teclado_inicializar(void) {
	// PA4-PA7 como saídas e PA0-PA3 como entradas 
	DDRA = 0xF0;
	// Ativa resistores de Pull-up nas entradas (PA0-PA3)
	PORTA = 0xFF;
}

void teclado_atualizar(void) {
	// Sincroniza com a base de tempo de 1ms do Timer 0
	if (flag_1ms) {
		unsigned char leitura = teclado_ler_bruto();
		// aqui tem a verificação de ruido
		if (leitura != tecla_atual) {
			contador_estabilidade = 0;
			tecla_atual = leitura;
			} else if (leitura != 0) { // se a leitura for estavel incrementamos o tempo 
			if (contador_estabilidade < DEBOUNCE_MAXIMO) {
				contador_estabilidade++;
			}
		}
		// Se a tecla permanecer estável pelo tempo definido 20ms
		if (contador_estabilidade >= DEBOUNCE_MAXIMO) {
			tecla_estavel = tecla_atual;
			} else if (leitura == 0) {
			// Reseta se a tecla for solta
			tecla_estavel = 0;
			contador_estabilidade = 0;
			tecla_atual = 0;
		}
	}
}
// função para ler a tecla e retornar a tecla pressionada
unsigned char teclado_obter_tecla(void) {
	static unsigned char ultima_tecla_enviada = 0;
	unsigned char retorno = 0;
	// so envia se a tecla for diferente da tecla enviada por ultimo
	if (tecla_estavel != 0 && ultima_tecla_enviada == 0) {
		retorno = tecla_estavel;
	}
	ultima_tecla_enviada = tecla_estavel; // trava o envio até o usuario parar de pressionar a tecla
	return retorno;
}
