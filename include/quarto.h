#ifndef QUARTO_H
#define QUARTO_H

#include "tipos.h"

void menu_quartos(void);
void incluir_quarto(void);
void alterar_quarto(void);
void excluir_quarto(void);
void listar_quartos(void);
void buscar_quarto(void);

int salvar_quarto(Quarto q);
int atualizar_quarto(Quarto q);
int buscar_quarto_por_numero(int numero, Quarto *destino);
long posicao_quarto_por_numero(int numero);
int quarto_em_uso(int numero);
void imprimir_quarto(const Quarto *q);
TipoQuarto ler_tipo_quarto(void);
StatusQuarto ler_status_quarto(void);

#endif
