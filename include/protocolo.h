#ifndef PROTOCOLO_H
#define PROTOCOLO_H


unsigned char enviar_login(unsigned char usuario_autenticado);
unsigned char enviar_logoff(unsigned char usuario_autenticado);
unsigned char enviar_venda( char bandeira, char cartao[6],char senha[6], char *valor_venda);
unsigned char enviar_venda_parcelada( char bandeira, char cartao[6],char senha[6], char parcelas, char *valor_venda);
unsigned char enviar_estorno( char bandeira, char cartao[6], char *valor_estorno);
unsigned char enviar_agendamento( char bandeira, char cartao[6], char *valor_agendamento);


#endif