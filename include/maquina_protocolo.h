#ifndef MAQUINA_PROTOCOLO_H
#define MAQUINA_PROTOCOLO_H

#define IDLE  0
#define LENDO_TIPO 1
#define LENDO_N 2
#define LENDO_PAYLOAD 3
#define LENDO_DADOS_FIXOS 4

unsigned char maquina_protocolo();
char* protocolo_get_mensagem(void);
#endif