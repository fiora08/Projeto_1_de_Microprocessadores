#include "maquina_protocolo.h"
#include "maquina.h"
#include "serial.h"
#include <math.h>
#include <string.h>

static unsigned char estado_protocolo = IDLE;
static unsigned char tipo_mensagem = 0;
static unsigned char n;
static unsigned char resposta_venda[3];
static char mensagem[16];
void maquina_protocolo(){

    if(serial_disponivel()==1){
        tipo_mensagem = serial_ler();
        switch(estado_protocolo){
            
            case IDLE:
                
                if(tipo_mensagem == 'S'){
                    estado_protocolo =LENDO_TIPO;
                }
                break;
            case LENDO_TIPO:
                tipo_mensagem = serial_ler();
                switch(tipo_mensagem){
                    case 'L':
                        estado_protocolo = LENDO_N;
                        break;
                    case 'M':
                        //lendo cartao magnetico
                        estado_protocolo = LENDO_N;
                        break;

                    case 'W':
                        //lendo cartao wireless
                        estado_protocolo = LENDO_N;
                        break;
                    
                    case 'O':
                        //login ok, mensagem ja fixa
                        strcpy(mensagem, "SO"); 
                        break;

                    case 'X':
                        //falha de login
                        strcpy(mensagem, "SX");
                        break;

                    case 'V':
                        strcpy(mensagem, "SV");
                        //feita venda
                        estado_protocolo = LENDO_DADOS;
                        break;
                    
                    case 'P':
                        
                        //feita parcelada
                        strcpy(mensagem, "SP");
                        estado_protocolo = LENDO_DADOS;
                        break;

                    case 'A':
                        
                        //feito agendamento
                        strcpy(mensagem, "SA");
                        estado_protocolo = LENDO_DADOS;
                        break;
                    
                }
                break;
            case LENDO_N:
                n = tipo_mensagem;
                //char mensagem[n+3];
                break;
            case LENDO_DADOS:
                //tipo_mensagem = serial_ler();
                switch(tipo_mensagem){
                    case 'V':
                        mensagem[2] = 'V';
                        //venda com sucesso 
                        break;
                    case 'C': 
                        mensagem[2] = 'C';
                        //cartao com falha
                        break;
                    case 'S': 
                        //senha com falha
                        mensagem[2] = 'S';
                        break;
                    case 'I':
                        //saldo insufuciente
                        mensagem[2] = 'I';
                        break;
                    case 'N':
                        //pagamento nao localizado
                        mensagem[2] = 'N';
                        break;
                }   
                break;

        }
    }
}