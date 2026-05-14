
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
#include "protocolo.h"

void maquina_login(usuario *novo_usuario, unsigned char *usuario_autenticado) {
    if (energia_sistema_ativo() != LIGADO) return;
    
    teclado_atualizar();

    if (sistema_ja_ligado == 0) {
        lcd_limpar();
        lcd_posicionar(0, 2);
        lcd_escrever_string("MicPay 2026");
        lcd_posicionar(1, 1);
        lcd_escrever_string("Sistema Ativo");
        sistema_ja_ligado = 1;
    }
    if (sistema_ja_ligado == 1 && tempo_n_bloqueante_v2(1500, 0)) {
        lcd_limpar();
        lcd_posicionar(0, 0);
        lcd_escrever_string("Bloqueado:");
        sistema_ja_ligado = 2;
        estado_atual = BLOQUEADO;
    }
    if (estado_atual == BLOQUEADO && sistema_ja_ligado == 2) {
        mascara_autentica_senha(novo_usuario, 4, usuario_autenticado);
        if (estado_atual == DESBLOQUEADO) {
            lcd_limpar();
            lcd_posicionar(0, 0);
            lcd_escrever_string("bem vindo:");
            lcd_posicionar(1, 0);
            lcd_escrever_string(novo_usuario[*usuario_autenticado].nome);
            sistema_ja_ligado = 3;
        }
        if (estado_atual == ERRO) {
            lcd_limpar();
            lcd_escrever_string("SENHA INVALIDA");
            estado_atual = BLOQUEADO;
            sistema_ja_ligado = 1;
        }
    }
    if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 3 && tempo_n_bloqueante_v2(1500, 1)) {
        lcd_limpar();
        enviar_login(*usuario_autenticado);
        sistema_ja_ligado = 4;
    }
}