#include "maquina_protocolo.h"
#include "serial.h"

static unsigned char estado_protocolo = IDLE;


if(serial_disponivel()==1){
switch(estado_protocolo){
    
    case IDLE:
        if(serial_ler() == 'S'){
            estado_protocolo =LENDO_TIPO;
        }
        break;
    case LENDO_TIPO:
        break
    case LENDO_N
        break;
    case LENDO_DADOS
        break;

}
}