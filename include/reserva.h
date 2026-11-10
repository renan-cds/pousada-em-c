#ifndef RESERVA_H
#define RESERVA_H

#include "tipos.h"

void menu_reservas(void);
void incluir_reserva(void);
void alterar_reserva(void);
void cancelar_reserva(void);
void excluir_reserva(void);
void listar_reservas(void);
void buscar_reserva(void);
void verificar_disponibilidade_menu(void);

int salvar_reserva(Reserva r);
int atualizar_reserva(Reserva r);
int buscar_reserva_por_codigo(int codigo, Reserva *destino);
long posicao_reserva_por_codigo(int codigo);

/*
 * Retorna 1 se o quarto esta livre no periodo [inicio, fim).
 * O dia de checkout e considerado liberado para nova reserva.
 * codigo_ignorar: codigo de reserva a ignorar (na alteracao), ou 0.
 */
int quarto_disponivel_periodo(int numero_quarto, Data inicio, Data fim, int codigo_ignorar);
float calcular_valor_estimado(float valor_diaria, Data checkin, Data checkout);
void imprimir_reserva(const Reserva *r);

#endif
