#ifndef ENERGIA_H
#define ENERGIA_H

// Estados possíveis do sistema MicPay
#define DESLIGADO 0
#define LIGADO    1

// Protótipos das funções
void energia_inicializar(void);
void energia_gerenciar(void);
unsigned char energia_sistema_ativo(void);

#endif
