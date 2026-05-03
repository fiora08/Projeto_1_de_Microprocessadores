#ifndef VENDAS_H
#define VENDAS_H

#define VENDA_VISTA 1
#define VENDA_PARCELADA 2
#define TIPO 3
#define VALOR 4
#define NUM_PARCELAS 5
#define OPCAO 6
#define BANDEIRA 7
#define NUM_CARTAO 8
#define SEN_CARTAO 9
#define DEBITO 10
#define CREDITO 11


typedef struct venda {
    int tipo_venda;
    char valor_venda[6];
    char num_parcelas;
    int opcao_venda;
    char bandeira;
    char num_cartao[7];
    char senha_cartao[7];
} venda;

extern venda n_vendas[5];
unsigned char vendas(unsigned char inicio);
#endif