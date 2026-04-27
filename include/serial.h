#ifndef SERIAL_H_
#define SERIAL_H_

#define TAM_BUFFER 64 // pensado para garantir que receba strings grandes e com isso não perca os dados.

void serial_inicializar(unsigned int valor_ubrr); // configura os pinos na inicialização e a paridade e velocidade
void serial_transmitir(unsigned char dado); // envia um unico caracter via tx
void serial_escrever(const char* texto); //  envia frases inteiras 
unsigned char serial_disponivel(void); // função que retorna verdadeiro se houver algum dado novo no buffer que ainda não foi lido
char serial_ler(void); //retira o dado mais antigo na fila de espera e entrega ao programa.
void serial_enviar_triplo(const char* comando, char *sucesso); // envia o comando até 3 vezes esperando 2 segundos a cada tentativa . 

extern volatile char erro_com_flag;
extern volatile char fora_do_ar_flag;

#endif

