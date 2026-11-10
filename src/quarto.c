#include <stdio.h>
#include <string.h>
#include "quarto.h"
#include "arquivo.h"
#include "util.h"
#include "tipos.h"

long posicao_quarto_por_numero(int numero) {
    long total = contar_registros(ARQ_QUARTOS, sizeof(Quarto));
    long i;
    Quarto q;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_QUARTOS, sizeof(Quarto), i, &q) &&
            q.numero == numero && q.ativo) {
            return i;
        }
    }
    return -1;
}

int buscar_quarto_por_numero(int numero, Quarto *destino) {
    long pos = posicao_quarto_por_numero(numero);
    if (pos < 0) {
        return 0;
    }
    return ler_registro_pos(ARQ_QUARTOS, sizeof(Quarto), pos, destino);
}

int salvar_quarto(Quarto q) {
    return acrescentar_registro(ARQ_QUARTOS, sizeof(Quarto), &q);
}

int atualizar_quarto(Quarto q) {
    long total = contar_registros(ARQ_QUARTOS, sizeof(Quarto));
    long i;
    Quarto tmp;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_QUARTOS, sizeof(Quarto), i, &tmp) &&
            tmp.numero == q.numero) {
            return escrever_registro_pos(ARQ_QUARTOS, sizeof(Quarto), i, &q);
        }
    }
    return 0;
}

int quarto_em_uso(int numero) {
    long total_r = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    long total_h = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Reserva r;
    Hospedagem hp;

    for (i = 0; i < total_r; i++) {
        if (ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r) &&
            r.ativo && r.numero_quarto == numero && r.status == RESERVA_ATIVA) {
            return 1;
        }
    }
    for (i = 0; i < total_h; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &hp) &&
            hp.ativo && hp.numero_quarto == numero && !hp.finalizada) {
            return 1;
        }
    }
    return 0;
}

TipoQuarto ler_tipo_quarto(void) {
    int opcao;
    while (1) {
        printf("Tipo do quarto:\n");
        printf("  1. Solteiro\n");
        printf("  2. Casal\n");
        printf("  3. Luxo\n");
        printf("  4. Familia\n");
        opcao = ler_inteiro("Escolha: ");
        if (opcao >= 1 && opcao <= 4) {
            return (TipoQuarto)opcao;
        }
        printf("Opcao invalida.\n");
    }
}

StatusQuarto ler_status_quarto(void) {
    int opcao;
    while (1) {
        printf("Status do quarto:\n");
        printf("  1. Disponivel\n");
        printf("  2. Manutencao\n");
        opcao = ler_inteiro("Escolha: ");
        if (opcao == 1 || opcao == 2) {
            return (StatusQuarto)opcao;
        }
        printf("Opcao invalida.\n");
    }
}

void imprimir_quarto(const Quarto *q) {
    printf("----------------------------------------\n");
    printf("Numero: %d\n", q->numero);
    printf("Tipo: %s\n", nome_tipo_quarto(q->tipo));
    printf("Valor da diaria: R$ %.2f\n", q->valor_diaria);
    printf("Capacidade: %d pessoa(s)\n", q->capacidade);
    printf("Status: %s\n", nome_status_quarto(q->status));
}

void incluir_quarto(void) {
    Quarto q;
    Quarto existente;

    printf("\n=== INCLUIR QUARTO ===\n");
    memset(&q, 0, sizeof(Quarto));
    q.numero = ler_inteiro("Numero do quarto: ");
    if (q.numero <= 0) {
        printf("Numero invalido.\n");
        return;
    }
    if (buscar_quarto_por_numero(q.numero, &existente)) {
        printf("Erro: quarto %d ja cadastrado.\n", q.numero);
        return;
    }

    q.tipo = ler_tipo_quarto();
    q.valor_diaria = ler_float("Valor da diaria (R$): ");
    if (q.valor_diaria <= 0) {
        printf("Valor da diaria deve ser positivo.\n");
        return;
    }
    q.capacidade = ler_inteiro("Capacidade (pessoas): ");
    if (q.capacidade <= 0) {
        printf("Capacidade invalida.\n");
        return;
    }
    q.status = ler_status_quarto();
    q.ativo = 1;

    if (salvar_quarto(q)) {
        printf("Quarto %d cadastrado com sucesso!\n", q.numero);
    } else {
        printf("Erro ao salvar quarto.\n");
    }
}

void alterar_quarto(void) {
    int numero;
    Quarto q;

    printf("\n=== ALTERAR QUARTO ===\n");
    numero = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(numero, &q)) {
        printf("Quarto nao encontrado.\n");
        return;
    }

    imprimir_quarto(&q);
    printf("\nInforme os novos dados:\n");
    q.tipo = ler_tipo_quarto();
    q.valor_diaria = ler_float("Valor da diaria (R$): ");
    if (q.valor_diaria <= 0) {
        printf("Valor invalido.\n");
        return;
    }
    q.capacidade = ler_inteiro("Capacidade (pessoas): ");
    if (q.capacidade <= 0) {
        printf("Capacidade invalida.\n");
        return;
    }
    q.status = ler_status_quarto();
    q.ativo = 1;

    if (atualizar_quarto(q)) {
        printf("Quarto atualizado com sucesso.\n");
    } else {
        printf("Erro ao atualizar quarto.\n");
    }
}

void excluir_quarto(void) {
    int numero;
    Quarto q;

    printf("\n=== EXCLUIR QUARTO ===\n");
    numero = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(numero, &q)) {
        printf("Quarto nao encontrado.\n");
        return;
    }

    imprimir_quarto(&q);
    if (quarto_em_uso(numero)) {
        printf("Nao e possivel excluir: quarto possui reserva ativa ou hospedagem em andamento.\n");
        return;
    }
    if (!confirmar("Confirma exclusao? (s/n): ")) {
        printf("Operacao cancelada.\n");
        return;
    }

    q.ativo = 0;
    if (atualizar_quarto(q)) {
        printf("Quarto excluido com sucesso.\n");
    } else {
        printf("Erro ao excluir quarto.\n");
    }
}

void listar_quartos(void) {
    long total = contar_registros(ARQ_QUARTOS, sizeof(Quarto));
    long i;
    Quarto q;
    int encontrados = 0;

    printf("\n=== LISTA DE QUARTOS ===\n");
    if (total == 0) {
        printf("Nenhum quarto cadastrado.\n");
        return;
    }

    printf("%-8s %-12s %-12s %-10s %-12s\n", "Numero", "Tipo", "Diaria", "Capac.", "Status");
    printf("--------------------------------------------------------------\n");
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_QUARTOS, sizeof(Quarto), i, &q) && q.ativo) {
            printf("%-8d %-12s R$ %-8.2f %-10d %-12s\n",
                   q.numero,
                   nome_tipo_quarto(q.tipo),
                   q.valor_diaria,
                   q.capacidade,
                   nome_status_quarto(q.status));
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum quarto ativo.\n");
    } else {
        printf("\nTotal: %d quarto(s).\n", encontrados);
    }
}

void buscar_quarto(void) {
    int numero;
    Quarto q;

    printf("\n=== BUSCAR QUARTO ===\n");
    numero = ler_inteiro("Numero do quarto: ");
    if (buscar_quarto_por_numero(numero, &q)) {
        imprimir_quarto(&q);
    } else {
        printf("Quarto nao encontrado.\n");
    }
}

void menu_quartos(void) {
    int opcao;
    do {
        limpar_tela();
        printf("========================================\n");
        printf("        CADASTRO DE QUARTOS\n");
        printf("========================================\n");
        printf("1. Incluir\n");
        printf("2. Alterar\n");
        printf("3. Excluir\n");
        printf("4. Listar todos\n");
        printf("5. Buscar por numero\n");
        printf("0. Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
            case 1: incluir_quarto(); break;
            case 2: alterar_quarto(); break;
            case 3: excluir_quarto(); break;
            case 4: listar_quartos(); break;
            case 5: buscar_quarto(); break;
            case 0: break;
            default: printf("Opcao invalida.\n"); break;
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}
