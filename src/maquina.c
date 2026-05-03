	#include "interface.h"
	#include "lcd.h"
    #include "maquina.h"
    #include "teclado.h"
    

   /* unsigned char DESLIGADO;
    unsigned char LIGADO;
    // Cria a variavel global iniciando bloqueada
	
    unsigned char DESBLOQUEADO;
    */
    unsigned char estado_atual = BLOQUEADO;


    /*void maquina_atualizar(void) {
    switch(estado_tela) {
        case INICIANDO:    iniciando();    break;
        case AGUARDANDO:   aguardando();   break;
        case BEM_VINDO:    bem_vindo();    break;
        case MENU_ADMIN:   menu_admin();   break;
        case CONFIGURACAO: configuracao(); break;
        case OPERANDO:     operando();     break;
    }
}*/