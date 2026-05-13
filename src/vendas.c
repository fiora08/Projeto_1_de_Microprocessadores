#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "maquina.h"
#include "timers.h"
#include "serial.h"
#include "lcd.h"
#include "teclado.h"
#include "energia.h"
#include "interface.h"
#include "senhas.h"
#include "vendas.h"

//  Variáveis internas
static unsigned char tecla;
static unsigned char ind_vds = 0;
static unsigned char ind_val = 0;
static unsigned char ind_num = 0;
static unsigned char ind_sen = 0;
static unsigned char inc_cod = 0;
static unsigned char cont_D = 0;
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
			lcd_escrever_string("Vist(1)");
			lcd_posicionar(0, 8);
			lcd_escrever_string("Praz(2)");
			lcd_posicionar(1, 0);
			lcd_escrever_string(">>");
		}

		if(tecla != 0 && ((tecla - '0') <= 2) && estado_venda == TIPO)
		{
			qual_venda = tecla - '0';
			lcd_posicionar(1, 2);
			lcd_caractere(tecla);
		}else if(tecla == 'D' && estado_venda == TIPO)
			{
				if(qual_venda == VENDA_VISTA) n_vendas[ind_vds].codigo = inc_cod++;
				estado_venda = VALOR;
				lcd_limpar();
				//Ler o valor da compra
				lcd_posicionar(0, 0);
				lcd_escrever_string("Valor:");
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
						lcd_posicionar(0, (6+ind_val));
						lcd_caractere(tecla);
						ind_val++;
					} else if (tecla == 'D' && n_vendas[ind_vds].valor_venda[0] !=0) //n_vendas[ind_vds].valor_venda[0] !=0
						{
							// Caso o CONFIRMA - tecla D - for apertado sai do estado VALOR e finaliza o vetor de venda
							estado_venda = OPCAO; 
							n_vendas[ind_vds].valor_venda[ind_val] = '\0';
							ind_val = 0;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("Deb(1)");
							lcd_posicionar(0, 7);
							lcd_escrever_string("Cred(2)");
							lcd_posicionar(1, 0);
							lcd_escrever_string(">>");
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
					} else if (tecla == 'D' && n_vendas[ind_vds].valor_venda[0] != 0)
						{
							// Caso o CONFIRMA - tecla D - for apertado sai do estado VALOR e finaliza o vetor de venda
							estado_venda = NUM_PARCELAS; 
							n_vendas[ind_vds].valor_venda[ind_val] = '\0';
							ind_val = 0;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("Parcelas:");
						}
				}
				
				if(estado_venda == NUM_PARCELAS && tecla != 0 && ((tecla - '0') <= 3))
				{	
					lcd_posicionar(0, 9);
					lcd_caractere(tecla);
					n_vendas[ind_vds].num_parcelas = tecla;
					cont_D = 3;
				}else if(estado_venda == NUM_PARCELAS && tecla == 'D' && cont_D == 3)
					{
						estado_venda = OPCAO;
						lcd_limpar();
						//Ler a opção de compra
						lcd_posicionar(0, 0);
						lcd_escrever_string("Deb(1)");
						lcd_posicionar(0, 7);
						lcd_escrever_string("Cred(2)");
						lcd_posicionar(1, 0);
						lcd_escrever_string(">>");
					}

				break;
			default:
				break;
		}
			//ESTADO OPÇÃO DE VENDA (DEBITO OU CREDITO)
				if(estado_venda == OPCAO && ((tecla-'0') == 1))
				{	
					lcd_posicionar(1, 2);
					lcd_caractere(tecla);
					n_vendas[ind_vds].opcao_venda = DEBITO;
					cont_D = 1;
				}else if(estado_venda == OPCAO && ((tecla-'0') == 2))
					{	
						lcd_posicionar(1, 2);
						lcd_caractere(tecla);
						n_vendas[ind_vds].opcao_venda = CREDITO;
						cont_D = 1;
					}else if(estado_venda == OPCAO && tecla == 'D' && cont_D == 1)
						{
							estado_venda = BANDEIRA;
							lcd_limpar();
							//Ler a opção de compra
							lcd_posicionar(0, 0);
							lcd_escrever_string("Bandeira:");
						}

				if(estado_venda == BANDEIRA && tecla != 'D' && tecla != 0)
				{	
					lcd_posicionar(0, 9);
					lcd_caractere(tecla);
					n_vendas[ind_vds].bandeira = tecla;
					cont_D = 2;
				}else if(estado_venda == BANDEIRA && tecla == 'D' && cont_D == 2)
					{
						estado_venda = NUM_CARTAO;
						lcd_limpar();
						//Ler a opção de compra
						lcd_posicionar(0, 0);
						lcd_escrever_string("N.CARTAO:");
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
							lcd_escrever_string("Senha:");	
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
							lcd_limpar();
							lcd_posicionar(0, 4);
							lcd_escrever_string("Compra");
							lcd_posicionar(1, 2);
							lcd_escrever_string("Finalizada!");
							return 1;
						}
				}

				return 0;
	}
	return 0;
}