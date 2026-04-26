#include "teclado.h"
#include "lcd.h"
void mascarar_senha(){
    unsigned char tecla = teclado_obter_tecla();
	static unsigned char senha[4];
	static unsigned char pos = 0;
    if(tecla!=0){
        //if(estado_atual == ESPERANDO_SENHA){
            if(pos<4){
            senha[pos++] = tecla;
            lcd_caractere('*');
            }
        }


    }
	
//}