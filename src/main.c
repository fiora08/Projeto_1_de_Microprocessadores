
/*
 * =============================================================================
 *  MicPay - main.c  (versão completa)
 *  Microprocessadores e Microcontroladores - Projeto 1 - 2026A
 * =============================================================================
 *
 *  FLUXO GERAL (conforme PDFs):
 *
 *  1. ENERGIA
 *     - Segurar 'D' por 3 s  → liga  (backlight ON, lcd_inicializar, estado=BLOQUEADO)
 *     - Segurar 'B' por 4 s  → desliga (backlight OFF, lcd_limpar, estado=BLOQUEADO)
 *
 *  2. LOGIN  (sistema_ja_ligado controla as etapas)
 *     0 → mostra "MicPay 2026"
 *     1 → aguarda 1,5 s → mostra "Bloqueado:" e pede senha (4 dígitos, exibe *)
 *     2 → autentica senha
 *         OK  → mostra "bem vindo: <nome>"  →  sistema_ja_ligado=3
 *         ERRO→ mostra "SENHA INVALIDA"     →  volta para 1
 *     3 → aguarda 1,5 s → envia login via serial  →  sistema_ja_ligado=4
 *
 *  3. OPERAÇÃO (sistema_ja_ligado==4, estado==DESBLOQUEADO)
 *     ├─ Administrador (usuario_autenticado==2):
 *     │    menu_admin() gerencia opções de configuração
 *     │
 *     └─ Operador (usuario_autenticado==0 ou 1) e habilitado:
 *          Menu principal (tecla '1'=Venda Vista, '2'=Venda Parcelada,
 *                          '3'=Estorno, '4'=Verificar Parcelas)
 *          ├─ Venda:
 *          │    vendas() coleta dados (não-bloqueante)
 *          │    Ao retornar 1 → envia ao servidor via enviar_venda() ou
 *          │                     enviar_venda_parcelada()
 *          │                   → aguarda resposta via maquina_protocolo()
 *          │                   → exibe resultado no LCD
 *          │
 *          ├─ Estorno:
 *          │    estorno_executar() — gerencia o fluxo completo
 *          │
 *          └─ Verificar Parcelas (manual pelo admin/operador):
 *               verificar_parcelas() — tenta nos horários 12h, 18h, 22h
 *
 *  4. MONITORAMENTO CONTÍNUO
 *     - maquina_protocolo() lida com mensagens assíncronas do servidor
 *       (SM=cartão magnético, SW=cartão wireless, SO=desconexão, SX=falha)
 *     - LED vermelho (PB2 / LED_fora_do_ar): acende após 2 min sem comunicação
 *     - LED amarelo (LED_pagamento_pendente): acende se parcela não confirmada
 *     - verificar_parcelas() chamada automaticamente nos horários 12h/18h/22h
 *
 * =============================================================================
 *  PROTOCOLO SERIAL (Taxa 19200, 8, E, 1)
 *
 *  Enviado pelo microcontrolador:
 *    Login:           'M' 'L' <operador>          (operador: '0','1','9'=admin)
 *    Logoff:          'M' 'O' <operador>
 *    Venda à vista:   'M' 'V' n <bandeira><cartao6><senha6><valor>\0
 *    Venda parcelada: 'M' 'P' n <bandeira><cartao6><senha6><parcelas><valor>\0
 *    Estorno:         'M' 'E' n <bandeira><cartao6><valor>\0
 *    Agendamento:     'M' 'A' n <bandeira><cartao6><valor>\0
 *    Confirm. cartão magnético:  'M' 'M'
 *    Confirm. cartão wireless:   'M' 'W'
 *
 *  Recebido do servidor (processado por maquina_protocolo()):
 *    Login OK:        'S' 'L' n <nome>\0
 *    Logoff:          'S' 'O'  (ok)  /  'S' 'X' (falha)
 *    Venda à vista:   'S' 'V' 'V'(ok) / 'C'(cartão) / 'S'(senha) / 'I'(saldo)
 *    Venda parcelada: 'S' 'P' <mesmo sufixo>
 *    Estorno:         'S' 'E' 'V'(ok) / 'C'(falha)
 *    Agendamento:     'S' 'A' 'P'(ok) / 'C'(falha) / 'N'(não localizado)
 *    Cartão magnético: 'S' 'M' n <bandeira><cartao6>
 *    Cartão wireless:  'S' 'W' n <bandeira><cartao6><senha6>
 * =============================================================================
 */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/iom2560.h>
#include <util/delay.h>
#include <string.h>

#include "maquina.h"          /* estado_atual, BLOQUEADO, DESBLOQUEADO, ERRO */
#include "timers.h"           /* timer0_init, timer1_init, flag_1ms, h, min, seg,
                                 mostrar_hora, atraso_ms,
                                 tempo_n_bloqueante_v2, contadores_tempo */
#include "serial.h"           /* serial_inicializar, serial_disponivel, serial_ler,
                                 serial_transmitir, fora_do_ar_flag */
#include "lcd.h"              /* lcd_inicializar, lcd_limpar, lcd_posicionar,
                                 lcd_escrever_string, lcd_caractere */
#include "teclado.h"          /* teclado_inicializar, teclado_atualizar,
                                 teclado_obter_tecla */
#include "energia.h"          /* energia_inicializar, energia_gerenciar,
                                 energia_sistema_ativo, sistema_ja_ligado,
                                 LED_fora_do_ar, LED_pagamento_pendente */
#include "interface.h"        /* (dependências internas) */
#include "senhas.h"           /* mascara_autentica_senha */
#include "vendas.h"           /* vendas(), n_vendas[], venda struct,
                                 VENDA_VISTA, VENDA_PARCELADA */
#include "maquina_protocolo.h"/* maquina_protocolo(), protocolo_get_mensagem() */
#include "protocolo.h"        /* enviar_login, enviar_logoff,
                                 enviar_venda, enviar_venda_parcelada,
                                 enviar_estorno, enviar_agendamento */
#include "login.h"            /* maquina_login() */
#include "admin.h"            /* menu_admin() */
#include "estorno.h"          /* estorno_executar() */
#include "venda_agendada.h"   /* verificar_parcelas() */


/* =========================================================================
 *  MENU PRINCIPAL DO OPERADOR
 *  Exibido após login bem-sucedido de operador 0 ou 1.
 *  Teclas:
 *    '1' → Venda à vista
 *    '2' → Venda parcelada
 *    '3' → Estorno
 *    '4' → (reservado para pendências / futuro)
 * ========================================================================= */
#define TELA_MENU        0   /* exibindo menu principal                        */
#define TELA_VENDA       1   /* executando fluxo de venda (não-bloqueante)     */
#define TELA_RESULTADO   2   /* exibindo resultado de venda por 2 s            */
#define TELA_ESTORNO     3   /* fluxo de estorno (bloqueante interno)          */

/* =========================================================================
 *  HELPERS INTERNOS
 * ========================================================================= */

/*
 * Aguarda até maquina_protocolo() montar uma mensagem completa ou
 * o servidor ficar sem resposta (flag_2s via serial_enviar_triplo).
 * Retorna o ponteiro para a mensagem montada.
 */
static char *aguardar_resposta_protocolo(void) {
    while (1) {
        if (maquina_protocolo() == 1) {
            return protocolo_get_mensagem();
        }
        /* Segurança: se o flag de erro de comunicação foi setado
           pela serial_enviar_triplo, saímos para não travar. */
        if (fora_do_ar_flag == 1) {
            return NULL; /* Sem resposta do servidor */
        }
    }
}

/*
 * Exibe no LCD o resultado de uma venda/parcelada com base na mensagem
 * recebida do servidor.  msg[0]='S', msg[1]='V'/'P', msg[2]=status
 */
static void exibir_resultado_venda(char *msg) {
    lcd_limpar();
    if (msg == NULL) {
        lcd_escrever_string("FALHA COMUNICA.");
        return;
    }
    switch (msg[2]) {
        case 'V': lcd_escrever_string("APROVADO!");        break;
        case 'C': lcd_escrever_string("CARTAO INVALIDO"); break;
        case 'S': lcd_escrever_string("SENHA INVALIDA");  break;
        case 'I': lcd_escrever_string("SALDO INSUF.");    break;
        default:  lcd_escrever_string("RESP. INVALIDA");  break;
    }
}

/*
 * Envia a venda que acabou de ser coletada ao servidor e aguarda resposta.
 * O índice da venda finalizada é (ind_vds - 1) com wrap em 5.
 * Como ind_vds é static interno ao vendas.c, usamos um contador próprio
 * para rastrear qual slot acabou de ser preenchido.  A forma mais simples
 * é guardar o valor de ind_vds antes e depois — mas como ele é estático
 * interno não temos acesso direto.
 *
 * SOLUÇÃO ADOTADA: n_vendas[] é global (declarado em vendas.c como extern).
 * Após retorno 1 de vendas(), o slot recém-preenchido é aquele cujo
 * ind_vds acabou de ser incrementado, ou seja: o último slot com
 * senha_cartao[0] != 0 E que ainda não foi enviado.
 * Para simplificar e ser robusto, rastreamos com uma variável local
 * que acompanha ind_vds por dedução: começa em 0 e incrementa junto.
 */
static void enviar_e_mostrar_venda(unsigned char idx) {
    char *resp = NULL;
    unsigned char ack = 0;

    if (n_vendas[idx].tipo_venda == VENDA_VISTA) {
        ack = enviar_venda(
            n_vendas[idx].bandeira,
            (char *)n_vendas[idx].num_cartao,
            (char *)n_vendas[idx].senha_cartao,
            (char *)n_vendas[idx].valor_venda
        );
    } else { /* VENDA_PARCELADA */
        ack = enviar_venda_parcelada(
            n_vendas[idx].bandeira,
            (char *)n_vendas[idx].num_cartao,
            (char *)n_vendas[idx].senha_cartao,
            n_vendas[idx].num_parcelas,
            (char *)n_vendas[idx].valor_venda
        );
    }

    if (ack == 1) {
        resp = aguardar_resposta_protocolo();
    }

    exibir_resultado_venda(resp);
}

/*
 * Trata mensagens assíncronas do servidor (cartão magnético / wireless).
 * Deve ser chamada no loop principal sempre que maquina_protocolo() == 1
 * e NÃO estivermos no meio de um fluxo de venda.
 * Retorna 1 se exibiu algo no LCD (para que a main saiba que deve
 * redesenhar o menu depois do timeout).
 */
static unsigned char tratar_msg_assincrona(char *msg) {
    if (msg == NULL) return 0;

    /* Cartão magnético inserido: 'S' 'M' n <bandeira><cartao6> */
    if (msg[0] == 'S' && msg[1] == 'M') {
        lcd_limpar();
        lcd_escrever_string("CARTAO MAGN.");
        lcd_posicionar(1, 0);
        lcd_escrever_string("INSERIDO");
        /* Confirma ao servidor */
        serial_transmitir('M');
        serial_transmitir('M');
        return 1;
    }

    /* Cartão wireless aproximado: 'S' 'W' n <bandeira><cartao6><senha6> */
    if (msg[0] == 'S' && msg[1] == 'W') {
        lcd_limpar();
        lcd_escrever_string("CARTAO WIRELESS");
        lcd_posicionar(1, 0);
        lcd_escrever_string("APROXIMADO");
        /* Confirma ao servidor */
        serial_transmitir('M');
        serial_transmitir('W');
        return 1;
    }

    /* Desconexão confirmada: 'S' 'O' */
    if (msg[0] == 'S' && msg[1] == 'O') {
        /* Nada a exibir — apenas confirma internamente */
        return 0;
    }

    /* Falha de login: 'S' 'X' */
    if (msg[0] == 'S' && msg[1] == 'X') {
        lcd_limpar();
        lcd_escrever_string("FALHA LOGIN SRV");
        return 1;
    }

    return 0;
}


/* =========================================================================
 *  MAIN
 * ========================================================================= */
int main(void) {

    /* ------------------------------------------------------------------
     * 1. INICIALIZAÇÃO DOS PERIFÉRICOS
     * ------------------------------------------------------------------ */
    serial_inicializar(51);   /* 19200 baud com F_CPU=16 MHz: UBRR = 51      */
    timer0_init();            /* Timer 0: base 1 ms (flag_1ms, debounce)      */
    timer1_init();            /* Timer 1: 1 ms (contador_serial, relógio)     */
    teclado_inicializar();    /* PORTA: PA4-PA7 saídas, PA0-PA3 entradas      */
    lcd_inicializar();        /* PORTC (dados) + PORTB (RS/EN/BKL)            */
    energia_inicializar();    /* Estado inicial DESLIGADO, configura LEDs     */

    sei();                    /* Habilita interrupções globais                */

    /* ------------------------------------------------------------------
     * 2. USUÁRIOS (hardcoded conforme especificação)
     *    Índice 0 → Operador 0 (senha "1254")
     *    Índice 1 → Operador 1 (senha "2349")
     *    Índice 2 → Administrador (senha "0738")
     * ------------------------------------------------------------------ */
    usuario novo_usuario[3];

    strcpy(novo_usuario[0].nome,  "ZE ROBERTO");
    strcpy(novo_usuario[0].senha, "1254");
    novo_usuario[0].habilitado = 1;

    strcpy(novo_usuario[1].nome,  "HUMBERTO");
    strcpy(novo_usuario[1].senha, "2349");
    novo_usuario[1].habilitado = 1;

    strcpy(novo_usuario[2].nome,  "ADEMASTOR");
    strcpy(novo_usuario[2].senha, "0738");
    novo_usuario[2].habilitado = 1;

    /* ------------------------------------------------------------------
     * 3. VARIÁVEIS DE CONTROLE DO LOOP PRINCIPAL
     * ------------------------------------------------------------------ */
    unsigned char usuario_autenticado = 0xFF;  /* índice do usuário logado   */
    unsigned char tecla               = 0;
    unsigned char tela_operador       = TELA_MENU; /* estado do menu operador */
    unsigned char venda_inicio        = 1;          /* flag de início de venda */
    unsigned char ultimo_idx_venda    = 0;          /* índice da última venda  */
    unsigned char venda_idx_contador  = 0;          /* espelha ind_vds interno */
    unsigned char menu_desenhado      = 0;          /* evita redesenho desnec. */
    static int    ultimo_seg          = -1;

    /* ------------------------------------------------------------------
     * 4. LOOP PRINCIPAL
     * ------------------------------------------------------------------ */
    while (1) {

        /* ============================================================
         * A) GERENCIAMENTO DE ENERGIA
         *    Sempre chamado — detecta pressionamento longo de D (liga)
         *    e B (desliga), independentemente do estado atual.
         * ============================================================ */
        energia_gerenciar();

        /* ============================================================
         * B) MÁQUINA DE LOGIN
         *    Cuida de tudo até sistema_ja_ligado == 4:
         *      0 → splash screen "MicPay 2026"
         *      1 → aguarda 1,5 s e mostra "Bloqueado:"
         *      2 → coleta senha (mascara com *), autentica, envia login
         *      3 → aguarda 1,5 s e envia login serial
         *    Quando sistema_ja_ligado == 4 e estado_atual == DESBLOQUEADO
         *    o sistema está pronto para operar.
         * ============================================================ */
        maquina_login(novo_usuario, &usuario_autenticado);

        /* ============================================================
         * C) BLOCO DE OPERAÇÃO PRINCIPAL
         *    Só executado quando totalmente autenticado.
         * ============================================================ */
        if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 4) {

            /* Atualiza teclado e obtém tecla */
            teclado_atualizar();
            tecla = teclado_obter_tecla();

            /* ----------------------------------------------------------
             * C.1) LED FORA DO AR
             *      serial.c seta fora_do_ar_flag=1 após 2 min sem resp.
             * ---------------------------------------------------------- */
            if (fora_do_ar_flag == 1) {
                PORTB |=  (1 << LED_fora_do_ar);
            } else {
                PORTB &= ~(1 << LED_fora_do_ar);
            }

            /* ----------------------------------------------------------
             * C.2) VERIFICAÇÃO AUTOMÁTICA DE PARCELAS
             *      Tenta nos horários 12h, 18h e 22h (uma vez cada).
             *      verificar_parcelas() usa flags estáticas internas
             *      para garantir uma única tentativa por horário.
             *      O LED amarelo é aceso internamente se houver falha.
             * ---------------------------------------------------------- */
            verificar_parcelas();

            /* ----------------------------------------------------------
             * C.3) MENSAGENS ASSÍNCRONAS DO SERVIDOR
             *      O servidor pode mandar SM (cartão magnético) ou SW
             *      (cartão wireless) a qualquer momento.
             *      maquina_protocolo() é não-bloqueante — retorna 1
             *      apenas quando um frame completo foi recebido.
             *      Só processamos aqui se NÃO estivermos no meio de
             *      uma venda (evita conflito com respostas de venda).
             * ---------------------------------------------------------- */
            if (tela_operador == TELA_MENU) {
                if (maquina_protocolo() == 1) {
                    char *msg = protocolo_get_mensagem();
                    if (tratar_msg_assincrona(msg)) {
                        menu_desenhado = 0;    /* força redesenho do menu */
                        atraso_ms(1500);
                    }
                }
            }

            /* ----------------------------------------------------------
             * C.4) MENU ADMINISTRADOR
             *      usuario_autenticado == 2 → acessa menu exclusivo.
             *      menu_admin() é não-bloqueante: gerencia hora,
             *      habilitar/desabilitar operadores e pendências.
             * ---------------------------------------------------------- */
            if (usuario_autenticado == 2) {
                menu_admin(tecla, novo_usuario);

                /* Exibe relógio na linha 1 continuamente */
                if (seg != ultimo_seg) {
                    ultimo_seg = seg;
                    lcd_posicionar(1, 9);
                    mostrar_hora();
                }

            /* ----------------------------------------------------------
             * C.5) MENU OPERADOR
             *      Operadores 0 e 1 (se habilitados).
             * ---------------------------------------------------------- */
            } else {

                /* Verifica se o operador está habilitado pelo admin */
                if (!novo_usuario[usuario_autenticado].habilitado) {
                    lcd_limpar();
                    lcd_escrever_string("OP DESABILITADO");
                    lcd_posicionar(1, 0);
                    lcd_escrever_string("CONTATE ADMIN");
                    /* Aguarda logoff — nada mais pode ser feito */
                    flag_1ms = 0;
                    continue;
                }

                switch (tela_operador) {

                /* ------------------------------------------------------
                 * TELA_MENU: Exibe opções e aguarda escolha do operador
                 * ------------------------------------------------------ */
                case TELA_MENU:
                    if (!menu_desenhado) {
                        lcd_limpar();
                        lcd_posicionar(0, 0);
                        lcd_escrever_string("1-VVista 2-Parc");
                        lcd_posicionar(1, 0);
                        lcd_escrever_string("3-Estorno");
                        menu_desenhado = 1;
                        venda_inicio   = 1; /* próxima venda sempre começa limpa */
                    }

                    switch (tecla) {
                        case '1':
                            /* Venda à vista */
                            venda_inicio   = 1;
                            tela_operador  = TELA_VENDA;
                            menu_desenhado = 0;
                            lcd_limpar();
                            break;

                        case '2':
                            /* Venda parcelada (mesmo fluxo — tipo definido
                               pela tecla dentro de vendas()) */
                            venda_inicio   = 1;
                            tela_operador  = TELA_VENDA;
                            menu_desenhado = 0;
                            lcd_limpar();
                            break;

                        case '3':
                            /* Estorno */
                            tela_operador  = TELA_ESTORNO;
                            menu_desenhado = 0;
                            break;

                        default:
                            break;
                    }
                    break;

                /* ------------------------------------------------------
                 * TELA_VENDA: Coleta de dados de venda (não-bloqueante)
                 *
                 *  vendas(inicio) retorna:
                 *    0 → ainda coletando dados (chamamos de novo no próx. ciclo)
                 *    1 → venda concluída → enviar ao servidor
                 *
                 *  O índice da venda recém-preenchida em n_vendas[]:
                 *    ind_vds interno foi incrementado ANTES do return 1,
                 *    portanto o dado está em (venda_idx_contador % 5).
                 *    Após cada retorno 1 incrementamos venda_idx_contador.
                 * ------------------------------------------------------ */
                case TELA_VENDA:
                    if (vendas(venda_inicio)) {
                        /* Venda coletada com sucesso — enviar ao servidor */
                        ultimo_idx_venda = venda_idx_contador % 5;
                        venda_idx_contador++;
                        if (venda_idx_contador >= 5) venda_idx_contador = 0;

                        enviar_e_mostrar_venda(ultimo_idx_venda);

                        tela_operador  = TELA_RESULTADO;
                        menu_desenhado = 0;
                        /* Zera contador do timeout de exibição */
                        contadores_tempo[0] = 0;
                    }
                    venda_inicio = 0; /* Nas próximas iterações não redesenha */
                    break;

                /* ------------------------------------------------------
                 * TELA_RESULTADO: Mostra resultado por 2 s e volta ao menu
                 * ------------------------------------------------------ */
                case TELA_RESULTADO:
                    if (tempo_n_bloqueante_v2(2000, 0)) {
                        tela_operador  = TELA_MENU;
                        menu_desenhado = 0;
                    }
                    break;

                /* ------------------------------------------------------
                 * TELA_ESTORNO: Fluxo de estorno (bloqueante internamente)
                 *
                 *  estorno_executar() cuida de todo o diálogo:
                 *    1. Pede senha de operador
                 *    2. Valida com servidor
                 *    3. Pede código da venda
                 *    4. Pede número do cartão (digitado ou inserido)
                 *    5. Confirma valor
                 *    6. Envia enviar_estorno() e mostra resultado
                 *  Retorna 1 quando termina (sucesso ou falha).
                 * ------------------------------------------------------ */
                case TELA_ESTORNO:
                    estorno_executar(novo_usuario);
                    tela_operador  = TELA_MENU;
                    menu_desenhado = 0;
                    break;

                default:
                    tela_operador  = TELA_MENU;
                    menu_desenhado = 0;
                    break;
                } /* switch tela_operador */

            } /* else (operador) */

        } /* if DESBLOQUEADO && sistema_ja_ligado==4 */

        /* ============================================================
         * D) RESET DA FLAG DE 1 ms
         *    Deve ser feito no final de cada iteração para sincronizar
         *    corretamente timers e debounce no próximo ciclo.
         * ============================================================ */
        flag_1ms = 0;

    } /* while(1) */

    return 0;
}

/*#include <avr/io.h>
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


int main(void) {
	// Inicialização dos periféricos
	serial_inicializar(51); //-> formula para dar o 119200 com 16 mhz foi definido o 51 para a velocidade baud. 
	timer0_init();
	timer1_init();
	teclado_inicializar();
	lcd_inicializar();
	energia_inicializar();
	
	sei(); // Habilita interrup��es

	//inicialização dos usuários 
	usuario  novo_usuario[3];
	cartao_local1 cartao_local[5];

	strcpy(novo_usuario[0].nome ,"ZE ROBERTO");
	strcpy(novo_usuario[0].senha , "1254");
	novo_usuario[0].habilitado = 1;

	strcpy(novo_usuario[1].nome ,"HUMBERTO");
	strcpy(novo_usuario[1].senha , "2349");
	novo_usuario[1].habilitado = 1;

	strcpy(novo_usuario[2].nome ,"ADEMASTOR");
	strcpy(novo_usuario[2].senha , "0738");
	novo_usuario[2].habilitado = 1;

	strcpy(cartao_local[0].numero_cartao, "123456");
	cartao_local[0].saldo = 800;

	unsigned char tecla=0;
	char buffer_lcd[17]; // 16 caracteres + '\0'
	unsigned char index = 0;
	unsigned char usuario_autenticado;
	unsigned char modo_edicao = 0;
	static int ultimo_seg = -1;




while (1) {
    energia_gerenciar();
    maquina_login(novo_usuario, &usuario_autenticado);
    if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 4) {
        tecla = teclado_obter_tecla();
        if (maquina_protocolo() == 1) {
            lcd_limpar();
            lcd_escrever_string(protocolo_get_mensagem());
        }
        if (tecla != 0) lcd_caractere(tecla);
        if (fora_do_ar_flag == 1) PORTB |= (1<<LED_fora_do_ar);
        else PORTB &= ~(1<<LED_fora_do_ar);
    }
    flag_1ms = 0;
}
return 0;
}
*/

// MAIN BASICA E FUNCIONAL ATÉ O MOMENTO
//PODEMOS CRIAR AS NOVAS VERSÕES A PARTIR DELA

/*while (1) {
    energia_gerenciar();
    if (energia_sistema_ativo() == LIGADO) {
        teclado_atualizar();

		
		if (sistema_ja_ligado == 0) {
			lcd_limpar();
			lcd_posicionar(0, 2);
			lcd_escrever_string("MicPay 2026");
			lcd_posicionar(1, 1);
			lcd_escrever_string("Sistema Ativo");
			sistema_ja_ligado = 1;
		}
		if (sistema_ja_ligado == 1 && tempo_n_bloqueante(1500)) {
			lcd_limpar();
			lcd_posicionar(0, 0);
			lcd_escrever_string("Bloqueado:");
			sistema_ja_ligado = 2;
			estado_atual = BLOQUEADO;
		}
	
        if (estado_atual == BLOQUEADO && sistema_ja_ligado ==2  ) {
            mascara_autentica_senha(novo_usuario, 4, &usuario_autenticado);
			if (estado_atual == DESBLOQUEADO) {
				lcd_limpar();
        		lcd_posicionar(0, 0);
				lcd_escrever_string("bem vindo:");
				lcd_posicionar(1, 0);
       	 		lcd_escrever_string(novo_usuario[usuario_autenticado].nome);
				sistema_ja_ligado =3;
			}
        }
		if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 3 && tempo_n_bloqueante(1500)) {
   			 lcd_limpar();
   			 sistema_ja_ligado = 4;
}
        if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 4) {
            tecla = teclado_obter_tecla();
            if (tecla != 0) {
                lcd_caractere(tecla);
            }
        }
    }
	flag_1ms=0;
}


return 0;
}*/


/*
//main para testar a função de vendas
// força estado desbloqueado para testar sem autenticação
    estado_atual = DESBLOQUEADO;

    unsigned char inicio = 1;

    while (1) {
        teclado_atualizar();

        if (vendas(inicio)) {
            inicio = 1; // reseta para nova venda
            lcd_limpar();
            lcd_posicionar(0, 0);
            lcd_escrever_string("Venda OK!");
        } else {
            inicio = 0; // não redesenha tela a cada ciclo
        }

        flag_1ms = 0;
    }
    return 0;
}
*/

/* 
usuario_autenticado = 0;
	
	//enviar_login(usuario_autenticado);
	//enviar_logoff(usuario_autenticado);

LOOP P/ TESTAR A MAQUINA DE ESTADOS DE MONTAGEM DE FRAMES RECEBIDOS
while(1){
	energia_gerenciar();
	if (maquina_protocolo()==1) {
		serial_escrever(protocolo_get_mensagem());
		lcd_escrever_string(protocolo_get_mensagem());
		

	
	}
	flag_1ms = 0;
}
return 0;
}
*/

/*
while(1){
    if(serial_disponivel()){
        char c = serial_ler();
        serial_transmitir(c); // ecoa de volta o que recebeu
    }
}
return 0;
}*/


//LOOP PARA TESTAR FUNÇÃO DE SETAR HORA
//AO LIGAR ELA COMEÇA A CONTAR AS HORAS DE 00:00:00
// O LOOP APÓS LIGAR COM d POR 4S E PRESSIONAR "A"
// A FUNÇÃO DE ALTERAR HORA -> ESPERA UM NUMERO PARA HORA E CONFIRMA COM #
//DAÍ VAI PRA ALTERAR MINUTO -> ESPERA UM NUMERO E CONFIRMA COM #
// EM A,BAS CASO PRESSIONE UM NUMERO DE DUAS CASAS ELE CONFIRMA AUTOMATICO
//SE PRESSIONAR UMA HORA OU MINUTO NAO VALIDO TIPO 90, ELA RESETA O LOOP
/*	
while (1) {
    energia_gerenciar();
    if (energia_sistema_ativo() == LIGADO) {
        teclado_atualizar();
        tecla = teclado_obter_tecla();

        if (tecla == 'A') {
            modo_edicao = 1;
            tecla = 0;
        }

        if (modo_edicao == 1) {
    if (alterar_hora(tecla)) {
        modo_edicao = 2;
        tecla = 0; // zera aqui
    }
}
if (modo_edicao == 2 && tecla != '#') { // só processa minuto se não for '#'
    if (alterar_minuto(tecla)) modo_edicao = 0;
}

        if (seg != ultimo_seg) {
            ultimo_seg = seg;
            lcd_posicionar(0, 0);
            mostrar_hora();
        }
    }
    flag_1ms = 0;
}
    return 0;
}
*/

/*
//MAIN TESTADA E FUNCIONAL DIA 14/05 às 2:14

while (1) {
    energia_gerenciar();
    if (energia_sistema_ativo() == LIGADO) {
        teclado_atualizar();

		
		if (sistema_ja_ligado == 0) {
			lcd_limpar();
			lcd_posicionar(0, 2);
			lcd_escrever_string("MicPay 2026");
			lcd_posicionar(1, 1);
			lcd_escrever_string("Sistema Ativo");
			sistema_ja_ligado = 1;
		}
		if (sistema_ja_ligado == 1 && tempo_n_bloqueante(1500)) {
			lcd_limpar();
			lcd_posicionar(0, 0);
			lcd_escrever_string("Bloqueado:");
			sistema_ja_ligado = 2;
			estado_atual = BLOQUEADO;
		}
	
        if (estado_atual == BLOQUEADO && sistema_ja_ligado ==2  ) {
            mascara_autentica_senha(novo_usuario, 4, &usuario_autenticado);
			if (estado_atual == DESBLOQUEADO) {
				lcd_limpar();
        		lcd_posicionar(0, 0);
				lcd_escrever_string("bem vindo:");
				lcd_posicionar(1, 0);
       	 		lcd_escrever_string(novo_usuario[usuario_autenticado].nome);
				sistema_ja_ligado =3;
			}
		if (estado_atual == ERRO) {
			lcd_limpar();
			lcd_escrever_string("SENHA INVALIDA");
			estado_atual = BLOQUEADO;
			sistema_ja_ligado = 1;
    }
        }

		if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 3 && tempo_n_bloqueante(1500)) {
   			 lcd_limpar();
			 enviar_login(usuario_autenticado);
   			 sistema_ja_ligado = 4;
}
// até esse bloco aqui ele só liga mostra a telinha inicial e autentica usuário
//as flags sistema ja ligado são só pra fazer um passo a passo
        if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 4) {
			
            tecla = teclado_obter_tecla();
			if(maquina_protocolo()== 1){
				char *msg = protocolo_get_mensagem();
				lcd_limpar();
				lcd_escrever_string(msg);
			}
			//a partir daqui da para adicionar coisas novas para testar!!
            if (tecla != 0) {
                lcd_caractere(tecla);// aqui lcd esta mostrando a tecla que foi lida
				//vendas(1);
            }

			if (fora_do_ar_flag ==1) {
				PORTB |= (1<<LED_fora_do_ar);
			
			}else {
				PORTB &= ~(1<< LED_fora_do_ar);
			}
        }
    }
	flag_1ms=0;
}
return 0;
}



*/


// MAIN DEIXADA COMO REF, MAS ELA ESTAVA COM PROBLEMAS DE DELAY E NAO RECEBER TECLA DO TECLADO
/*
while (1) {
    energia_gerenciar();
    if (energia_sistema_ativo() == LIGADO) {
        teclado_atualizar();

		
		if (sistema_ja_ligado == 0) {
			lcd_limpar();
			lcd_posicionar(0, 2);
			lcd_escrever_string("MicPay 2026");
			lcd_posicionar(1, 1);
			lcd_escrever_string("Sistema Ativo");
			sistema_ja_ligado = 1;
		}
		if (sistema_ja_ligado == 1 && tempo_n_bloqueante(1500)) {
			lcd_limpar();
			lcd_posicionar(0, 0);
			lcd_escrever_string("Bloqueado:");
			sistema_ja_ligado = 2;
			estado_atual = BLOQUEADO;
		}
	
        if (estado_atual == BLOQUEADO && sistema_ja_ligado ==2  ) {
            mascara_autentica_senha(novo_usuario, 4, &usuario_autenticado);
			if (estado_atual == DESBLOQUEADO) {
				lcd_limpar();
        		lcd_posicionar(0, 0);
				lcd_escrever_string("bem vindo:");
				lcd_posicionar(1, 0);
       	 		lcd_escrever_string(novo_usuario[usuario_autenticado].nome);
				sistema_ja_ligado =3;
			}
        }
		if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 3 && tempo_n_bloqueante(1500)) {
   			 lcd_limpar();
   			 sistema_ja_ligado = 4;
}
        if (estado_atual == DESBLOQUEADO && sistema_ja_ligado == 4) {
            tecla = teclado_obter_tecla();
            if (usuario_autenticado == 2) {
        // menu admin
				switch(tecla) {
					case '1': break;// habilitar/desabilitar operador0
					case '2': break;// habilitar/desabilitar operador1
					case '3': 
						modo_edicao = 1;
						break;
					case '4': break; // pagamentos pendentes
					case '5': break;// pendências
				}
				
				if (seg != ultimo_seg) {
					ultimo_seg = seg;
					lcd_posicionar(0, 0);
					lcd_escrever_string("Admin:");
					mostrar_hora();
				}
				tecla=0;
				if (modo_edicao == 1) {
  			if (alterar_hora(tecla)) modo_edicao = 2;
				}
			if (modo_edicao == 2) {
  				 if (alterar_minuto(tecla)) modo_edicao = 0;
}
			} else {
				
				
				if (flag_1ms) {
					lcd_posicionar(1, 0);
					lcd_escrever_string("hora");
					mostrar_hora();
				}
				
			}

		
        }
    }
	flag_1ms=0;
}


return 0;


}*/

