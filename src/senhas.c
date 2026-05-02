#include <string.h>
#include "interface.h"
#include "teclado.h"
#include "senhas.h"
#include "maquina.h"
#include "lcd.h"
/*unsigned char autenticar_usuario(usuario *novo_usuario ,unsigned  char leitura_teclado[], unsigned char *indice_out){
    for(int i=0; i<3; i++){
        // esse if usa a função de comparar strings de string.h para ver se a leitura do teclado ta igual à senha colocada no campo senha da struct
        if(strcmp((const char *)novo_usuario[i].senha, (const char *)leitura_teclado) == 0){
            *indice_out = i;
         return estado_atual = DESBLOQUEADO;
        }
            //tive que fazer isso de (const char *) pq a strcmp usa char padrao e nao o unsigned, dai quando declarei unsigned char nos parametros dela ficou dando warning ao compilar  
    }
        estado_atual = BLOQUEADO;
    return estado_atual;
}*/

/*unsigned char autenticar_usuario(usuario *novo_usuario , unsigned char *indice_out){
    for(int i=0; i<3; i++){
        // esse if usa a função de comparar strings de string.h para ver se a leitura do teclado ta igual à senha colocada no campo senha da struct
        if(strcmp((const char *)novo_usuario[i].senha, (const char *)leitura_teclado) == 0){
            *indice_out = i;
         return estado_atual = DESBLOQUEADO;
        }
            //tive que fazer isso de (const char *) pq a strcmp usa char padrao e nao o unsigned, dai quando declarei unsigned char nos parametros dela ficou dando warning ao compilar  
    }
        estado_atual = BLOQUEADO;
    return estado_atual;
}*/

unsigned char mascara_autentica_senha( usuario *novo_usuario, unsigned char tamanho_senha, unsigned char *indice_out) {
		
		unsigned char tecla = teclado_obter_tecla();
		
		static unsigned char senha[6];
		static unsigned char pos = 0;
		
		if (tecla != 0) {
			if (pos < tamanho_senha) {
				senha[pos++] = tecla;
				lcd_caractere('*');
			}
		}
		
		if (pos == tamanho_senha) {
            senha[tamanho_senha]='\0'; //autenticação de senha
            for(int i=0; i<3; i++){
                // esse if usa a função de comparar strings de string.h para ver se a leitura do teclado ta igual à senha colocada no campo senha da struct
                if(strcmp((const char *)novo_usuario[i].senha, (const char *)senha) == 0){
                    *indice_out = i;
                    pos = 0;
                    
                //tive que fazer isso de (const char *) pq a strcmp usa char padrao e nao o unsigned, dai quando declarei unsigned char nos parametros dela ficou dando warning ao compilar  
                return estado_atual = DESBLOQUEADO;
        } 
    }
        pos = 0;
		return estado_atual = BLOQUEADO;
		}
		
		return estado_atual;
    
	}

/*unsigned char mascara_autentica_senha(usuario *novo_usuario,
                                      unsigned char tamanho_senha,
                                      unsigned char *indice_out,
                                      unsigned char *tecla) {

    unsigned char senha[6];
    unsigned char pos = 0;

    // Copia da string recebida
    while (tecla[pos] != '\0' && pos < tamanho_senha) {
        senha[pos] = tecla[pos];
        lcd_caractere('*');
        pos++;
    }

    // Se não completou tamanho esperado, sai
    if (pos != tamanho_senha) {
        return estado_atual;
    }

    senha[tamanho_senha] = '\0';

    for (int i = 0; i < 3; i++) {
        if (strcmp((const char *)novo_usuario[i].senha,
                   (const char *)senha) == 0) {
            *indice_out = i;
            return estado_atual = DESBLOQUEADO;
        }
    }

    return estado_atual = BLOQUEADO;
}*/