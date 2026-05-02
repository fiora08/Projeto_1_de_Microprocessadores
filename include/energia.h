#ifndef ENERGIA_H
#define ENERGIA_H

#include "maquina.h"
// Estados poss�veis do sistema MicPay

extern unsigned char sistema_ja_ligado;

// Prot�tipos das fun��es
void energia_inicializar(void);
void energia_gerenciar(void);
unsigned char energia_sistema_ativo(void);

#endif
