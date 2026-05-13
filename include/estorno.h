/*
 * estorno.h
 *
 * Created: 23/04/2026 08:53:26
 *  Author: User
 */ 

#ifndef ESTORNO_H
#define ESTORNO_H

#include "senhas.h" // Obrigatório para o compilador reconhecer o que é a struct 'usuario'

// Inicia a máquina de estados do processo de estorno.
// Recebe o vetor de usuários para validar a senha do operador.
// Retorna 1 se o estorno foi aprovado com sucesso, ou 0 se foi cancelado/reprovado.
unsigned char estorno_executar(usuario *lista_usuarios);

#endif