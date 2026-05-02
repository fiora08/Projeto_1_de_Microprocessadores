#ifndef TECLADO_H
#define TECLADO_H

#define DEBOUNCE_MAXIMO 20

// Prot�tipos das funções
void teclado_inicializar(void);
unsigned char teclado_ler_bruto(void);
void teclado_atualizar(void);
unsigned char teclado_obter_tecla(void);
void teclado_resetar_estado(void);

#endif