#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "util.h"

void limpar_tela(void) {
    /* Evita system("clear") para nao poluir saida redirecionada / scripts */
    printf("\n\n");
}

void pausar(void) {
    char buffer[8];
    if (!isatty(fileno(stdin))) {
        return; /* em scripts/redirecionamento, nao bloqueia */
    }
    printf("\nPressione ENTER para continuar...");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        clearerr(stdin);
    }
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void ler_string(char *destino, int tamanho, const char *prompt) {
    char buffer[512];
    printf("%s", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    strncpy(destino, buffer, (size_t)tamanho - 1);
    destino[tamanho - 1] = '\0';
}

int ler_inteiro(const char *prompt) {
    char buffer[64];
    int valor;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            clearerr(stdin);
            return 0;
        }
        if (sscanf(buffer, "%d %c", &valor, &extra) == 1) {
            return valor;
        }
        printf("Valor invalido. Digite um numero inteiro.\n");
    }
}

float ler_float(const char *prompt) {
    char buffer[64];
    float valor;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            clearerr(stdin);
            return 0.0f;
        }
        if (sscanf(buffer, "%f %c", &valor, &extra) == 1) {
            return valor;
        }
        printf("Valor invalido. Digite um numero decimal.\n");
    }
}

int confirmar(const char *prompt) {
    char resp[8];
    ler_string(resp, sizeof(resp), prompt);
    return (resp[0] == 's' || resp[0] == 'S');
}

static int ano_bissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int data_valida(Data d) {
    int dias_mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d.ano < 1900 || d.ano > 2100) {
        return 0;
    }
    if (d.mes < 1 || d.mes > 12) {
        return 0;
    }
    if (ano_bissexto(d.ano)) {
        dias_mes[2] = 29;
    }
    if (d.dia < 1 || d.dia > dias_mes[d.mes]) {
        return 0;
    }
    return 1;
}

int data_vazia(Data d) {
    return d.dia == 0 && d.mes == 0 && d.ano == 0;
}

Data ler_data(const char *prompt) {
    Data d;
    char buffer[64];
    d.dia = 0;
    d.mes = 0;
    d.ano = 0;
    while (1) {
        printf("%s (DD/MM/AAAA): ", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            clearerr(stdin);
            return d;
        }
        if (sscanf(buffer, "%d/%d/%d", &d.dia, &d.mes, &d.ano) == 3 && data_valida(d)) {
            return d;
        }
        printf("Data invalida. Use o formato DD/MM/AAAA.\n");
    }
}

int data_para_juliano(Data d) {
    int a = (14 - d.mes) / 12;
    int y = d.ano + 4800 - a;
    int m = d.mes + 12 * a - 3;
    return d.dia + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

int comparar_datas(Data a, Data b) {
    int ja = data_para_juliano(a);
    int jb = data_para_juliano(b);
    if (ja < jb) {
        return -1;
    }
    if (ja > jb) {
        return 1;
    }
    return 0;
}

int dias_entre(Data inicio, Data fim) {
    return data_para_juliano(fim) - data_para_juliano(inicio);
}

void imprimir_data(Data d) {
    if (data_vazia(d)) {
        printf("--/--/----");
    } else {
        printf("%02d/%02d/%04d", d.dia, d.mes, d.ano);
    }
}

void normalizar_cpf(char *cpf) {
    char limpo[MAX_CPF];
    int j = 0;
    int i;
    for (i = 0; cpf[i] != '\0' && j < MAX_CPF - 1; i++) {
        if (isdigit((unsigned char)cpf[i])) {
            limpo[j++] = cpf[i];
        }
    }
    limpo[j] = '\0';
    strncpy(cpf, limpo, MAX_CPF - 1);
    cpf[MAX_CPF - 1] = '\0';
}

int cpf_valido_formato(const char *cpf) {
    int i;
    if (cpf == NULL || strlen(cpf) != 11) {
        return 0;
    }
    for (i = 0; i < 11; i++) {
        if (!isdigit((unsigned char)cpf[i])) {
            return 0;
        }
    }
    /* rejeita sequencias iguais (000... / 111...) */
    for (i = 1; i < 11; i++) {
        if (cpf[i] != cpf[0]) {
            return 1;
        }
    }
    return 0;
}

const char *nome_tipo_quarto(TipoQuarto tipo) {
    switch (tipo) {
        case TIPO_SOLTEIRO: return "Solteiro";
        case TIPO_CASAL: return "Casal";
        case TIPO_LUXO: return "Luxo";
        case TIPO_FAMILIA: return "Familia";
        default: return "Desconhecido";
    }
}

const char *nome_status_quarto(StatusQuarto status) {
    switch (status) {
        case QUARTO_DISPONIVEL: return "Disponivel";
        case QUARTO_MANUTENCAO: return "Manutencao";
        default: return "Desconhecido";
    }
}

const char *nome_status_reserva(StatusReserva status) {
    switch (status) {
        case RESERVA_ATIVA: return "Ativa";
        case RESERVA_CANCELADA: return "Cancelada";
        case RESERVA_FINALIZADA: return "Finalizada";
        case RESERVA_NOSHOW: return "No-show";
        default: return "Desconhecido";
    }
}

const char *nome_forma_pagamento(FormaPagamento forma) {
    switch (forma) {
        case PAG_DINHEIRO: return "Dinheiro";
        case PAG_CARTAO_CREDITO: return "Cartao Credito";
        case PAG_CARTAO_DEBITO: return "Cartao Debito";
        case PAG_PIX: return "PIX";
        case PAG_TRANSFERENCIA: return "Transferencia";
        default: return "Desconhecido";
    }
}

void garantir_diretorio_dados(void) {
    struct stat st;
    if (stat("data", &st) == -1) {
        mkdir("data", 0755);
    }
}
