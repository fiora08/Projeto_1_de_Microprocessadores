#ifndef INTERFACE_H
#define INTERFACE_H

#define BLOQUEADO 0
#define LIBERADO  1
#define ERRO      2

extern unsigned char estado_atual;

unsigned char mascarar_senha(unsigned char *senha_out, unsigned char tamanho_senha);

#endif