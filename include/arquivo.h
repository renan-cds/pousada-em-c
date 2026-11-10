#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include "tipos.h"

FILE *abrir_arquivo(const char *caminho, const char *modo);
long contar_registros(const char *caminho, size_t tamanho_registro);
int ler_registro_pos(const char *caminho, size_t tamanho, long pos, void *destino);
int escrever_registro_pos(const char *caminho, size_t tamanho, long pos, const void *origem);
int acrescentar_registro(const char *caminho, size_t tamanho, const void *origem);

Contadores carregar_contadores(void);
void salvar_contadores(Contadores c);
int gerar_codigo_hospede(void);
int gerar_codigo_reserva(void);
int gerar_codigo_hospedagem(void);

#endif
