#include "serial.h" 
#include <string.h>




unsigned char enviar_login(unsigned char usuario_autenticado){

    char msg_login[4];
    char ACK;

    msg_login[0]= 'M';
    msg_login[1] = 'L';
    msg_login[3] = '\0';

    if(usuario_autenticado ==2)
        msg_login[2] = '9';

    else{
        msg_login[2] =  '0'+ usuario_autenticado;
    }
    serial_enviar_triplo(msg_login, &ACK);

    return ACK;
    
}

unsigned char enviar_logoff(unsigned char usuario_autenticado){

    char msg_logoff[4];
    char ACK;

    msg_logoff[0]= 'M';
    msg_logoff[1] = 'O';
    msg_logoff[3] = '\0';

    if(usuario_autenticado ==2)
        msg_logoff[2] = '9';

    else{
        msg_logoff[2] =  '0'+ usuario_autenticado;
    }
    serial_enviar_triplo(msg_logoff, &ACK);

    return ACK;
    
}

unsigned char enviar_estorno( char bandeira, char cartao[6], char *valor_estorno){
    unsigned char n = 0;
    unsigned char tam_mensagem = 0;
    char ACK;

    n = 8 + strlen(valor_estorno);
    tam_mensagem = 11 + strlen(valor_estorno);
    char msg_estorno[tam_mensagem];


    msg_estorno[0] = 'M';
    msg_estorno[1] = 'E';
    msg_estorno[2] = n ;
    msg_estorno[3] = bandeira;
    strcpy(&msg_estorno[4], cartao);
    strcpy(&msg_estorno[10], valor_estorno);
    //pelo que vi strcpy copia no ultimo local do array um \0 entao a parte debaixo ficava redundante
    //msg_estorno[tam_mensagem-1] = '\0'; 
    serial_enviar_triplo(msg_estorno, &ACK);
    
    return ACK;

}

unsigned char enviar_venda( char bandeira, char cartao[6],char senha[6], char *valor_venda){
    unsigned char n = 0;
    unsigned char tam_mensagem = 0;
    char ACK;

    n = 14 + strlen(valor_venda);
    tam_mensagem = 17 + strlen(valor_venda);
    char msg_venda[tam_mensagem];


    msg_venda[0] = 'M';
    msg_venda[1] = 'V';
    msg_venda[2] = n ;
    msg_venda[3] = bandeira;
    strcpy(&msg_venda[4], cartao);
    strcpy(&msg_venda[10], senha);
    strcpy(&msg_venda[16], valor_venda);
    //pelo que vi strcpy copia no ultimo local do array um \0 entao a parte debaixo ficava redundante
    //msg_venda[tam_mensagem-1] = '\0'; 
    serial_enviar_triplo(msg_venda, &ACK);

    return ACK;

}

/*unsigned char receber_login(usuario *novo_usuario){
    unsigned char nome_logado = 0;
    char campo_dados[nome_logado];
    for (int i= 0; i<nome_logado; i++) {
        campo_dados[i] = serial_ler();
    }
    if (strcmp(novo_usuario[i].nome, campo_dados)) {
    
    }
}*/
