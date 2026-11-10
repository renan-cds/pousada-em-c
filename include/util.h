#ifndef UTIL_H
#define UTIL_H

#include "tipos.h"

void limpar_tela(void);
void pausar(void);
void limpar_buffer(void);
void ler_string(char *destino, int tamanho, const char *prompt);
int ler_inteiro(const char *prompt);
float ler_float(const char *prompt);
int confirmar(const char *prompt);

Data ler_data(const char *prompt);
int data_valida(Data d);
int data_vazia(Data d);
int comparar_datas(Data a, Data b);
int dias_entre(Data inicio, Data fim);
void imprimir_data(Data d);
int data_para_juliano(Data d);

int cpf_valido_formato(const char *cpf);
void normalizar_cpf(char *cpf);

const char *nome_tipo_quarto(TipoQuarto tipo);
const char *nome_status_quarto(StatusQuarto status);
const char *nome_status_reserva(StatusReserva status);
const char *nome_forma_pagamento(FormaPagamento forma);

void garantir_diretorio_dados(void);

#endif
