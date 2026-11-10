#include <stdio.h>
#include <string.h>
#include "hospedagem.h"
#include "reserva.h"
#include "hospede.h"
#include "quarto.h"
#include "arquivo.h"
#include "util.h"

long posicao_hospedagem_por_codigo(int codigo) {
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem h;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h) &&
            h.codigo == codigo && h.ativo) {
            return i;
        }
    }
    return -1;
}

int buscar_hospedagem_por_codigo(int codigo, Hospedagem *destino) {
    long pos = posicao_hospedagem_por_codigo(codigo);
    if (pos < 0) {
        return 0;
    }
    return ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), pos, destino);
}

int salvar_hospedagem(Hospedagem h) {
    return acrescentar_registro(ARQ_HOSPEDAGENS, sizeof(Hospedagem), &h);
}

int atualizar_hospedagem(Hospedagem h) {
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem tmp;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &tmp) &&
            tmp.codigo == h.codigo) {
            return escrever_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h);
        }
    }
    return 0;
}

int hospede_esta_hospedado(int codigo_hospede) {
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem h;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h) &&
            h.ativo && !h.finalizada && h.codigo_hospede == codigo_hospede) {
            return 1;
        }
    }
    return 0;
}

int quarto_tem_hospedagem_ativa(int numero_quarto) {
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem h;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h) &&
            h.ativo && !h.finalizada && h.numero_quarto == numero_quarto) {
            return 1;
        }
    }
    return 0;
}

FormaPagamento ler_forma_pagamento(void) {
    int opcao;
    while (1) {
        printf("Forma de pagamento:\n");
        printf("  1. Dinheiro\n");
        printf("  2. Cartao Credito\n");
        printf("  3. Cartao Debito\n");
        printf("  4. PIX\n");
        printf("  5. Transferencia\n");
        opcao = ler_inteiro("Escolha: ");
        if (opcao >= 1 && opcao <= 5) {
            return (FormaPagamento)opcao;
        }
        printf("Opcao invalida.\n");
    }
}

void imprimir_hospedagem(const Hospedagem *h) {
    Hospede hosp;
    printf("----------------------------------------\n");
    printf("Codigo hospedagem: %d\n", h->codigo);
    if (h->codigo_reserva > 0) {
        printf("Reserva origem: %d\n", h->codigo_reserva);
    } else {
        printf("Reserva origem: (check-in direto)\n");
    }
    printf("Hospede: %d", h->codigo_hospede);
    if (buscar_hospede_por_codigo(h->codigo_hospede, &hosp)) {
        printf(" - %s", hosp.nome);
    }
    printf("\n");
    printf("Quarto: %d\n", h->numero_quarto);
    printf("Entrada: ");
    imprimir_data(h->entrada_real);
    printf("\nSaida: ");
    if (h->finalizada) {
        imprimir_data(h->saida_real);
    } else {
        printf("(em andamento)");
    }
    printf("\nValor final: R$ %.2f\n", h->valor_final);
    if (h->finalizada) {
        printf("Pagamento: %s\n", nome_forma_pagamento(h->forma_pagamento));
    }
    if (h->observacoes[0] != '\0') {
        printf("Obs: %s\n", h->observacoes);
    }
    printf("Situacao: %s\n", h->finalizada ? "Finalizada" : "Em andamento");
}

void checkin_por_reserva(void) {
    int codigo_reserva;
    Reserva r;
    Quarto q;
    Hospedagem h;
    Data entrada;

    printf("\n=== CHECK-IN POR RESERVA ===\n");
    codigo_reserva = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo_reserva, &r)) {
        printf("Reserva nao encontrada.\n");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Reserva nao esta ativa (status: %s).\n", nome_status_reserva(r.status));
        return;
    }

    imprimir_reserva(&r);

    if (hospede_esta_hospedado(r.codigo_hospede)) {
        printf("Hospede ja possui hospedagem em andamento.\n");
        return;
    }
    if (quarto_tem_hospedagem_ativa(r.numero_quarto)) {
        printf("Quarto ja possui hospede em check-in.\n");
        return;
    }
    if (!buscar_quarto_por_numero(r.numero_quarto, &q)) {
        printf("Quarto da reserva nao encontrado.\n");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        printf("Quarto em manutencao.\n");
        return;
    }

    entrada = ler_data("Data real de entrada");
    /* valida disponibilidade considerando a saida prevista da reserva */
    if (!quarto_disponivel_periodo(r.numero_quarto, entrada, r.checkout, r.codigo)) {
        printf("Conflito de disponibilidade no periodo.\n");
        return;
    }

    memset(&h, 0, sizeof(Hospedagem));
    h.codigo = gerar_codigo_hospedagem();
    h.codigo_reserva = r.codigo;
    h.codigo_hospede = r.codigo_hospede;
    h.numero_quarto = r.numero_quarto;
    h.entrada_real = entrada;
    h.saida_real.dia = 0;
    h.saida_real.mes = 0;
    h.saida_real.ano = 0;
    h.valor_final = r.valor_estimado;
    h.forma_pagamento = PAG_DINHEIRO;
    ler_string(h.observacoes, MAX_OBS, "Observacoes (opcional): ");
    h.finalizada = 0;
    h.ativo = 1;

    if (!salvar_hospedagem(h)) {
        printf("Erro ao registrar hospedagem.\n");
        return;
    }

    /* marca reserva como finalizada (convertida em hospedagem) */
    r.status = RESERVA_FINALIZADA;
    atualizar_reserva(r);

    printf("Check-in realizado! Codigo da hospedagem: %d\n", h.codigo);
    printf("Quarto %d ocupado.\n", h.numero_quarto);
}

void checkin_direto(void) {
    Hospedagem h;
    Hospede hosp;
    Quarto q;
    Data saida_prevista;

    printf("\n=== CHECK-IN DIRETO (SEM RESERVA) ===\n");
    memset(&h, 0, sizeof(Hospedagem));

    h.codigo_hospede = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(h.codigo_hospede, &hosp)) {
        printf("Hospede nao encontrado.\n");
        return;
    }
    if (hospede_esta_hospedado(h.codigo_hospede)) {
        printf("Hospede ja esta hospedado.\n");
        return;
    }

    h.numero_quarto = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(h.numero_quarto, &q)) {
        printf("Quarto nao encontrado.\n");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        printf("Quarto em manutencao.\n");
        return;
    }
    if (quarto_tem_hospedagem_ativa(h.numero_quarto)) {
        printf("Quarto ja ocupado.\n");
        return;
    }

    h.entrada_real = ler_data("Data de entrada");
    saida_prevista = ler_data("Data prevista de saida");
    if (comparar_datas(h.entrada_real, saida_prevista) >= 0) {
        printf("Data de saida deve ser posterior a entrada.\n");
        return;
    }

    if (!quarto_disponivel_periodo(h.numero_quarto, h.entrada_real, saida_prevista, 0)) {
        printf("Quarto indisponivel no periodo (ha reserva ou ocupacao).\n");
        return;
    }

    h.codigo = gerar_codigo_hospedagem();
    h.codigo_reserva = 0;
    h.saida_real.dia = 0;
    h.saida_real.mes = 0;
    h.saida_real.ano = 0;
    h.valor_final = calcular_valor_estimado(q.valor_diaria, h.entrada_real, saida_prevista);
    h.forma_pagamento = PAG_DINHEIRO;
    ler_string(h.observacoes, MAX_OBS, "Observacoes (opcional): ");
    h.finalizada = 0;
    h.ativo = 1;

    printf("Valor estimado: R$ %.2f\n", h.valor_final);
    if (!confirmar("Confirmar check-in? (s/n): ")) {
        printf("Operacao cancelada.\n");
        return;
    }

    if (salvar_hospedagem(h)) {
        printf("Check-in realizado! Codigo: %d\n", h.codigo);
    } else {
        printf("Erro ao salvar hospedagem.\n");
    }
}

void checkout(void) {
    int codigo;
    Hospedagem h;
    Quarto q;
    int dias;
    float valor_calc;

    printf("\n=== CHECK-OUT ===\n");
    codigo = ler_inteiro("Codigo da hospedagem: ");
    if (!buscar_hospedagem_por_codigo(codigo, &h)) {
        printf("Hospedagem nao encontrada.\n");
        return;
    }
    if (h.finalizada) {
        printf("Hospedagem ja foi finalizada.\n");
        return;
    }

    imprimir_hospedagem(&h);

    h.saida_real = ler_data("Data real de saida");
    if (comparar_datas(h.entrada_real, h.saida_real) >= 0) {
        printf("Data de saida deve ser posterior a entrada.\n");
        return;
    }

    if (!buscar_quarto_por_numero(h.numero_quarto, &q)) {
        printf("Quarto nao encontrado. Usando valor registrado.\n");
        valor_calc = h.valor_final;
    } else {
        dias = dias_entre(h.entrada_real, h.saida_real);
        if (dias <= 0) {
            dias = 1;
        }
        valor_calc = q.valor_diaria * (float)dias;
    }

    printf("Diarias: %d\n", dias_entre(h.entrada_real, h.saida_real));
    printf("Valor calculado: R$ %.2f\n", valor_calc);
    if (confirmar("Ajustar valor manualmente? (s/n): ")) {
        h.valor_final = ler_float("Valor total final (R$): ");
    } else {
        h.valor_final = valor_calc;
    }

    h.forma_pagamento = ler_forma_pagamento();
    ler_string(h.observacoes, MAX_OBS, "Observacoes finais (opcional): ");
    h.finalizada = 1;

    if (atualizar_hospedagem(h)) {
        printf("Check-out realizado!\n");
        printf("Quarto %d liberado.\n", h.numero_quarto);
        printf("Valor total: R$ %.2f | Pagamento: %s\n",
               h.valor_final, nome_forma_pagamento(h.forma_pagamento));
    } else {
        printf("Erro ao atualizar hospedagem.\n");
    }
}

void listar_hospedagens(void) {
    long total = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Hospedagem h;
    int encontrados = 0;

    printf("\n=== LISTA DE HOSPEDAGENS ===\n");
    if (total == 0) {
        printf("Nenhuma hospedagem registrada.\n");
        return;
    }

    printf("%-6s %-8s %-8s %-12s %-12s %-10s %-12s\n",
           "Cod", "Hosp.", "Quarto", "Entrada", "Saida", "Valor", "Situacao");
    printf("----------------------------------------------------------------------------\n");
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &h) && h.ativo) {
            printf("%-6d %-8d %-8d %02d/%02d/%04d  ",
                   h.codigo, h.codigo_hospede, h.numero_quarto,
                   h.entrada_real.dia, h.entrada_real.mes, h.entrada_real.ano);
            if (h.finalizada) {
                printf("%02d/%02d/%04d  ", h.saida_real.dia, h.saida_real.mes, h.saida_real.ano);
            } else {
                printf("%-12s ", "Em curso");
            }
            printf("R$ %-7.2f %-12s\n",
                   h.valor_final,
                   h.finalizada ? "Finalizada" : "Andamento");
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhuma hospedagem ativa no arquivo.\n");
    } else {
        printf("\nTotal: %d hospedagem(ns).\n", encontrados);
    }
}

void buscar_hospedagem(void) {
    int codigo;
    Hospedagem h;

    printf("\n=== BUSCAR HOSPEDAGEM ===\n");
    codigo = ler_inteiro("Codigo da hospedagem: ");
    if (buscar_hospedagem_por_codigo(codigo, &h)) {
        imprimir_hospedagem(&h);
    } else {
        printf("Hospedagem nao encontrada.\n");
    }
}

void menu_hospedagens(void) {
    int opcao;
    do {
        limpar_tela();
        printf("========================================\n");
        printf("      CHECK-IN / CHECK-OUT\n");
        printf("========================================\n");
        printf("1. Check-in a partir de reserva\n");
        printf("2. Check-in direto\n");
        printf("3. Check-out\n");
        printf("4. Listar hospedagens\n");
        printf("5. Buscar hospedagem\n");
        printf("0. Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
            case 1: checkin_por_reserva(); break;
            case 2: checkin_direto(); break;
            case 3: checkout(); break;
            case 4: listar_hospedagens(); break;
            case 5: buscar_hospedagem(); break;
            case 0: break;
            default: printf("Opcao invalida.\n"); break;
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}
