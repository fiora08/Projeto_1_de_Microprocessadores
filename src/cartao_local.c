#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/iom2560.h>
#include <util/delay.h>
#include <string.h>
#include "maquina.h"
#include "timers.h"
#include "serial.h"
#include "lcd.h"
#include "teclado.h"
#include "energia.h"
#include "interface.h"
#include "senhas.h"
#include "vendas.h"
#include "maquina_protocolo.h"
#include "protocolo.h"
#include "login.h"
#include "admin.h"
#include "vendas.h"


unsigned char cadastrar_cartao_local(cartao_local1 *cartoes, unsigned char num_cartoes){
    unsigned char p = 0;
    
    lcd_limpar();
    lcd_escrever_string("N.CARTAO:");
    lcd_posicionar(1, 0);
    
    while(p < 9){
        teclado_atualizar();
        unsigned char tecla = teclado_obter_tecla();
        if(tecla >= '0' && tecla <= '9'){
            cartoes[num_cartoes].numero_cartao[p++] = tecla;
            lcd_caractere(tecla);
        }
        if(tecla == 'D') break;
    }
    cartoes[num_cartoes].numero_cartao[p] = '\0';
    
    // senha
    p = 0;
    lcd_limpar();
    lcd_escrever_string("SENHA:");
    lcd_posicionar(1, 0);
    
    while(p < 7){
        teclado_atualizar();
        unsigned char tecla = teclado_obter_tecla();
        if(tecla >= '0' && tecla <= '9'){
            cartoes[num_cartoes].senha_cartao[p++] = tecla;
            lcd_caractere('*');
        }
        if(tecla == 'D') break;
    }
    cartoes[num_cartoes].senha_cartao[p] = '\0';
    cartoes[num_cartoes].saldo = 800;
    
    return num_cartoes + 1;
}