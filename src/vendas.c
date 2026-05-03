#include <avr/io.h>
#include <string.h>
#include "teclado.h"
#include "maquina.h"
#include "lcd.h"
#include "vendas.h"

//  Variáveis internas
static unsigned char tecla;
static unsigned char ind_vds = 0;
static unsigned char ind_val = 0;
static unsigned char ind_num = 0;
static unsigned char ind_sen = 0;
static unsigned char inc_cod = 0;
static unsigned char qual_venda;
static unsigned char estado_venda = TIPO;
venda n_vendas[5];

unsigned char vendas(unsigned char inicio){
	
	if(estado_atual == DESBLOQUEADO)
	{
		tecla = teclado_obter_tecla();
		// Limpar a tela na MAIN
		if(inicio == 1)
		{
			lcd_posicionar(0, 0);
			lcd_escrever_string("A.V.-1");
			lcd_posicionar(0, 8);
			lcd_escrever_string("PRAZO-2");
			lcd_posicionar(1, 0);
			lcd_escrever_string("Selecione:");
		}

		if(tecla != 0 && ((tecla - '0') <= 2) && estado_venda == TIPO)
		{
			qual_venda = tecla - '0';
			lcd_caractere(tecla);
		}else if(tecla == 'D' && estado_venda == TIPO)
			{
				if(qual_venda == VENDA_VISTA) n_vendas[ind_vds].codigo = inc_cod++;
				estado_venda = VALOR;
				lcd_limpar();
				//Ler o valor da compra
				lcd_posicionar(0, 0);
				lcd_escrever_string("VALOR:");	
			}

		switch(qual_venda)
		{
			case VENDA_VISTA:
				// Vai salvar o tipo da venda em um dos indices do struct vendas
				n_vendas[ind_vds].tipo_venda = VENDA_VISTA;
				
				if(tecla != 0 && estado_venda == VALOR)
				{
					if(ind_val < 5 && tecla != 'D')
					{
						n_vendas[ind_vds].valor_venda[ind_val] = tecla;
						//Imprimir valor na tela
						lcd_caractere(n_vendas[ind_vds].valor_venda[ind_val]);
						ind_val++;
					} else if (tecla == 'D')
						{
							// Caso o CONFIRMA - tecla D - for apertado sai do estado VALOR e finaliza o vetor de venda
							estado_venda = OPCAO; 
							n_vendas[ind_vds].valor_venda[ind_val] = '\0';
							ind_val = 0;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("OP: DB-1");
							lcd_posicionar(0, 10);
							lcd_escrever_string("CRED-2");
						}
				}
				
				break;
			case VENDA_PARCELADA:
				// Vai salvar o tipo da venda em um dos indices do struct vendas
				n_vendas[ind_vds].tipo_venda = VENDA_PARCELADA; 

				if(tecla != 0 && estado_venda == VALOR)
				{
					if(ind_val < 5 && tecla != 'D')
					{
						n_vendas[ind_vds].valor_venda[ind_val] = tecla;
						//Imprimir valor na tela
						lcd_caractere(n_vendas[ind_vds].valor_venda[ind_val]);
						ind_val++;
					} else if (tecla == 'D')
						{
							// Caso o CONFIRMA - tecla D - for apertado sai do estado VALOR e finaliza o vetor de venda
							estado_venda = NUM_PARCELAS; 
							n_vendas[ind_vds].valor_venda[ind_val] = '\0';
							ind_val = 0;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("PARCELAS:");
						}
				}
				
				if(estado_venda == NUM_PARCELAS && tecla != 0 && ((tecla - '0') <= 3))
				{	
					n_vendas[ind_vds].num_parcelas = tecla;
				}else if(estado_venda == NUM_PARCELAS && tecla == 'D')
					{
						estado_venda = OPCAO;
						lcd_limpar();
						//Ler a opção de compra
						lcd_posicionar(0, 0);
						lcd_escrever_string("OP: DB-1");
						lcd_posicionar(0, 10);
						lcd_escrever_string("CRED-2");
					}

				break;
			default:
				break;
		}

				if(estado_venda == OPCAO && ((tecla-'0') == 1))
				{	
					n_vendas[ind_vds].opcao_venda = DEBITO;
				}else if(estado_venda == OPCAO && ((tecla-'0') == 2))
					{	
						n_vendas[ind_vds].opcao_venda = CREDITO;
					}else if(estado_venda == OPCAO && tecla == 'D')
						{
							estado_venda = BANDEIRA;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("BANDEIRA:");
						}

				if(estado_venda == BANDEIRA && tecla != 'D' && tecla != 0)
				{	
					n_vendas[ind_vds].bandeira = tecla;
				}else if(estado_venda == BANDEIRA && tecla == 'D')
					{
						estado_venda = NUM_CARTAO;
						lcd_limpar();
						//Ler a opção de compra
						lcd_posicionar(0, 0);
						lcd_escrever_string("N. CARTAO:");
					}

				if(estado_venda == NUM_CARTAO && tecla != 0)
				{
					if (ind_num < 6 && tecla != 'D') {
						n_vendas[ind_vds].num_cartao[ind_num] = tecla;
						lcd_caractere(tecla);
						ind_num++;
					}else if (tecla == 'D' &&  ind_num == 6)
						{
							// Caso CONFIRMA - tecla D - for apertado sai do estado VALOR e finaliza o vetor numero do cartão
							estado_venda = SEN_CARTAO; 
							n_vendas[ind_vds].num_cartao[ind_num] = '\0';
							ind_num = 0;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("SENHA:");	
						}
				}

				if(estado_venda == SEN_CARTAO && tecla != 0)
				{
					if (ind_sen < 6 && tecla != 'D') {
						n_vendas[ind_vds].senha_cartao[ind_sen] = tecla;
						lcd_caractere('*');
						ind_sen++;
					}else if(ind_sen == 6 && tecla == 'D')
						{
							// Caso o CONFIRMA - tecla D - for apertado sai do estado VALOR e finaliza o vetor numero do cartão
							estado_venda = TIPO; 
							n_vendas[ind_vds].senha_cartao[ind_sen] = '\0';
							ind_sen = 0;
							lcd_limpar();
							qual_venda = 0;	
							ind_vds++;
							if(ind_vds > 4)	ind_vds = 0;
							return 1;
						}
				}

				return 0;
	}
	return 0;
}