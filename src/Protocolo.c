#include "serial.h" 




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