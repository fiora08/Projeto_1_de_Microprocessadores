#include <string.h>
#include "interface.h"
#include "teclado.h"
#include "senhas.h"
#include "maquina.h"
unsigned char autenticar_usuario(usuario *novo_usuario ,unsigned  char leitura_teclado[] ){
    for(int i=0; i<3; i++){
        // esse if usa a função de comparar strings de string.h para ver se a leitura do teclado ta igual à senha colocada no campo senha da struct
        if(strcmp((const char *)novo_usuario[i].senha, (const char *)leitura_teclado) == 0)
            return estado_atual = DESBLOQUEADO;
            //tive que fazer isso de (const char *) pq a strcmp usa char padrao e nao o unsigned, dai quando declarei unsigned char nos parametros dela ficou dando warning ao compilar  
    }
        estado_atual = BLOQUEADO;
    return estado_atual;
}