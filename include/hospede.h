#ifndef HOSPEDE_H
#define HOSPEDE_H

#include "tipos.h"

void menu_hospedes(void);
void incluir_hospede(void);
void alterar_hospede(void);
void excluir_hospede(void);
void listar_hospedes(void);
void buscar_hospede(void);

int salvar_hospede(Hospede h);
int atualizar_hospede(Hospede h);
int buscar_hospede_por_codigo(int codigo, Hospede *destino);
int buscar_hospede_por_cpf(const char *cpf, Hospede *destino);
long posicao_hospede_por_codigo(int codigo);
int hospede_em_uso(int codigo);
void imprimir_hospede(const Hospede *h);

#endif
