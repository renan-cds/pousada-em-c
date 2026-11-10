#include <stdio.h>
#include "util.h"
#include "hospede.h"
#include "quarto.h"
#include "reserva.h"
#include "hospedagem.h"
#include "relatorio.h"

static void menu_principal(void) {
    int opcao;
    do {
        limpar_tela();
        printf("========================================\n");
        printf("   SISTEMA DE GERENCIAMENTO DE POUSADA\n");
        printf("========================================\n");
        printf("1. Cadastro de Hospedes\n");
        printf("2. Cadastro de Quartos\n");
        printf("3. Gerenciar Reservas\n");
        printf("4. Check-in / Check-out\n");
        printf("5. Consultas e Relatorios\n");
        printf("0. Sair\n");
        printf("========================================\n");
        opcao = ler_inteiro("Opcao: ");

        switch (opcao) {
            case 1:
                menu_hospedes();
                break;
            case 2:
                menu_quartos();
                break;
            case 3:
                menu_reservas();
                break;
            case 4:
                menu_hospedagens();
                break;
            case 5:
                menu_relatorios();
                break;
            case 0:
                printf("\nEncerrando o sistema. Ate logo!\n");
                break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}

int main(void) {
    garantir_diretorio_dados();
    menu_principal();
    return 0;
}
