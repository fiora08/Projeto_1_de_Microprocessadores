	#include "interface.h"
	#include "teclado.h"
	#include "lcd.h"

    unsigned char DESLIGADO;
    unsigned char LIGADO;
    // Cria a variavel global iniciando bloqueada
	unsigned char estado_atual = BLOQUEADO;
    unsigned char DESBLOQUEADO;