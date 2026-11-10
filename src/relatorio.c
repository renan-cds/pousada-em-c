#include <stdio.h>
#include <string.h>
#include "relatorio.h"
#include "tipos.h"
#include "arquivo.h"
#include "util.h"
#include "hospede.h"
#include "quarto.h"
#include "reserva.h"
#include "hospedagem.h"

void relatorio_reservas_periodo(void) {
    Data inicio, fim;
    long total = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    long i;
    Reserva r;
    int encontrados = 0;

    printf("\n=== RESERVAS POR PERIODO ===\n");
    inicio = ler_data("Inicio do periodo");
    fim = ler_data("Fim do periodo");
    if (comparar_datas(inicio, fim) > 0) {
        printf("Periodo invalido.\n");
        return;
    }

    printf("\n%-6s %-8s %-8s %-12s %-12s %-12s\n",
           "Cod", "Hosp.", "Quarto", "Check-in", "Check-out", "Status");
    printf("------------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (!ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r) || !r.ativo) {
            continue;
        }
        /* reserva cujo check-in esta dentro do periodo solicitado */
        if (comparar_datas(r.checkin, inicio) >= 0 && comparar_datas(r.checkin, fim) <= 0) {
            printf("%-6d %-8d %-8d %02d/%02d/%04d  %02d/%02d/%04d  %-12s\n",
                   r.codigo, r.codigo_hospede, r.numero_quarto,
                   r.checkin.dia, r.checkin.mes, r.checkin.ano,
                   r.checkout.dia, r.checkout.mes, r.checkout.ano,
                   nome_status_reserva(r.status));
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhuma reserva encontrada no periodo.\n");
    } else {
        printf("\nTotal: %d reserva(s).\n", encontrados);
    }
}

void relatorio_hospedes_atuais(void) {
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem h;
    Hospede hosp;
    int encontrados = 0;

    printf("\n=== HOSPEDES ATUALMENTE HOSPEDADOS ===\n");
    printf("%-6s %-30s %-8s %-12s\n", "Cod.H", "Nome", "Quarto", "Entrada");
    printf("--------------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (!ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h)) {
            continue;
        }
        if (!h.ativo || h.finalizada) {
            continue;
        }
        if (!buscar_hospede_por_codigo(h.codigo_hospede, &hosp)) {
            strncpy(hosp.nome, "(desconhecido)", MAX_NOME - 1);
        }
        printf("%-6d %-30.30s %-8d %02d/%02d/%04d\n",
               h.codigo_hospede, hosp.nome, h.numero_quarto,
               h.entrada_real.dia, h.entrada_real.mes, h.entrada_real.ano);
        encontrados++;
    }

    if (encontrados == 0) {
        printf("Nenhum hospede hospedado no momento.\n");
    } else {
        printf("\nTotal: %d hospede(s) hospedado(s).\n", encontrados);
    }
}

void relatorio_quartos_disponiveis_data(void) {
    Data data;
    Data dia_seguinte;
    long total = contar_registros(ARQ_QUARTOS, sizeof(Quarto));
    long i;
    Quarto q;
    int encontrados = 0;
    int dias_mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    printf("\n=== QUARTOS DISPONIVEIS EM UMA DATA ===\n");
    data = ler_data("Data");

    /* disponibilidade para pernoite na data: [data, data+1) */
    dia_seguinte = data;
    if ((dia_seguinte.ano % 4 == 0 && dia_seguinte.ano % 100 != 0) ||
        (dia_seguinte.ano % 400 == 0)) {
        dias_mes[2] = 29;
    }
    dia_seguinte.dia++;
    if (dia_seguinte.dia > dias_mes[dia_seguinte.mes]) {
        dia_seguinte.dia = 1;
        dia_seguinte.mes++;
        if (dia_seguinte.mes > 12) {
            dia_seguinte.mes = 1;
            dia_seguinte.ano++;
        }
    }

    printf("\n%-8s %-12s %-12s %-10s\n", "Numero", "Tipo", "Diaria", "Capacidade");
    printf("--------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (!ler_registro_pos(ARQ_QUARTOS, sizeof(Quarto), i, &q) || !q.ativo) {
            continue;
        }
        if (q.status == QUARTO_MANUTENCAO) {
            continue;
        }
        if (quarto_disponivel_periodo(q.numero, data, dia_seguinte, 0)) {
            printf("%-8d %-12s R$ %-8.2f %-10d\n",
                   q.numero, nome_tipo_quarto(q.tipo), q.valor_diaria, q.capacidade);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum quarto disponivel nesta data.\n");
    } else {
        printf("\nTotal: %d quarto(s) disponivel(is).\n", encontrados);
    }
}

void relatorio_faturamento_periodo(void) {
    Data inicio, fim;
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem h;
    float total_fat = 0.0f;
    int contagem = 0;

    printf("\n=== FATURAMENTO POR PERIODO ===\n");
    printf("(Considera hospedagens com check-out no periodo)\n");
    inicio = ler_data("Inicio do periodo");
    fim = ler_data("Fim do periodo");
    if (comparar_datas(inicio, fim) > 0) {
        printf("Periodo invalido.\n");
        return;
    }

    printf("\n%-6s %-8s %-8s %-12s %-10s %-16s\n",
           "Cod", "Hosp.", "Quarto", "Saida", "Valor", "Pagamento");
    printf("--------------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (!ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h)) {
            continue;
        }
        if (!h.ativo || !h.finalizada) {
            continue;
        }
        if (comparar_datas(h.saida_real, inicio) >= 0 &&
            comparar_datas(h.saida_real, fim) <= 0) {
            printf("%-6d %-8d %-8d %02d/%02d/%04d  R$ %-7.2f %-16s\n",
                   h.codigo, h.codigo_hospede, h.numero_quarto,
                   h.saida_real.dia, h.saida_real.mes, h.saida_real.ano,
                   h.valor_final, nome_forma_pagamento(h.forma_pagamento));
            total_fat += h.valor_final;
            contagem++;
        }
    }

    if (contagem == 0) {
        printf("Nenhuma hospedagem finalizada no periodo.\n");
    } else {
        printf("\nHospedagens: %d\n", contagem);
        printf("Faturamento total: R$ %.2f\n", total_fat);
    }
}

void relatorio_historico_hospede(void) {
    int codigo;
    Hospede hosp;
    long total_r, total_h, i;
    Reserva r;
    Hospedagem h;
    int achou_r = 0, achou_h = 0;

    printf("\n=== HISTORICO DE HOSPEDAGENS DO HOSPEDE ===\n");
    codigo = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(codigo, &hosp)) {
        printf("Hospede nao encontrado.\n");
        return;
    }

    imprimir_hospede(&hosp);

    printf("\n-- Reservas --\n");
    total_r = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    for (i = 0; i < total_r; i++) {
        if (ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r) &&
            r.ativo && r.codigo_hospede == codigo) {
            printf("Reserva %d | Quarto %d | ", r.codigo, r.numero_quarto);
            imprimir_data(r.checkin);
            printf(" a ");
            imprimir_data(r.checkout);
            printf(" | %s | R$ %.2f\n",
                   nome_status_reserva(r.status), r.valor_estimado);
            achou_r = 1;
        }
    }
    if (!achou_r) {
        printf("Nenhuma reserva.\n");
    }

    printf("\n-- Hospedagens --\n");
    total_h = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    for (i = 0; i < total_h; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h) &&
            h.ativo && h.codigo_hospede == codigo) {
            printf("Hosp. %d | Quarto %d | Entrada ", h.codigo, h.numero_quarto);
            imprimir_data(h.entrada_real);
            printf(" | Saida ");
            if (h.finalizada) {
                imprimir_data(h.saida_real);
            } else {
                printf("(em andamento)");
            }
            printf(" | R$ %.2f\n", h.valor_final);
            achou_h = 1;
        }
    }
    if (!achou_h) {
        printf("Nenhuma hospedagem.\n");
    }
}

void menu_relatorios(void) {
    int opcao;
    do {
        limpar_tela();
        printf("========================================\n");
        printf("     CONSULTAS E RELATORIOS\n");
        printf("========================================\n");
        printf("1. Reservas por periodo\n");
        printf("2. Hospedes atualmente hospedados\n");
        printf("3. Quartos disponiveis em uma data\n");
        printf("4. Faturamento por periodo\n");
        printf("5. Historico de um hospede\n");
        printf("0. Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
            case 1: relatorio_reservas_periodo(); break;
            case 2: relatorio_hospedes_atuais(); break;
            case 3: relatorio_quartos_disponiveis_data(); break;
            case 4: relatorio_faturamento_periodo(); break;
            case 5: relatorio_historico_hospede(); break;
            case 0: break;
            default: printf("Opcao invalida.\n"); break;
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}
