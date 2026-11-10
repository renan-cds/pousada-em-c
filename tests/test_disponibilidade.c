/*
 * Teste automatizado da logica de disponibilidade e calculo de diarias.
 * Compilar: make test
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tipos.h"
#include "util.h"
#include "arquivo.h"
#include "hospede.h"
#include "quarto.h"
#include "reserva.h"
#include "hospedagem.h"

static int falhas = 0;

static void ok(const char *nome, int cond) {
    if (cond) {
        printf("[OK] %s\n", nome);
    } else {
        printf("[FALHA] %s\n", nome);
        falhas++;
    }
}

static Data d(int dia, int mes, int ano) {
    Data x;
    x.dia = dia;
    x.mes = mes;
    x.ano = ano;
    return x;
}

static void limpar_dados(void) {
    remove(ARQ_HOSPEDES);
    remove(ARQ_QUARTOS);
    remove(ARQ_RESERVAS);
    remove(ARQ_HOSPEDAGENS);
    remove(ARQ_CONTADORES);
}

int main(void) {
    Hospede h;
    Quarto q;
    Reserva r1, r2;
    Hospedagem hp;

    garantir_diretorio_dados();
    limpar_dados();

    printf("=== Testes do Sistema de Pousada ===\n\n");

    /* Datas */
    ok("dias_entre 2 diarias", dias_entre(d(10, 11, 2026), d(12, 11, 2026)) == 2);
    ok("comparar_datas ordem", comparar_datas(d(10, 11, 2026), d(12, 11, 2026)) < 0);
    ok("data_valida 29/02/2024", data_valida(d(29, 2, 2024)));
    ok("data_invalida 29/02/2025", !data_valida(d(29, 2, 2025)));
    ok("cpf formato valido", cpf_valido_formato("52998224725"));
    ok("cpf sequencia invalida", !cpf_valido_formato("11111111111"));

    /* Cadastros base */
    memset(&h, 0, sizeof(h));
    h.codigo = gerar_codigo_hospede();
    strcpy(h.nome, "Maria Silva");
    strcpy(h.cpf, "52998224725");
    strcpy(h.telefone, "11999990001");
    strcpy(h.email, "maria@email.com");
    h.ativo = 1;
    ok("salvar hospede", salvar_hospede(h));

    memset(&q, 0, sizeof(q));
    q.numero = 101;
    q.tipo = TIPO_SOLTEIRO;
    q.valor_diaria = 150.0f;
    q.capacidade = 1;
    q.status = QUARTO_DISPONIVEL;
    q.ativo = 1;
    ok("salvar quarto", salvar_quarto(q));

    ok("quarto livre sem reservas",
       quarto_disponivel_periodo(101, d(10, 11, 2026), d(12, 11, 2026), 0));

    /* Reserva ativa */
    memset(&r1, 0, sizeof(r1));
    r1.codigo = gerar_codigo_reserva();
    r1.codigo_hospede = h.codigo;
    r1.numero_quarto = 101;
    r1.checkin = d(10, 11, 2026);
    r1.checkout = d(12, 11, 2026);
    r1.qtd_hospedes = 1;
    r1.valor_estimado = calcular_valor_estimado(150.0f, r1.checkin, r1.checkout);
    r1.status = RESERVA_ATIVA;
    r1.ativo = 1;
    ok("salvar reserva", salvar_reserva(r1));
    ok("valor estimado 2*150", r1.valor_estimado > 299.0f && r1.valor_estimado < 301.0f);

    ok("bloqueia sobreposicao total",
       !quarto_disponivel_periodo(101, d(10, 11, 2026), d(12, 11, 2026), 0));
    ok("bloqueia sobreposicao parcial",
       !quarto_disponivel_periodo(101, d(11, 11, 2026), d(13, 11, 2026), 0));
    ok("libera no dia do checkout",
       quarto_disponivel_periodo(101, d(12, 11, 2026), d(14, 11, 2026), 0));
    ok("livre antes da reserva",
       quarto_disponivel_periodo(101, d(8, 11, 2026), d(10, 11, 2026), 0));
    ok("ignora propria reserva na alteracao",
       quarto_disponivel_periodo(101, d(10, 11, 2026), d(12, 11, 2026), r1.codigo));

    /* Cancelamento libera */
    r1.status = RESERVA_CANCELADA;
    ok("atualizar cancelamento", atualizar_reserva(r1));
    ok("livre apos cancelamento",
       quarto_disponivel_periodo(101, d(10, 11, 2026), d(12, 11, 2026), 0));

    /* Nova reserva + check-in ocupa quarto */
    r1.status = RESERVA_ATIVA;
    r1.codigo = gerar_codigo_reserva();
    ok("nova reserva", salvar_reserva(r1));

    memset(&hp, 0, sizeof(hp));
    hp.codigo = gerar_codigo_hospedagem();
    hp.codigo_reserva = r1.codigo;
    hp.codigo_hospede = h.codigo;
    hp.numero_quarto = 101;
    hp.entrada_real = d(10, 11, 2026);
    hp.valor_final = r1.valor_estimado;
    hp.finalizada = 0;
    hp.ativo = 1;
    ok("salvar hospedagem ativa", salvar_hospedagem(hp));

    r1.status = RESERVA_FINALIZADA;
    atualizar_reserva(r1);

    ok("hospedagem ativa bloqueia periodo futuro",
       !quarto_disponivel_periodo(101, d(15, 11, 2026), d(17, 11, 2026), 0));

    /* Checkout libera */
    hp.saida_real = d(12, 11, 2026);
    hp.finalizada = 1;
    hp.forma_pagamento = PAG_PIX;
    ok("checkout", atualizar_hospedagem(hp));
    ok("livre apos checkout",
       quarto_disponivel_periodo(101, d(12, 11, 2026), d(14, 11, 2026), 0));

    /* Quarto manutencao */
    q.status = QUARTO_MANUTENCAO;
    atualizar_quarto(q);
    ok("manutencao indisponivel",
       !quarto_disponivel_periodo(101, d(20, 11, 2026), d(22, 11, 2026), 0));

    /* Contadores / busca */
    ok("buscar hospede por cpf", buscar_hospede_por_cpf("52998224725", &h));
    ok("buscar quarto", buscar_quarto_por_numero(101, &q));

    memset(&r2, 0, sizeof(r2));
    (void)r2;

    printf("\n=== Resultado: %d falha(s) ===\n", falhas);
    limpar_dados();
    return falhas == 0 ? 0 : 1;
}
