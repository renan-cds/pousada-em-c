#include <stdio.h>
#include <string.h>
#include "reserva.h"
#include "hospede.h"
#include "quarto.h"
#include "hospedagem.h"
#include "arquivo.h"
#include "util.h"

long posicao_reserva_por_codigo(int codigo) {
    long total = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    long i;
    Reserva r;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r) &&
            r.codigo == codigo && r.ativo) {
            return i;
        }
    }
    return -1;
}

int buscar_reserva_por_codigo(int codigo, Reserva *destino) {
    long pos = posicao_reserva_por_codigo(codigo);
    if (pos < 0) {
        return 0;
    }
    return ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), pos, destino);
}

int salvar_reserva(Reserva r) {
    return acrescentar_registro(ARQ_RESERVAS, sizeof(Reserva), &r);
}

int atualizar_reserva(Reserva r) {
    long total = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    long i;
    Reserva tmp;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &tmp) &&
            tmp.codigo == r.codigo) {
            return escrever_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r);
        }
    }
    return 0;
}

/*
 * Periodos [inicio, fim) se sobrepoem quando:
 * inicio_a < fim_b && inicio_b < fim_a
 * O dia de checkout libera o quarto para nova entrada.
 */
static int periodos_sobrepoem(Data a_ini, Data a_fim, Data b_ini, Data b_fim) {
    return comparar_datas(a_ini, b_fim) < 0 && comparar_datas(b_ini, a_fim) < 0;
}

int quarto_disponivel_periodo(int numero_quarto, Data inicio, Data fim, int codigo_ignorar) {
    Quarto q;
    long total_r, total_h, i;
    Reserva r;
    Hospedagem hp;
    Data saida_efetiva;

    if (!buscar_quarto_por_numero(numero_quarto, &q) || !q.ativo) {
        return 0;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        return 0;
    }

    total_r = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    for (i = 0; i < total_r; i++) {
        if (!ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r)) {
            continue;
        }
        if (!r.ativo || r.status != RESERVA_ATIVA) {
            continue;
        }
        if (r.numero_quarto != numero_quarto) {
            continue;
        }
        if (codigo_ignorar > 0 && r.codigo == codigo_ignorar) {
            continue;
        }
        if (periodos_sobrepoem(inicio, fim, r.checkin, r.checkout)) {
            return 0;
        }
    }

    total_h = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    for (i = 0; i < total_h; i++) {
        if (!ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &hp)) {
            continue;
        }
        if (!hp.ativo || hp.numero_quarto != numero_quarto) {
            continue;
        }
        if (hp.finalizada) {
            saida_efetiva = hp.saida_real;
        } else {
            /* hospedagem em andamento: ocupa ate uma data futura distante */
            saida_efetiva.dia = 31;
            saida_efetiva.mes = 12;
            saida_efetiva.ano = 2099;
        }
        if (periodos_sobrepoem(inicio, fim, hp.entrada_real, saida_efetiva)) {
            return 0;
        }
    }

    return 1;
}

float calcular_valor_estimado(float valor_diaria, Data checkin, Data checkout) {
    int dias = dias_entre(checkin, checkout);
    if (dias <= 0) {
        return 0.0f;
    }
    return valor_diaria * (float)dias;
}

void imprimir_reserva(const Reserva *r) {
    Hospede h;
    printf("----------------------------------------\n");
    printf("Codigo reserva: %d\n", r->codigo);
    printf("Hospede: %d", r->codigo_hospede);
    if (buscar_hospede_por_codigo(r->codigo_hospede, &h)) {
        printf(" - %s", h.nome);
    }
    printf("\n");
    printf("Quarto: %d\n", r->numero_quarto);
    printf("Check-in: ");
    imprimir_data(r->checkin);
    printf("\nCheck-out: ");
    imprimir_data(r->checkout);
    printf("\nQtd hospedes: %d\n", r->qtd_hospedes);
    printf("Valor estimado: R$ %.2f\n", r->valor_estimado);
    printf("Status: %s\n", nome_status_reserva(r->status));
}

void verificar_disponibilidade_menu(void) {
    int numero;
    Data inicio, fim;
    Quarto q;

    printf("\n=== VERIFICAR DISPONIBILIDADE ===\n");
    numero = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(numero, &q)) {
        printf("Quarto nao encontrado.\n");
        return;
    }
    imprimir_quarto(&q);
    inicio = ler_data("Data inicio");
    fim = ler_data("Data fim");
    if (comparar_datas(inicio, fim) >= 0) {
        printf("A data fim deve ser posterior a data inicio.\n");
        return;
    }

    if (quarto_disponivel_periodo(numero, inicio, fim, 0)) {
        printf("\nQuarto %d DISPONIVEL no periodo.\n", numero);
        printf("Valor estimado: R$ %.2f (%d diaria(s))\n",
               calcular_valor_estimado(q.valor_diaria, inicio, fim),
               dias_entre(inicio, fim));
    } else {
        if (q.status == QUARTO_MANUTENCAO) {
            printf("\nQuarto %d esta em MANUTENCAO.\n", numero);
        } else {
            printf("\nQuarto %d INDISPONIVEL no periodo informado.\n", numero);
        }
    }
}

void incluir_reserva(void) {
    Reserva r;
    Hospede h;
    Quarto q;

    printf("\n=== INCLUIR RESERVA ===\n");
    memset(&r, 0, sizeof(Reserva));

    r.codigo_hospede = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(r.codigo_hospede, &h)) {
        printf("Hospede nao encontrado.\n");
        return;
    }
    printf("Hospede: %s\n", h.nome);

    r.numero_quarto = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(r.numero_quarto, &q)) {
        printf("Quarto nao encontrado.\n");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        printf("Quarto em manutencao. Nao e possivel reservar.\n");
        return;
    }
    printf("Quarto %d - %s - R$ %.2f/diaria - Capacidade: %d\n",
           q.numero, nome_tipo_quarto(q.tipo), q.valor_diaria, q.capacidade);

    r.checkin = ler_data("Data de check-in");
    r.checkout = ler_data("Data de check-out");
    if (comparar_datas(r.checkin, r.checkout) >= 0) {
        printf("Check-out deve ser posterior ao check-in.\n");
        return;
    }

    r.qtd_hospedes = ler_inteiro("Quantidade de hospedes: ");
    if (r.qtd_hospedes <= 0 || r.qtd_hospedes > q.capacidade) {
        printf("Quantidade invalida. Capacidade do quarto: %d.\n", q.capacidade);
        return;
    }

    if (!quarto_disponivel_periodo(r.numero_quarto, r.checkin, r.checkout, 0)) {
        printf("Erro: quarto ja ocupado no periodo solicitado.\n");
        return;
    }

    r.valor_estimado = calcular_valor_estimado(q.valor_diaria, r.checkin, r.checkout);
    r.status = RESERVA_ATIVA;
    r.ativo = 1;
    r.codigo = gerar_codigo_reserva();

    printf("\nResumo:\n");
    printf("Periodo: ");
    imprimir_data(r.checkin);
    printf(" a ");
    imprimir_data(r.checkout);
    printf(" (%d diaria(s))\n", dias_entre(r.checkin, r.checkout));
    printf("Valor estimado: R$ %.2f\n", r.valor_estimado);

    if (!confirmar("Confirmar reserva? (s/n): ")) {
        printf("Reserva nao confirmada.\n");
        return;
    }

    if (salvar_reserva(r)) {
        printf("Reserva cadastrada com sucesso! Codigo: %d\n", r.codigo);
    } else {
        printf("Erro ao salvar reserva.\n");
    }
}

void alterar_reserva(void) {
    int codigo;
    Reserva r;
    Quarto q;

    printf("\n=== ALTERAR RESERVA ===\n");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        printf("Reserva nao encontrada.\n");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Somente reservas ATIVAS podem ser alteradas.\n");
        return;
    }

    imprimir_reserva(&r);
    printf("\nNovos dados:\n");

    r.numero_quarto = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(r.numero_quarto, &q)) {
        printf("Quarto nao encontrado.\n");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        printf("Quarto em manutencao.\n");
        return;
    }

    r.checkin = ler_data("Data de check-in");
    r.checkout = ler_data("Data de check-out");
    if (comparar_datas(r.checkin, r.checkout) >= 0) {
        printf("Datas invalidas.\n");
        return;
    }

    r.qtd_hospedes = ler_inteiro("Quantidade de hospedes: ");
    if (r.qtd_hospedes <= 0 || r.qtd_hospedes > q.capacidade) {
        printf("Quantidade invalida para a capacidade do quarto.\n");
        return;
    }

    if (!quarto_disponivel_periodo(r.numero_quarto, r.checkin, r.checkout, r.codigo)) {
        printf("Erro: quarto indisponivel no novo periodo.\n");
        return;
    }

    r.valor_estimado = calcular_valor_estimado(q.valor_diaria, r.checkin, r.checkout);

    if (atualizar_reserva(r)) {
        printf("Reserva atualizada. Novo valor estimado: R$ %.2f\n", r.valor_estimado);
    } else {
        printf("Erro ao atualizar reserva.\n");
    }
}

void cancelar_reserva(void) {
    int codigo;
    Reserva r;

    printf("\n=== CANCELAR RESERVA ===\n");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        printf("Reserva nao encontrada.\n");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Reserva nao esta ativa (status: %s).\n", nome_status_reserva(r.status));
        return;
    }

    imprimir_reserva(&r);
    if (!confirmar("Confirma cancelamento? (s/n): ")) {
        printf("Operacao cancelada.\n");
        return;
    }

    r.status = RESERVA_CANCELADA;
    if (atualizar_reserva(r)) {
        printf("Reserva cancelada. Quarto liberado no periodo.\n");
    } else {
        printf("Erro ao cancelar reserva.\n");
    }
}

void excluir_reserva(void) {
    int codigo;
    Reserva r;

    printf("\n=== EXCLUIR RESERVA ===\n");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        printf("Reserva nao encontrada.\n");
        return;
    }
    if (r.status == RESERVA_ATIVA) {
        printf("Cancele a reserva antes de excluir, ou finalize via check-out.\n");
        return;
    }
    if (!confirmar("Confirma exclusao definitiva do registro? (s/n): ")) {
        printf("Operacao cancelada.\n");
        return;
    }

    r.ativo = 0;
    if (atualizar_reserva(r)) {
        printf("Registro de reserva excluido.\n");
    } else {
        printf("Erro ao excluir reserva.\n");
    }
}

void listar_reservas(void) {
    long total = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    long i;
    Reserva r;
    int encontrados = 0;

    printf("\n=== LISTA DE RESERVAS ===\n");
    if (total == 0) {
        printf("Nenhuma reserva cadastrada.\n");
        return;
    }

    printf("%-6s %-8s %-8s %-12s %-12s %-10s %-12s\n",
           "Cod", "Hosp.", "Quarto", "Check-in", "Check-out", "Valor", "Status");
    printf("----------------------------------------------------------------------------\n");
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r) && r.ativo) {
            printf("%-6d %-8d %-8d %02d/%02d/%04d  %02d/%02d/%04d  R$ %-7.2f %-12s\n",
                   r.codigo, r.codigo_hospede, r.numero_quarto,
                   r.checkin.dia, r.checkin.mes, r.checkin.ano,
                   r.checkout.dia, r.checkout.mes, r.checkout.ano,
                   r.valor_estimado, nome_status_reserva(r.status));
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhuma reserva ativa no arquivo.\n");
    } else {
        printf("\nTotal: %d reserva(s).\n", encontrados);
    }
}

void buscar_reserva(void) {
    int codigo;
    Reserva r;

    printf("\n=== BUSCAR RESERVA ===\n");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (buscar_reserva_por_codigo(codigo, &r)) {
        imprimir_reserva(&r);
    } else {
        printf("Reserva nao encontrada.\n");
    }
}

void menu_reservas(void) {
    int opcao;
    do {
        limpar_tela();
        printf("========================================\n");
        printf("       GERENCIAR RESERVAS\n");
        printf("========================================\n");
        printf("1. Incluir reserva\n");
        printf("2. Alterar reserva\n");
        printf("3. Cancelar reserva\n");
        printf("4. Excluir reserva\n");
        printf("5. Listar todas\n");
        printf("6. Buscar por codigo\n");
        printf("7. Verificar disponibilidade\n");
        printf("0. Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
            case 1: incluir_reserva(); break;
            case 2: alterar_reserva(); break;
            case 3: cancelar_reserva(); break;
            case 4: excluir_reserva(); break;
            case 5: listar_reservas(); break;
            case 6: buscar_reserva(); break;
            case 7: verificar_disponibilidade_menu(); break;
            case 0: break;
            default: printf("Opcao invalida.\n"); break;
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}
