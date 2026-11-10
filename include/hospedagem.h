#ifndef HOSPEDAGEM_H
#define HOSPEDAGEM_H

#include "tipos.h"

void menu_hospedagens(void);
void checkin_por_reserva(void);
void checkin_direto(void);
void checkout(void);
void listar_hospedagens(void);
void buscar_hospedagem(void);

int salvar_hospedagem(Hospedagem h);
int atualizar_hospedagem(Hospedagem h);
int buscar_hospedagem_por_codigo(int codigo, Hospedagem *destino);
long posicao_hospedagem_por_codigo(int codigo);
int hospede_esta_hospedado(int codigo_hospede);
int quarto_tem_hospedagem_ativa(int numero_quarto);
void imprimir_hospedagem(const Hospedagem *h);
FormaPagamento ler_forma_pagamento(void);

#endif
