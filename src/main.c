
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

int main(void) {
	// Inicialização dos periféricos
	serial_inicializar(51); //-> formula para dar o 119200 com 16 mhz foi definido o 51 para a velocidade baud. 
	timer0_init();
	timer1_init();
	teclado_inicializar();
	lcd_inicializar();
	energia_inicializar();
	
	sei(); // Habilita interrup��es

	//inicialização dos usuários 
	usuario  novo_usuario[3];

	strcpy(novo_usuario[0].nome ,"operador0");
	strcpy(novo_usuario[0].senha , "1254");

	strcpy(novo_usuario[1].nome ,"operador1");
	strcpy(novo_usuario[1].senha , "2349");

	strcpy(novo_usuario[2].nome ,"administrador");
	strcpy(novo_usuario[2].senha , "0738");

	unsigned char tecla=0;
	char buffer_lcd[17]; // 16 caracteres + '\0'
	unsigned char index = 0;
	unsigned char sistema_ja_ligado = 0;




while (1) {
    energia_gerenciar();
    if (energia_sistema_ativo() == LIGADO) {
        teclado_atualizar();

		
		if (sistema_ja_ligado == 0) {
			lcd_limpar();
			lcd_posicionar(0, 2);
			lcd_escrever_string("MicPay 2026");
			lcd_posicionar(1, 1);
			lcd_escrever_string("Sistema Ativo");
			sistema_ja_ligado = 1;
		}
		if (sistema_ja_ligado == 1 && tempo_n_bloqueante(1500)) {
			lcd_limpar();
			lcd_posicionar(0, 0);
			lcd_escrever_string("Bloqueado:");
			sistema_ja_ligado = 2;
			estado_atual = BLOQUEADO;
		}
		
		



        if (estado_atual == BLOQUEADO && sistema_ja_ligado ==2  ) {
            unsigned char usuario_autenticado;
            mascara_autentica_senha(novo_usuario, 4, &usuario_autenticado);
        }
        if (estado_atual == DESBLOQUEADO) {
			vendas(1); //Acho que teria que chamar aqui 
            tecla = teclado_obter_tecla();
            if (tecla != 0) {
                lcd_caractere(tecla);
            }
        }
    }
	flag_1ms=0;
}


return 0;


}


//estava sem funcionar após autenticar
/*	while (1) {


	
		energia_gerenciar();	
		//if (flag_1ms) {
		//energia_gerenciar();
		//	teclado_atualizar(); 
		//	 flag_1ms = 0;	
		//}
		 //
	 

		if (energia_sistema_ativo() == LIGADO) {
		teclado_atualizar();	
		//if (flag_1ms) {
		
			
			 	
		//}
			
			// --- INICIALIZAÇÃO VISUAL ---
			if (!sistema_ja_ligado) {
				lcd_limpar(); //
				lcd_posicionar(0, 2); // Centraliza levemente "MicPay 2026"
				lcd_escrever_string("MicPay 2026");
				lcd_posicionar(1, 1);
				lcd_escrever_string("Sistema Ativo");
				//_delay_ms(1500);
				atraso_ms(1500);
				lcd_limpar();
				lcd_escrever_string("Bloqueado:");
				//_delay_ms(1500);
				sistema_ja_ligado = 1;


			}

			// --- ESCUTA SERIAL (LINHA 0) ---
			if (serial_disponivel()) {
				char recebido = serial_ler();
				
				// Trata Enter ou limite de 16 colunas do LCD
				if (recebido == '\n' || recebido == '\r' || index >= 16) {
					buffer_lcd[index] = '\0';
					
					// Limpa apenas a linha 0 para novos dados da serial
					lcd_posicionar(0, 0);
					lcd_escrever_string("TIAGO"); // 16 espa�os para limpar
					lcd_posicionar(0, 0);
					lcd_escrever_string(buffer_lcd);
					index = 0;
					} else {
					buffer_lcd[index++] = recebido;
				}
			}


			// --- FALA TECLADO (LINHA 1) ---
			teclado_atualizar(); //
			tecla = teclado_obter_tecla();
			
			if (tecla != 0) {
				serial_transmitir(tecla);
				
				// Limpa a parte de dados da linha 1 (coluna 9 em diante)
				lcd_posicionar(1, 0);
				lcd_escrever_string("Teclado:� � � � "); // Mant�m o r�tulo e limpa o resto
				lcd_posicionar(1, 9);
				lcd_caractere(tecla); //
			}
			

			// --- FALA TECLADO (LINHA 1) ---
			//autenticação de senha
			//
			if(estado_atual == BLOQUEADO ){
				unsigned char senha_login[4]= "0738";
				unsigned char usuario_autenticado;
				mascara_autentica_senha(novo_usuario, 4, &usuario_autenticado,(unsigned char *)senha_login);
				if (estado_atual==DESBLOQUEADO) {
				
				//}
					//autenticar_usuario(novo_usuario, senha_login,&usuario_autenticado );
					lcd_posicionar(1, 0);
					//lcd_escrever_string("user:");
					lcd_escrever_string(novo_usuario[usuario_autenticado].nome);
					//atraso_ms(1500);
					lcd_limpar();
					if(estado_atual==BLOQUEADO){
					lcd_posicionar(1, 0);					
					lcd_escrever_string("senha invalida");
					atraso_ms(1500);
				}
				}
				
			}

			else if (estado_atual == DESBLOQUEADO) {
				//teclado_atualizar();
				//serial_transmitir(tecla);
				tecla = teclado_obter_tecla();
				// Limpa a parte de dados da linha 1 (coluna 9 em diante)
				lcd_posicionar(0, 0);
				if (tecla == '1') {
					lcd_escrever_string("Teclado:");
				}
				 atraso_ms(3000);
				 lcd_escrever_string("cu");
				// Mantém o rótulo e limpa o resto
				lcd_posicionar(1, 9);
				lcd_caractere(tecla); 
			}
			//else{estado_atual = ERRO;
			//}
			
			
			
			flag_1ms = 0;//
			} else {
			sistema_ja_ligado = 0;
		}




	}
	return 0;
}
	*/