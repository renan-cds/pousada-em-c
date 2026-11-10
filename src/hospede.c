#include <stdio.h>
#include <string.h>
#include "hospede.h"
#include "arquivo.h"
#include "util.h"
#include "reserva.h"
#include "hospedagem.h"

long posicao_hospede_por_codigo(int codigo) {
    long total = contar_registros(ARQ_HOSPEDES, sizeof(Hospede));
    long i;
    Hospede h;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), i, &h) &&
            h.codigo == codigo && h.ativo) {
            return i;
        }
    }
    return -1;
}

int buscar_hospede_por_codigo(int codigo, Hospede *destino) {
    long pos = posicao_hospede_por_codigo(codigo);
    if (pos < 0) {
        return 0;
    }
    return ler_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), pos, destino);
}

int buscar_hospede_por_cpf(const char *cpf, Hospede *destino) {
    long total = contar_registros(ARQ_HOSPEDES, sizeof(Hospede));
    long i;
    Hospede h;
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), i, &h) &&
            h.ativo && strcmp(h.cpf, cpf) == 0) {
            if (destino != NULL) {
                *destino = h;
            }
            return 1;
        }
    }
    return 0;
}

int salvar_hospede(Hospede h) {
    return acrescentar_registro(ARQ_HOSPEDES, sizeof(Hospede), &h);
}

int atualizar_hospede(Hospede h) {
    long pos = posicao_hospede_por_codigo(h.codigo);
    if (pos < 0) {
        /* pode estar inativo: procura sem filtro ativo para reativar update */
        long total = contar_registros(ARQ_HOSPEDES, sizeof(Hospede));
        long i;
        Hospede tmp;
        for (i = 0; i < total; i++) {
            if (ler_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), i, &tmp) &&
                tmp.codigo == h.codigo) {
                return escrever_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), i, &h);
            }
        }
        return 0;
    }
    return escrever_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), pos, &h);
}

int hospede_em_uso(int codigo) {
    long total_r = contar_registros(ARQ_RESERVAS, sizeof(Reserva));
    long total_h = contar_registros(ARQ_HOSPEDAGENS, sizeof(Hospedagem));
    long i;
    Reserva r;
    Hospedagem hp;

    for (i = 0; i < total_r; i++) {
        if (ler_registro_pos(ARQ_RESERVAS, sizeof(Reserva), i, &r) &&
            r.ativo && r.codigo_hospede == codigo &&
            (r.status == RESERVA_ATIVA || r.status == RESERVA_FINALIZADA)) {
            return 1;
        }
    }
    for (i = 0; i < total_h; i++) {
        if (ler_registro_pos(ARQ_HOSPEDAGENS, sizeof(Hospedagem), i, &hp) &&
            hp.ativo && hp.codigo_hospede == codigo) {
            return 1;
        }
    }
    return 0;
}

void imprimir_hospede(const Hospede *h) {
    printf("----------------------------------------\n");
    printf("Codigo: %d\n", h->codigo);
    printf("Nome: %s\n", h->nome);
    printf("CPF: %s\n", h->cpf);
    printf("Telefone: %s\n", h->telefone);
    printf("E-mail: %s\n", h->email);
    printf("Nascimento: ");
    imprimir_data(h->data_nascimento);
    printf("\n");
}

static int ler_dados_hospede(Hospede *h, int permitir_nascimento_vazio) {
    char resp[8];
    ler_string(h->nome, MAX_NOME, "Nome: ");
    if (h->nome[0] == '\0') {
        return 0;
    }
    while (1) {
        ler_string(h->cpf, MAX_CPF, "CPF (somente numeros): ");
        if (h->cpf[0] == '\0') {
            return 0;
        }
        normalizar_cpf(h->cpf);
        if (!cpf_valido_formato(h->cpf)) {
            printf("CPF invalido. Informe 11 digitos.\n");
            continue;
        }
        break;
    }
    ler_string(h->telefone, MAX_TELEFONE, "Telefone: ");
    ler_string(h->email, MAX_EMAIL, "E-mail: ");
    if (permitir_nascimento_vazio) {
        ler_string(resp, sizeof(resp), "Informar data de nascimento? (s/n): ");
        if (resp[0] == 's' || resp[0] == 'S') {
            h->data_nascimento = ler_data("Data de nascimento");
        } else {
            h->data_nascimento.dia = 0;
            h->data_nascimento.mes = 0;
            h->data_nascimento.ano = 0;
        }
    }
    return 1;
}

void incluir_hospede(void) {
    Hospede h;
    Hospede existente;
    memset(&h, 0, sizeof(Hospede));

    printf("\n=== INCLUIR HOSPEDE ===\n");
    if (!ler_dados_hospede(&h, 1)) {
        printf("Inclusao cancelada.\n");
        return;
    }

    if (buscar_hospede_por_cpf(h.cpf, &existente)) {
        printf("Erro: ja existe hospede com este CPF (codigo %d).\n", existente.codigo);
        return;
    }

    h.codigo = gerar_codigo_hospede();
    h.ativo = 1;

    if (salvar_hospede(h)) {
        printf("Hospede cadastrado com sucesso! Codigo: %d\n", h.codigo);
    } else {
        printf("Erro ao salvar hospede no arquivo.\n");
    }
}

void alterar_hospede(void) {
    int codigo;
    Hospede h;
    Hospede outro;
    char cpf_antigo[MAX_CPF];

    printf("\n=== ALTERAR HOSPEDE ===\n");
    codigo = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(codigo, &h)) {
        printf("Hospede nao encontrado.\n");
        return;
    }

    imprimir_hospede(&h);
    printf("\nInforme os novos dados:\n");
    strncpy(cpf_antigo, h.cpf, MAX_CPF);
    if (!ler_dados_hospede(&h, 1)) {
        printf("Alteracao cancelada.\n");
        return;
    }

    if (strcmp(cpf_antigo, h.cpf) != 0 && buscar_hospede_por_cpf(h.cpf, &outro)) {
        printf("Erro: CPF ja cadastrado para outro hospede.\n");
        return;
    }

    h.ativo = 1;
    if (atualizar_hospede(h)) {
        printf("Hospede atualizado com sucesso.\n");
    } else {
        printf("Erro ao atualizar hospede.\n");
    }
}

void excluir_hospede(void) {
    int codigo;
    Hospede h;

    printf("\n=== EXCLUIR HOSPEDE ===\n");
    codigo = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(codigo, &h)) {
        printf("Hospede nao encontrado.\n");
        return;
    }

    imprimir_hospede(&h);
    if (hospede_em_uso(codigo)) {
        printf("Nao e possivel excluir: hospede possui reservas ou hospedagens vinculadas.\n");
        return;
    }
    if (!confirmar("Confirma exclusao? (s/n): ")) {
        printf("Operacao cancelada.\n");
        return;
    }

    h.ativo = 0;
    if (atualizar_hospede(h)) {
        printf("Hospede excluido com sucesso.\n");
    } else {
        printf("Erro ao excluir hospede.\n");
    }
}

void listar_hospedes(void) {
    long total = contar_registros(ARQ_HOSPEDES, sizeof(Hospede));
    long i;
    Hospede h;
    int encontrados = 0;

    printf("\n=== LISTA DE HOSPEDES ===\n");
    if (total == 0) {
        printf("Nenhum hospede cadastrado.\n");
        return;
    }

    printf("%-6s %-30s %-14s %-16s\n", "Cod", "Nome", "CPF", "Telefone");
    printf("--------------------------------------------------------------------\n");
    for (i = 0; i < total; i++) {
        if (ler_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), i, &h) && h.ativo) {
            printf("%-6d %-30.30s %-14s %-16s\n", h.codigo, h.nome, h.cpf, h.telefone);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum hospede ativo.\n");
    } else {
        printf("\nTotal: %d hospede(s).\n", encontrados);
    }
}

void buscar_hospede(void) {
    int opcao;
    Hospede h;
    long total, i;
    char termo[MAX_NOME];
    int achou = 0;

    printf("\n=== BUSCAR HOSPEDE ===\n");
    printf("1. Por codigo\n");
    printf("2. Por nome\n");
    printf("3. Por CPF\n");
    opcao = ler_inteiro("Opcao: ");

    if (opcao == 1) {
        int codigo = ler_inteiro("Codigo: ");
        if (buscar_hospede_por_codigo(codigo, &h)) {
            imprimir_hospede(&h);
        } else {
            printf("Hospede nao encontrado.\n");
        }
        return;
    }

    if (opcao == 2) {
        ler_string(termo, MAX_NOME, "Nome (parcial): ");
        total = contar_registros(ARQ_HOSPEDES, sizeof(Hospede));
        for (i = 0; i < total; i++) {
            if (ler_registro_pos(ARQ_HOSPEDES, sizeof(Hospede), i, &h) && h.ativo) {
                if (strstr(h.nome, termo) != NULL) {
                    imprimir_hospede(&h);
                    achou = 1;
                }
            }
        }
        if (!achou) {
            printf("Nenhum hospede encontrado.\n");
        }
        return;
    }

    if (opcao == 3) {
        ler_string(termo, MAX_CPF, "CPF: ");
        normalizar_cpf(termo);
        if (buscar_hospede_por_cpf(termo, &h)) {
            imprimir_hospede(&h);
        } else {
            printf("Hospede nao encontrado.\n");
        }
        return;
    }

    printf("Opcao invalida.\n");
}

void menu_hospedes(void) {
    int opcao;
    do {
        limpar_tela();
        printf("========================================\n");
        printf("       CADASTRO DE HOSPEDES\n");
        printf("========================================\n");
        printf("1. Incluir\n");
        printf("2. Alterar\n");
        printf("3. Excluir\n");
        printf("4. Listar todos\n");
        printf("5. Buscar\n");
        printf("0. Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
            case 1: incluir_hospede(); break;
            case 2: alterar_hospede(); break;
            case 3: excluir_hospede(); break;
            case 4: listar_hospedes(); break;
            case 5: buscar_hospede(); break;
            case 0: break;
            default: printf("Opcao invalida.\n"); break;
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}
