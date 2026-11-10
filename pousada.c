/*
 * Sistema de Gerenciamento de Pousada
 * Algoritmos II - Linguagem C
 * Ambiente: Windows (Dev-C++ / MinGW)
 *
 * Conteudo usado conforme a disciplina:
 * struct, typedef, enum, funcoes, menus,
 * string.h (strcpy, strlen, strcmp, strstr),
 * fgets, scanf, limpeza de buffer,
 * arquivos binarios (fopen, fread, fwrite, fseek, feof, remove, rename).
 * Interface Windows: conio.h (getch) + system("cls") + cores ANSI.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_NOME 80
#define MAX_CPF 15
#define MAX_TELEFONE 20
#define MAX_EMAIL 80
#define MAX_OBS 200

#define ARQ_HOSPEDES "hospedes.dat"
#define ARQ_QUARTOS "quartos.dat"
#define ARQ_RESERVAS "reservas.dat"
#define ARQ_HOSPEDAGENS "hospedagens.dat"

/* Cores ANSI no printf (Windows Terminal / Prompt do Windows 10+). */
#define COR_RESET    "\033[0m"
#define COR_NEGRITO  "\033[1m"
#define COR_VERDE    "\033[1;32m"
#define COR_VERMELHO "\033[1;31m"
#define COR_AMARELO  "\033[1;33m"
#define COR_CIANO    "\033[1;36m"
#define COR_MAGENTA  "\033[1;35m"
#define COR_AZUL     "\033[1;34m"
#define COR_BRANCO   "\033[1;37m"

/* -------------------- tipos -------------------- */

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

typedef enum {
    TIPO_SOLTEIRO = 1,
    TIPO_CASAL,
    TIPO_LUXO,
    TIPO_FAMILIA
} TipoQuarto;

typedef enum {
    QUARTO_DISPONIVEL = 1,
    QUARTO_MANUTENCAO
} StatusQuarto;

typedef enum {
    RESERVA_ATIVA = 1,
    RESERVA_CANCELADA,
    RESERVA_FINALIZADA,
    RESERVA_NOSHOW
} StatusReserva;

typedef enum {
    PAG_DINHEIRO = 1,
    PAG_CARTAO_CREDITO,
    PAG_CARTAO_DEBITO,
    PAG_PIX,
    PAG_TRANSFERENCIA
} FormaPagamento;

typedef struct {
    int codigo;
    char nome[MAX_NOME];
    char cpf[MAX_CPF];
    char telefone[MAX_TELEFONE];
    char email[MAX_EMAIL];
    Data data_nascimento;
} Hospede;

typedef struct {
    int numero;
    TipoQuarto tipo;
    float valor_diaria;
    int capacidade;
    StatusQuarto status;
} Quarto;

typedef struct {
    int codigo;
    int codigo_hospede;
    int numero_quarto;
    Data checkin;
    Data checkout;
    int qtd_hospedes;
    float valor_estimado;
    StatusReserva status;
} Reserva;

typedef struct {
    int codigo;
    int codigo_reserva;
    int codigo_hospede;
    int numero_quarto;
    Data entrada_real;
    Data saida_real;
    float valor_final;
    FormaPagamento forma_pagamento;
    char observacoes[MAX_OBS];
    int finalizada;
} Hospedagem;

/* -------------------- utilitarios / interface -------------------- */

void limpar_tela(void) {
    system("cls");
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void linha_caixa(void) {
    printf("%s+--------------------------------------------------+%s\n", COR_CIANO, COR_RESET);
}

void titulo_menu(char *titulo) {
    printf("\n");
    linha_caixa();
    printf("%s|  %s%s%s\n", COR_CIANO, COR_BRANCO, titulo, COR_CIANO);
    printf("|--------------------------------------------------+%s\n", COR_RESET);
}

void titulo_secao(char *titulo) {
    printf("\n%s--- %s ---%s\n", COR_MAGENTA, titulo, COR_RESET);
}

void msg_ok(char *texto) {
    printf("%s[OK]%s %s\n", COR_VERDE, COR_RESET, texto);
}

void msg_erro(char *texto) {
    printf("%s[ERRO]%s %s\n", COR_VERMELHO, COR_RESET, texto);
}

void msg_aviso(char *texto) {
    printf("%s[!]%s %s\n", COR_AMARELO, COR_RESET, texto);
}

void item_menu(int numero, char *texto) {
    printf("  %s%d.%s %s\n", COR_CIANO, numero, COR_RESET, texto);
}

void pausar(void) {
    printf("\n%sPressione qualquer tecla para continuar...%s", COR_AMARELO, COR_RESET);
    getch();
    printf("\n");
}

void ler_string(char *destino, int tamanho) {
    int i;
    fgets(destino, tamanho, stdin);
    i = strlen(destino);
    if (i > 0 && destino[i - 1] == '\n') {
        destino[i - 1] = '\0';
    }
}

int ler_inteiro(char *prompt) {
    int valor;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &valor) == 1) {
            limpar_buffer();
            return valor;
        }
        limpar_buffer();
        printf("Valor invalido. Digite um numero inteiro.\n");
    }
}

float ler_float(char *prompt) {
    float valor;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &valor) == 1) {
            limpar_buffer();
            return valor;
        }
        limpar_buffer();
        printf("Valor invalido. Digite um numero decimal.\n");
    }
}

int confirmar(char *prompt) {
    char resp[8];
    printf("%s", prompt);
    ler_string(resp, 8);
    return (resp[0] == 's' || resp[0] == 'S');
}

int ano_bissexto(int ano) {
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

/* Converte data em total de dias desde 01/01/1900 (estilo L02 / dma). */
int data_para_dias(Data d) {
    int dias_mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int total = 0;
    int a, m;

    for (a = 1900; a < d.ano; a++) {
        if (ano_bissexto(a)) {
            total = total + 366;
        } else {
            total = total + 365;
        }
    }
    if (ano_bissexto(d.ano)) {
        dias_mes[2] = 29;
    }
    for (m = 1; m < d.mes; m++) {
        total = total + dias_mes[m];
    }
    total = total + d.dia;
    return total;
}

int comparar_datas(Data a, Data b) {
    int da = data_para_dias(a);
    int db = data_para_dias(b);
    if (da < db) {
        return -1;
    }
    if (da > db) {
        return 1;
    }
    return 0;
}

int dias_entre(Data inicio, Data fim) {
    return data_para_dias(fim) - data_para_dias(inicio);
}

void ler_data(char *prompt, Data *d) {
    while (1) {
        printf("%s (DD/MM/AAAA): ", prompt);
        if (scanf("%d/%d/%d", &d->dia, &d->mes, &d->ano) == 3) {
            limpar_buffer();
            if (data_valida(*d)) {
                return;
            }
            printf("Data invalida. Use o formato DD/MM/AAAA.\n");
        } else {
            limpar_buffer();
            printf("Data invalida. Use o formato DD/MM/AAAA.\n");
        }
    }
}

void data_mais_um_dia(Data d, Data *r) {
    int dias_mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    r->dia = d.dia;
    r->mes = d.mes;
    r->ano = d.ano;
    if (ano_bissexto(r->ano)) {
        dias_mes[2] = 29;
    }
    r->dia = r->dia + 1;
    if (r->dia > dias_mes[r->mes]) {
        r->dia = 1;
        r->mes = r->mes + 1;
        if (r->mes > 12) {
            r->mes = 1;
            r->ano = r->ano + 1;
        }
    }
}

void imprimir_data(Data d) {
    if (data_vazia(d)) {
        printf("--/--/----");
    } else {
        printf("%d/%d/%d", d.dia, d.mes, d.ano);
    }
}

int eh_digito(char c) {
    return c >= '0' && c <= '9';
}

void normalizar_cpf(char *cpf) {
    char limpo[MAX_CPF];
    int i, j = 0;
    for (i = 0; cpf[i] != '\0' && j < MAX_CPF - 1; i++) {
        if (eh_digito(cpf[i])) {
            limpo[j] = cpf[i];
            j = j + 1;
        }
    }
    limpo[j] = '\0';
    strcpy(cpf, limpo);
}

int cpf_valido_formato(char *cpf) {
    int i;
    if (cpf == NULL || strlen(cpf) != 11) {
        return 0;
    }
    for (i = 0; i < 11; i++) {
        if (!eh_digito(cpf[i])) {
            return 0;
        }
    }
    for (i = 1; i < 11; i++) {
        if (cpf[i] != cpf[0]) {
            return 1;
        }
    }
    return 0;
}

void imprimir_tipo_quarto(TipoQuarto tipo) {
    if (tipo == TIPO_SOLTEIRO) {
        printf("Solteiro");
    } else if (tipo == TIPO_CASAL) {
        printf("Casal");
    } else if (tipo == TIPO_LUXO) {
        printf("Luxo");
    } else if (tipo == TIPO_FAMILIA) {
        printf("Familia");
    } else {
        printf("Desconhecido");
    }
}

void imprimir_status_quarto(StatusQuarto status) {
    if (status == QUARTO_DISPONIVEL) {
        printf("Disponivel");
    } else if (status == QUARTO_MANUTENCAO) {
        printf("Manutencao");
    } else {
        printf("Desconhecido");
    }
}

void imprimir_status_reserva(StatusReserva status) {
    if (status == RESERVA_ATIVA) {
        printf("Ativa");
    } else if (status == RESERVA_CANCELADA) {
        printf("Cancelada");
    } else if (status == RESERVA_FINALIZADA) {
        printf("Finalizada");
    } else if (status == RESERVA_NOSHOW) {
        printf("No-show");
    } else {
        printf("Desconhecido");
    }
}

void imprimir_forma_pagamento(FormaPagamento forma) {
    if (forma == PAG_DINHEIRO) {
        printf("Dinheiro");
    } else if (forma == PAG_CARTAO_CREDITO) {
        printf("Cartao Credito");
    } else if (forma == PAG_CARTAO_DEBITO) {
        printf("Cartao Debito");
    } else if (forma == PAG_PIX) {
        printf("PIX");
    } else if (forma == PAG_TRANSFERENCIA) {
        printf("Transferencia");
    } else {
        printf("Desconhecido");
    }
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
        if (opcao == 1) {
            return TIPO_SOLTEIRO;
        }
        if (opcao == 2) {
            return TIPO_CASAL;
        }
        if (opcao == 3) {
            return TIPO_LUXO;
        }
        if (opcao == 4) {
            return TIPO_FAMILIA;
        }
        msg_aviso("Opcao invalida.");
    }
}

StatusQuarto ler_status_quarto(void) {
    int opcao;
    while (1) {
        printf("Status do quarto:\n");
        printf("  1. Disponivel\n");
        printf("  2. Manutencao\n");
        opcao = ler_inteiro("Escolha: ");
        if (opcao == 1) {
            return QUARTO_DISPONIVEL;
        }
        if (opcao == 2) {
            return QUARTO_MANUTENCAO;
        }
        msg_aviso("Opcao invalida.");
    }
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
        if (opcao == 1) {
            return PAG_DINHEIRO;
        }
        if (opcao == 2) {
            return PAG_CARTAO_CREDITO;
        }
        if (opcao == 3) {
            return PAG_CARTAO_DEBITO;
        }
        if (opcao == 4) {
            return PAG_PIX;
        }
        if (opcao == 5) {
            return PAG_TRANSFERENCIA;
        }
        msg_aviso("Opcao invalida.");
    }
}

/* Proximo codigo = maior codigo do arquivo + 1 */
int gerar_codigo_hospede(void) {
    FILE *f = fopen(ARQ_HOSPEDES, "rb");
    Hospede h;
    int maior = 0;
    if (f != NULL) {
        while (fread(&h, sizeof(Hospede), 1, f) == 1) {
            if (h.codigo > maior) {
                maior = h.codigo;
            }
        }
        fclose(f);
    }
    return maior + 1;
}

int gerar_codigo_reserva(void) {
    FILE *f = fopen(ARQ_RESERVAS, "rb");
    Reserva r;
    int maior = 0;
    if (f != NULL) {
        while (fread(&r, sizeof(Reserva), 1, f) == 1) {
            if (r.codigo > maior) {
                maior = r.codigo;
            }
        }
        fclose(f);
    }
    return maior + 1;
}

int gerar_codigo_hospedagem(void) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    int maior = 0;
    if (f != NULL) {
        while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
            if (h.codigo > maior) {
                maior = h.codigo;
            }
        }
        fclose(f);
    }
    return maior + 1;
}

/* -------------------- hospedes -------------------- */

int buscar_hospede_por_codigo(int codigo, Hospede *destino) {
    FILE *f = fopen(ARQ_HOSPEDES, "rb");
    Hospede h;
    if (f == NULL) {
        return 0;
    }
    while (fread(&h, sizeof(Hospede), 1, f) == 1) {
        if (h.codigo == codigo) {
            *destino = h;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int buscar_hospede_por_cpf(char *cpf, Hospede *destino) {
    FILE *f = fopen(ARQ_HOSPEDES, "rb");
    Hospede h;
    if (f == NULL) {
        return 0;
    }
    while (fread(&h, sizeof(Hospede), 1, f) == 1) {
        if (strcmp(h.cpf, cpf) == 0) {
            if (destino != NULL) {
                *destino = h;
            }
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int posicao_hospede(int codigo) {
    FILE *f = fopen(ARQ_HOSPEDES, "rb");
    Hospede h;
    int pos = 0;
    if (f == NULL) {
        return -1;
    }
    while (fread(&h, sizeof(Hospede), 1, f) == 1) {
        if (h.codigo == codigo) {
            fclose(f);
            return pos;
        }
        pos = pos + 1;
    }
    fclose(f);
    return -1;
}

/* Bloqueia exclusao so com vinculo ativo (reserva ativa ou hospedagem em curso). */
int hospede_em_uso(int codigo) {
    FILE *fr = fopen(ARQ_RESERVAS, "rb");
    FILE *fh = fopen(ARQ_HOSPEDAGENS, "rb");
    Reserva r;
    Hospedagem hp;

    if (fr != NULL) {
        while (fread(&r, sizeof(Reserva), 1, fr) == 1) {
            if (r.codigo_hospede == codigo && r.status == RESERVA_ATIVA) {
                fclose(fr);
                if (fh != NULL) {
                    fclose(fh);
                }
                return 1;
            }
        }
        fclose(fr);
    }
    if (fh != NULL) {
        while (fread(&hp, sizeof(Hospedagem), 1, fh) == 1) {
            if (hp.codigo_hospede == codigo && hp.finalizada == 0) {
                fclose(fh);
                return 1;
            }
        }
        fclose(fh);
    }
    return 0;
}

void imprimir_hospede(Hospede *h) {
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

void incluir_hospede(void) {
    Hospede h, existente;
    char resp[8];
    FILE *f;

    titulo_secao("INCLUIR HOSPEDE");
    h.codigo = 0;
    h.data_nascimento.dia = 0;
    h.data_nascimento.mes = 0;
    h.data_nascimento.ano = 0;

    printf("Nome: ");
    ler_string(h.nome, MAX_NOME);
    if (strlen(h.nome) == 0) {
        msg_aviso("Inclusao cancelada.");
        return;
    }

    while (1) {
        printf("CPF (somente numeros): ");
        ler_string(h.cpf, MAX_CPF);
        if (strlen(h.cpf) == 0) {
            msg_aviso("Inclusao cancelada.");
            return;
        }
        normalizar_cpf(h.cpf);
        if (!cpf_valido_formato(h.cpf)) {
            printf("CPF invalido. Informe 11 digitos.\n");
        } else {
            break;
        }
    }

    printf("Telefone: ");
    ler_string(h.telefone, MAX_TELEFONE);
    printf("E-mail: ");
    ler_string(h.email, MAX_EMAIL);

    printf("Informar data de nascimento? (s/n): ");
    ler_string(resp, 8);
    if (resp[0] == 's' || resp[0] == 'S') {
        ler_data("Data de nascimento", &h.data_nascimento);
    }

    if (buscar_hospede_por_cpf(h.cpf, &existente)) {
        printf("Erro: ja existe hospede com este CPF (codigo %d).\n", existente.codigo);
        return;
    }

    h.codigo = gerar_codigo_hospede();
    f = fopen(ARQ_HOSPEDES, "ab");
    if (f == NULL) {
        msg_erro("Erro ao salvar hospede.");
        return;
    }
    fwrite(&h, sizeof(Hospede), 1, f);
    fclose(f);
    printf("%s[OK]%s Hospede cadastrado com sucesso! Codigo: %s%d%s\n", COR_VERDE, COR_RESET, COR_BRANCO, h.codigo, COR_RESET);
}

void alterar_hospede(void) {
    int codigo, pos;
    Hospede h, outro;
    char cpf_antigo[MAX_CPF];
    char resp[8];
    FILE *f;

    titulo_secao("ALTERAR HOSPEDE");
    codigo = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(codigo, &h)) {
        msg_erro("Hospede nao encontrado.");
        return;
    }
    pos = posicao_hospede(codigo);
    imprimir_hospede(&h);
    strcpy(cpf_antigo, h.cpf);

    printf("\nInforme os novos dados:\n");
    printf("Nome: ");
    ler_string(h.nome, MAX_NOME);
    while (1) {
        printf("CPF (somente numeros): ");
        ler_string(h.cpf, MAX_CPF);
        normalizar_cpf(h.cpf);
        if (!cpf_valido_formato(h.cpf)) {
            printf("CPF invalido. Informe 11 digitos.\n");
        } else {
            break;
        }
    }
    printf("Telefone: ");
    ler_string(h.telefone, MAX_TELEFONE);
    printf("E-mail: ");
    ler_string(h.email, MAX_EMAIL);
    printf("Informar data de nascimento? (s/n): ");
    ler_string(resp, 8);
    if (resp[0] == 's' || resp[0] == 'S') {
        ler_data("Data de nascimento", &h.data_nascimento);
    } else {
        h.data_nascimento.dia = 0;
        h.data_nascimento.mes = 0;
        h.data_nascimento.ano = 0;
    }

    if (strcmp(cpf_antigo, h.cpf) != 0 && buscar_hospede_por_cpf(h.cpf, &outro)) {
        printf("Erro: CPF ja cadastrado para outro hospede.\n");
        return;
    }

    f = fopen(ARQ_HOSPEDES, "rb+");
    if (f == NULL) {
        msg_erro("Erro ao abrir arquivo.");
        return;
    }
    fseek(f, pos * sizeof(Hospede), SEEK_SET);
    fwrite(&h, sizeof(Hospede), 1, f);
    fclose(f);
    msg_ok("Hospede atualizado com sucesso.");
}

void excluir_hospede(void) {
    int codigo;
    Hospede h, atual;
    FILE *origem, *temp;

    titulo_secao("EXCLUIR HOSPEDE");
    codigo = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(codigo, &h)) {
        msg_erro("Hospede nao encontrado.");
        return;
    }
    imprimir_hospede(&h);
    if (hospede_em_uso(codigo)) {
        printf("Nao e possivel excluir: hospede possui reservas ou hospedagens vinculadas.\n");
        return;
    }
    if (!confirmar("Confirma exclusao? (s/n): ")) {
        msg_aviso("Operacao cancelada.");
        return;
    }

    origem = fopen(ARQ_HOSPEDES, "rb");
    temp = fopen("hospedes.tmp", "wb");
    if (origem == NULL || temp == NULL) {
        msg_erro("Erro ao reorganizar arquivo.");
        if (origem != NULL) {
            fclose(origem);
        }
        if (temp != NULL) {
            fclose(temp);
        }
        return;
    }
    while (fread(&atual, sizeof(Hospede), 1, origem) == 1) {
        if (atual.codigo != codigo) {
            fwrite(&atual, sizeof(Hospede), 1, temp);
        }
    }
    fclose(origem);
    fclose(temp);
    remove(ARQ_HOSPEDES);
    rename("hospedes.tmp", ARQ_HOSPEDES);
    msg_ok("Hospede excluido com sucesso.");
}

void listar_hospedes(void) {
    FILE *f = fopen(ARQ_HOSPEDES, "rb");
    Hospede h;
    int total = 0;

    titulo_secao("LISTA DE HOSPEDES");
    if (f == NULL) {
        printf("Nenhum hospede cadastrado.\n");
        return;
    }
    printf("%sCod | Nome | CPF | Telefone%s\n", COR_AZUL, COR_RESET);
    printf("----------------------------------------\n");
    while (fread(&h, sizeof(Hospede), 1, f) == 1) {
        printf("%d | %s | %s | %s\n", h.codigo, h.nome, h.cpf, h.telefone);
        total = total + 1;
    }
    fclose(f);
    if (total == 0) {
        printf("Nenhum hospede cadastrado.\n");
    } else {
        printf("\nTotal: %d hospede(s).\n", total);
    }
}

void buscar_hospede(void) {
    int opcao, codigo;
    Hospede h;
    char termo[MAX_NOME];
    FILE *f;
    int achou = 0;

    titulo_secao("BUSCAR HOSPEDE");
    printf("1. Por codigo\n");
    printf("2. Por nome\n");
    printf("3. Por CPF\n");
    opcao = ler_inteiro("Opcao: ");

    if (opcao == 1) {
        codigo = ler_inteiro("Codigo: ");
        if (buscar_hospede_por_codigo(codigo, &h)) {
            imprimir_hospede(&h);
        } else {
            msg_erro("Hospede nao encontrado.");
        }
    } else if (opcao == 2) {
        printf("Nome (parcial): ");
        ler_string(termo, MAX_NOME);
        f = fopen(ARQ_HOSPEDES, "rb");
        if (f == NULL) {
            printf("Nenhum hospede cadastrado.\n");
        } else {
            while (fread(&h, sizeof(Hospede), 1, f) == 1) {
                if (strstr(h.nome, termo) != NULL) {
                    imprimir_hospede(&h);
                    achou = 1;
                }
            }
            fclose(f);
            if (!achou) {
                printf("Nenhum hospede encontrado.\n");
            }
        }
    } else if (opcao == 3) {
        printf("CPF: ");
        ler_string(termo, MAX_CPF);
        normalizar_cpf(termo);
        if (buscar_hospede_por_cpf(termo, &h)) {
            imprimir_hospede(&h);
        } else {
            msg_erro("Hospede nao encontrado.");
        }
    } else {
        msg_aviso("Opcao invalida.");
    }
}

void menu_hospedes(void) {
    int opcao;
    do {
        limpar_tela();
        titulo_menu("CADASTRO DE HOSPEDES");
        item_menu(1, "Incluir");
        item_menu(2, "Alterar");
        item_menu(3, "Excluir");
        item_menu(4, "Listar todos");
        item_menu(5, "Buscar");
        item_menu(0, "Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            incluir_hospede();
        } else if (opcao == 2) {
            alterar_hospede();
        } else if (opcao == 3) {
            excluir_hospede();
        } else if (opcao == 4) {
            listar_hospedes();
        } else if (opcao == 5) {
            buscar_hospede();
        } else if (opcao != 0) {
            msg_aviso("Opcao invalida.");
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}

/* -------------------- quartos -------------------- */

int buscar_quarto_por_numero(int numero, Quarto *destino) {
    FILE *f = fopen(ARQ_QUARTOS, "rb");
    Quarto q;
    if (f == NULL) {
        return 0;
    }
    while (fread(&q, sizeof(Quarto), 1, f) == 1) {
        if (q.numero == numero) {
            *destino = q;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int posicao_quarto(int numero) {
    FILE *f = fopen(ARQ_QUARTOS, "rb");
    Quarto q;
    int pos = 0;
    if (f == NULL) {
        return -1;
    }
    while (fread(&q, sizeof(Quarto), 1, f) == 1) {
        if (q.numero == numero) {
            fclose(f);
            return pos;
        }
        pos = pos + 1;
    }
    fclose(f);
    return -1;
}

int quarto_em_uso(int numero) {
    FILE *fr = fopen(ARQ_RESERVAS, "rb");
    FILE *fh = fopen(ARQ_HOSPEDAGENS, "rb");
    Reserva r;
    Hospedagem hp;

    if (fr != NULL) {
        while (fread(&r, sizeof(Reserva), 1, fr) == 1) {
            if (r.numero_quarto == numero && r.status == RESERVA_ATIVA) {
                fclose(fr);
                if (fh != NULL) {
                    fclose(fh);
                }
                return 1;
            }
        }
        fclose(fr);
    }
    if (fh != NULL) {
        while (fread(&hp, sizeof(Hospedagem), 1, fh) == 1) {
            if (hp.numero_quarto == numero && hp.finalizada == 0) {
                fclose(fh);
                return 1;
            }
        }
        fclose(fh);
    }
    return 0;
}

void imprimir_quarto(Quarto *q) {
    printf("----------------------------------------\n");
    printf("Numero: %d\n", q->numero);
    printf("Tipo: ");
    imprimir_tipo_quarto(q->tipo);
    printf("\nValor da diaria: R$ %.2f\n", q->valor_diaria);
    printf("Capacidade: %d pessoa(s)\n", q->capacidade);
    printf("Status: ");
    imprimir_status_quarto(q->status);
    printf("\n");
}

void incluir_quarto(void) {
    Quarto q, existente;
    FILE *f;

    titulo_secao("INCLUIR QUARTO");
    q.numero = ler_inteiro("Numero do quarto: ");
    if (q.numero <= 0) {
        msg_erro("Numero invalido.");
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
        msg_erro("Capacidade invalida.");
        return;
    }
    q.status = ler_status_quarto();

    f = fopen(ARQ_QUARTOS, "ab");
    if (f == NULL) {
        msg_erro("Erro ao salvar quarto.");
        return;
    }
    fwrite(&q, sizeof(Quarto), 1, f);
    fclose(f);
    printf("%s[OK]%s Quarto %s%d%s cadastrado com sucesso!\n", COR_VERDE, COR_RESET, COR_BRANCO, q.numero, COR_RESET);
}

void alterar_quarto(void) {
    int numero, pos;
    Quarto q;
    FILE *f;

    titulo_secao("ALTERAR QUARTO");
    numero = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(numero, &q)) {
        msg_erro("Quarto nao encontrado.");
        return;
    }
    pos = posicao_quarto(numero);
    imprimir_quarto(&q);
    printf("\nInforme os novos dados:\n");
    q.tipo = ler_tipo_quarto();
    q.valor_diaria = ler_float("Valor da diaria (R$): ");
    if (q.valor_diaria <= 0) {
        msg_erro("Valor invalido.");
        return;
    }
    q.capacidade = ler_inteiro("Capacidade (pessoas): ");
    if (q.capacidade <= 0) {
        msg_erro("Capacidade invalida.");
        return;
    }
    q.status = ler_status_quarto();

    f = fopen(ARQ_QUARTOS, "rb+");
    if (f == NULL) {
        msg_erro("Erro ao abrir arquivo.");
        return;
    }
    fseek(f, pos * sizeof(Quarto), SEEK_SET);
    fwrite(&q, sizeof(Quarto), 1, f);
    fclose(f);
    msg_ok("Quarto atualizado com sucesso.");
}

void excluir_quarto(void) {
    int numero;
    Quarto q, atual;
    FILE *origem, *temp;

    titulo_secao("EXCLUIR QUARTO");
    numero = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(numero, &q)) {
        msg_erro("Quarto nao encontrado.");
        return;
    }
    imprimir_quarto(&q);
    if (quarto_em_uso(numero)) {
        printf("Nao e possivel excluir: quarto possui reserva ativa ou hospedagem em andamento.\n");
        return;
    }
    if (!confirmar("Confirma exclusao? (s/n): ")) {
        msg_aviso("Operacao cancelada.");
        return;
    }

    origem = fopen(ARQ_QUARTOS, "rb");
    temp = fopen("quartos.tmp", "wb");
    if (origem == NULL || temp == NULL) {
        msg_erro("Erro ao reorganizar arquivo.");
        if (origem != NULL) {
            fclose(origem);
        }
        if (temp != NULL) {
            fclose(temp);
        }
        return;
    }
    while (fread(&atual, sizeof(Quarto), 1, origem) == 1) {
        if (atual.numero != numero) {
            fwrite(&atual, sizeof(Quarto), 1, temp);
        }
    }
    fclose(origem);
    fclose(temp);
    remove(ARQ_QUARTOS);
    rename("quartos.tmp", ARQ_QUARTOS);
    msg_ok("Quarto excluido com sucesso.");
}

void listar_quartos(void) {
    FILE *f = fopen(ARQ_QUARTOS, "rb");
    Quarto q;
    int total = 0;

    titulo_secao("LISTA DE QUARTOS");
    if (f == NULL) {
        printf("Nenhum quarto cadastrado.\n");
        return;
    }
    printf("%sNumero | Tipo | Diaria | Capacidade | Status%s\n", COR_AZUL, COR_RESET);
    printf("----------------------------------------------\n");
    while (fread(&q, sizeof(Quarto), 1, f) == 1) {
        printf("%d | ", q.numero);
        imprimir_tipo_quarto(q.tipo);
        printf(" | R$ %.2f | %d | ", q.valor_diaria, q.capacidade);
        imprimir_status_quarto(q.status);
        printf("\n");
        total = total + 1;
    }
    fclose(f);
    if (total == 0) {
        printf("Nenhum quarto cadastrado.\n");
    } else {
        printf("\nTotal: %d quarto(s).\n", total);
    }
}

void buscar_quarto(void) {
    int numero;
    Quarto q;
    titulo_secao("BUSCAR QUARTO");
    numero = ler_inteiro("Numero do quarto: ");
    if (buscar_quarto_por_numero(numero, &q)) {
        imprimir_quarto(&q);
    } else {
        msg_erro("Quarto nao encontrado.");
    }
}

void menu_quartos(void) {
    int opcao;
    do {
        limpar_tela();
        titulo_menu("CADASTRO DE QUARTOS");
        item_menu(1, "Incluir");
        item_menu(2, "Alterar");
        item_menu(3, "Excluir");
        item_menu(4, "Listar todos");
        item_menu(5, "Buscar por numero");
        item_menu(0, "Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            incluir_quarto();
        } else if (opcao == 2) {
            alterar_quarto();
        } else if (opcao == 3) {
            excluir_quarto();
        } else if (opcao == 4) {
            listar_quartos();
        } else if (opcao == 5) {
            buscar_quarto();
        } else if (opcao != 0) {
            msg_aviso("Opcao invalida.");
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}

/* -------------------- disponibilidade / reservas -------------------- */

/*
 * Periodos no formato [inicio, fim): o dia de check-out libera o quarto.
 * Exemplo: reserva 10/11 a 12/11 ocupa 10 e 11; dia 12 ja pode nova entrada.
 * Sobrepoe se: a_ini < b_fim E b_ini < a_fim.
 */
int periodos_sobrepoem(Data a_ini, Data a_fim, Data b_ini, Data b_fim) {
    return comparar_datas(a_ini, b_fim) < 0 && comparar_datas(b_ini, a_fim) < 0;
}

float calcular_valor_estimado(float valor_diaria, Data checkin, Data checkout) {
    int dias = dias_entre(checkin, checkout);
    if (dias <= 0) {
        return 0.0f;
    }
    return valor_diaria * (float)dias;
}

int quarto_disponivel_periodo(int numero_quarto, Data inicio, Data fim, int codigo_ignorar) {
    Quarto q;
    Reserva r;
    Hospedagem hp;
    FILE *fr, *fh;

    if (!buscar_quarto_por_numero(numero_quarto, &q)) {
        return 0;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        return 0;
    }

    fr = fopen(ARQ_RESERVAS, "rb");
    if (fr != NULL) {
        while (fread(&r, sizeof(Reserva), 1, fr) == 1) {
            if (r.status == RESERVA_ATIVA) {
                if (r.numero_quarto == numero_quarto) {
                    if (!(codigo_ignorar > 0 && r.codigo == codigo_ignorar)) {
                        if (periodos_sobrepoem(inicio, fim, r.checkin, r.checkout)) {
                            fclose(fr);
                            return 0;
                        }
                    }
                }
            }
        }
        fclose(fr);
    }

    fh = fopen(ARQ_HOSPEDAGENS, "rb");
    if (fh != NULL) {
        while (fread(&hp, sizeof(Hospedagem), 1, fh) == 1) {
            if (hp.numero_quarto == numero_quarto) {
                if (hp.finalizada == 0) {
                    /* Hospedagem em andamento: ocupa desde a entrada, sem data fim. */
                    if (comparar_datas(hp.entrada_real, fim) < 0) {
                        fclose(fh);
                        return 0;
                    }
                } else {
                    if (periodos_sobrepoem(inicio, fim, hp.entrada_real, hp.saida_real)) {
                        fclose(fh);
                        return 0;
                    }
                }
            }
        }
        fclose(fh);
    }
    return 1;
}

int buscar_reserva_por_codigo(int codigo, Reserva *destino) {
    FILE *f = fopen(ARQ_RESERVAS, "rb");
    Reserva r;
    if (f == NULL) {
        return 0;
    }
    while (fread(&r, sizeof(Reserva), 1, f) == 1) {
        if (r.codigo == codigo) {
            *destino = r;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int posicao_reserva(int codigo) {
    FILE *f = fopen(ARQ_RESERVAS, "rb");
    Reserva r;
    int pos = 0;
    if (f == NULL) {
        return -1;
    }
    while (fread(&r, sizeof(Reserva), 1, f) == 1) {
        if (r.codigo == codigo) {
            fclose(f);
            return pos;
        }
        pos = pos + 1;
    }
    fclose(f);
    return -1;
}

void imprimir_reserva(Reserva *r) {
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
    printf("Status: ");
    imprimir_status_reserva(r->status);
    printf("\n");
}

void verificar_disponibilidade_menu(void) {
    int numero;
    Data inicio, fim;
    Quarto q;

    titulo_secao("VERIFICAR DISPONIBILIDADE");
    numero = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(numero, &q)) {
        msg_erro("Quarto nao encontrado.");
        return;
    }
    imprimir_quarto(&q);
    ler_data("Data inicio", &inicio);
    ler_data("Data fim", &fim);
    if (comparar_datas(inicio, fim) >= 0) {
        printf("A data fim deve ser posterior a data inicio.\n");
        return;
    }
    if (quarto_disponivel_periodo(numero, inicio, fim, 0)) {
        printf("\n%s[OK]%s Quarto %d %sDISPONIVEL%s no periodo.\n", COR_VERDE, COR_RESET, numero, COR_VERDE, COR_RESET);
        printf("Valor estimado: R$ %.2f (%d diaria(s))\n",
               calcular_valor_estimado(q.valor_diaria, inicio, fim),
               dias_entre(inicio, fim));
    } else {
        if (q.status == QUARTO_MANUTENCAO) {
            printf("\n%s[!]%s Quarto %d esta em %sMANUTENCAO%s.\n", COR_AMARELO, COR_RESET, numero, COR_AMARELO, COR_RESET);
        } else {
            printf("\n%s[X]%s Quarto %d %sINDISPONIVEL%s no periodo informado.\n", COR_VERMELHO, COR_RESET, numero, COR_VERMELHO, COR_RESET);
        }
    }
}

void incluir_reserva(void) {
    Reserva r;
    Hospede h;
    Quarto q;
    FILE *f;

    titulo_secao("INCLUIR RESERVA");
    r.codigo_hospede = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(r.codigo_hospede, &h)) {
        msg_erro("Hospede nao encontrado.");
        return;
    }
    printf("Hospede: %s\n", h.nome);

    r.numero_quarto = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(r.numero_quarto, &q)) {
        msg_erro("Quarto nao encontrado.");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        printf("Quarto em manutencao. Nao e possivel reservar.\n");
        return;
    }
    printf("Quarto %d - diaria R$ %.2f - capacidade %d\n",
           q.numero, q.valor_diaria, q.capacidade);

    ler_data("Data de check-in", &r.checkin);
    ler_data("Data de check-out", &r.checkout);
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
    r.codigo = gerar_codigo_reserva();

    printf("\nResumo: ");
    imprimir_data(r.checkin);
    printf(" a ");
    imprimir_data(r.checkout);
    printf(" (%d diaria(s)) | R$ %.2f\n", dias_entre(r.checkin, r.checkout), r.valor_estimado);
    if (!confirmar("Confirmar reserva? (s/n): ")) {
        msg_aviso("Reserva nao confirmada.");
        return;
    }

    f = fopen(ARQ_RESERVAS, "ab");
    if (f == NULL) {
        msg_erro("Erro ao salvar reserva.");
        return;
    }
    fwrite(&r, sizeof(Reserva), 1, f);
    fclose(f);
    printf("%s[OK]%s Reserva cadastrada com sucesso! Codigo: %s%d%s\n", COR_VERDE, COR_RESET, COR_BRANCO, r.codigo, COR_RESET);
}

void alterar_reserva(void) {
    int codigo, pos;
    Reserva r;
    Quarto q;
    FILE *f;

    titulo_secao("ALTERAR RESERVA");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        msg_erro("Reserva nao encontrada.");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Somente reservas ATIVAS podem ser alteradas.\n");
        return;
    }
    pos = posicao_reserva(codigo);
    imprimir_reserva(&r);
    printf("\nNovos dados:\n");

    r.numero_quarto = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(r.numero_quarto, &q)) {
        msg_erro("Quarto nao encontrado.");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        msg_erro("Quarto em manutencao.");
        return;
    }
    ler_data("Data de check-in", &r.checkin);
    ler_data("Data de check-out", &r.checkout);
    if (comparar_datas(r.checkin, r.checkout) >= 0) {
        msg_erro("Datas invalidas.");
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

    f = fopen(ARQ_RESERVAS, "rb+");
    if (f == NULL) {
        msg_erro("Erro ao abrir arquivo.");
        return;
    }
    fseek(f, pos * sizeof(Reserva), SEEK_SET);
    fwrite(&r, sizeof(Reserva), 1, f);
    fclose(f);
    printf("Reserva atualizada. Novo valor estimado: R$ %.2f\n", r.valor_estimado);
}

void cancelar_reserva(void) {
    int codigo, pos;
    Reserva r;
    FILE *f;

    titulo_secao("CANCELAR RESERVA");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        msg_erro("Reserva nao encontrada.");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Reserva nao esta ativa.\n");
        return;
    }
    pos = posicao_reserva(codigo);
    imprimir_reserva(&r);
    if (!confirmar("Confirma cancelamento? (s/n): ")) {
        msg_aviso("Operacao cancelada.");
        return;
    }
    r.status = RESERVA_CANCELADA;
    f = fopen(ARQ_RESERVAS, "rb+");
    if (f == NULL) {
        msg_erro("Erro ao abrir arquivo.");
        return;
    }
    fseek(f, pos * sizeof(Reserva), SEEK_SET);
    fwrite(&r, sizeof(Reserva), 1, f);
    fclose(f);
    msg_ok("Reserva cancelada. Quarto liberado no periodo.");
}

void marcar_noshow(void) {
    int codigo, pos;
    Reserva r;
    FILE *f;

    titulo_secao("MARCAR NO-SHOW");
    printf("(Hospede nao compareceu; libera o quarto no periodo)\n");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        msg_erro("Reserva nao encontrada.");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Somente reservas ATIVAS podem ser marcadas como no-show.\n");
        return;
    }
    pos = posicao_reserva(codigo);
    imprimir_reserva(&r);
    if (!confirmar("Confirma no-show? (s/n): ")) {
        msg_aviso("Operacao cancelada.");
        return;
    }
    r.status = RESERVA_NOSHOW;
    f = fopen(ARQ_RESERVAS, "rb+");
    if (f == NULL) {
        msg_erro("Erro ao abrir arquivo.");
        return;
    }
    fseek(f, pos * sizeof(Reserva), SEEK_SET);
    fwrite(&r, sizeof(Reserva), 1, f);
    fclose(f);
    msg_ok("Reserva marcada como No-show. Quarto liberado no periodo.");
}

void excluir_reserva(void) {
    int codigo;
    Reserva r, atual;
    FILE *origem, *temp;

    titulo_secao("EXCLUIR RESERVA");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo, &r)) {
        msg_erro("Reserva nao encontrada.");
        return;
    }
    if (r.status == RESERVA_ATIVA) {
        printf("Cancele a reserva antes de excluir, ou finalize via check-out.\n");
        return;
    }
    if (!confirmar("Confirma exclusao definitiva do registro? (s/n): ")) {
        msg_aviso("Operacao cancelada.");
        return;
    }

    origem = fopen(ARQ_RESERVAS, "rb");
    temp = fopen("reservas.tmp", "wb");
    if (origem == NULL || temp == NULL) {
        msg_erro("Erro ao reorganizar arquivo.");
        if (origem != NULL) {
            fclose(origem);
        }
        if (temp != NULL) {
            fclose(temp);
        }
        return;
    }
    while (fread(&atual, sizeof(Reserva), 1, origem) == 1) {
        if (atual.codigo != codigo) {
            fwrite(&atual, sizeof(Reserva), 1, temp);
        }
    }
    fclose(origem);
    fclose(temp);
    remove(ARQ_RESERVAS);
    rename("reservas.tmp", ARQ_RESERVAS);
    msg_ok("Registro de reserva excluido.");
}

void listar_reservas(void) {
    FILE *f = fopen(ARQ_RESERVAS, "rb");
    Reserva r;
    int total = 0;

    titulo_secao("LISTA DE RESERVAS");
    if (f == NULL) {
        printf("Nenhuma reserva cadastrada.\n");
        return;
    }
    printf("%sCod | Hosp. | Quarto | Check-in | Check-out | Valor | Status%s\n", COR_AZUL, COR_RESET);
    printf("------------------------------------------------------------\n");
    while (fread(&r, sizeof(Reserva), 1, f) == 1) {
        printf("%d | %d | %d | ", r.codigo, r.codigo_hospede, r.numero_quarto);
        imprimir_data(r.checkin);
        printf(" | ");
        imprimir_data(r.checkout);
        printf(" | R$ %.2f | ", r.valor_estimado);
        imprimir_status_reserva(r.status);
        printf("\n");
        total = total + 1;
    }
    fclose(f);
    if (total == 0) {
        printf("Nenhuma reserva cadastrada.\n");
    } else {
        printf("\nTotal: %d reserva(s).\n", total);
    }
}

void buscar_reserva(void) {
    int codigo;
    Reserva r;
    titulo_secao("BUSCAR RESERVA");
    codigo = ler_inteiro("Codigo da reserva: ");
    if (buscar_reserva_por_codigo(codigo, &r)) {
        imprimir_reserva(&r);
    } else {
        msg_erro("Reserva nao encontrada.");
    }
}

void menu_reservas(void) {
    int opcao;
    do {
        limpar_tela();
        titulo_menu("GERENCIAR RESERVAS");
        item_menu(1, "Incluir reserva");
        item_menu(2, "Alterar reserva");
        item_menu(3, "Cancelar reserva");
        item_menu(4, "Marcar no-show");
        item_menu(5, "Excluir reserva");
        item_menu(6, "Listar todas");
        item_menu(7, "Buscar por codigo");
        item_menu(8, "Verificar disponibilidade");
        item_menu(0, "Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            incluir_reserva();
        } else if (opcao == 2) {
            alterar_reserva();
        } else if (opcao == 3) {
            cancelar_reserva();
        } else if (opcao == 4) {
            marcar_noshow();
        } else if (opcao == 5) {
            excluir_reserva();
        } else if (opcao == 6) {
            listar_reservas();
        } else if (opcao == 7) {
            buscar_reserva();
        } else if (opcao == 8) {
            verificar_disponibilidade_menu();
        } else if (opcao != 0) {
            msg_aviso("Opcao invalida.");
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}

/* -------------------- hospedagens -------------------- */

int buscar_hospedagem_por_codigo(int codigo, Hospedagem *destino) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    if (f == NULL) {
        return 0;
    }
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        if (h.codigo == codigo) {
            *destino = h;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int posicao_hospedagem(int codigo) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    int pos = 0;
    if (f == NULL) {
        return -1;
    }
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        if (h.codigo == codigo) {
            fclose(f);
            return pos;
        }
        pos = pos + 1;
    }
    fclose(f);
    return -1;
}

int hospede_esta_hospedado(int codigo_hospede) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    if (f == NULL) {
        return 0;
    }
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        if (h.finalizada == 0 && h.codigo_hospede == codigo_hospede) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int quarto_tem_hospedagem_ativa(int numero_quarto) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    if (f == NULL) {
        return 0;
    }
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        if (h.finalizada == 0 && h.numero_quarto == numero_quarto) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void imprimir_hospedagem(Hospedagem *h) {
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
    printf("\nQuarto: %d\n", h->numero_quarto);
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
        printf("Pagamento: ");
        imprimir_forma_pagamento(h->forma_pagamento);
        printf("\n");
    }
    if (h->observacoes[0] != '\0') {
        printf("Obs: %s\n", h->observacoes);
    }
    if (h->finalizada) {
        printf("Situacao: Finalizada\n");
    } else {
        printf("Situacao: Em andamento\n");
    }
}

void checkin_por_reserva(void) {
    int codigo_reserva, pos;
    Reserva r;
    Quarto q;
    Hospedagem h;
    Data entrada;
    FILE *f;

    titulo_secao("CHECK-IN POR RESERVA");
    codigo_reserva = ler_inteiro("Codigo da reserva: ");
    if (!buscar_reserva_por_codigo(codigo_reserva, &r)) {
        msg_erro("Reserva nao encontrada.");
        return;
    }
    if (r.status != RESERVA_ATIVA) {
        printf("Reserva nao esta ativa.\n");
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
        msg_erro("Quarto em manutencao.");
        return;
    }

    ler_data("Data real de entrada", &entrada);
    if (!quarto_disponivel_periodo(r.numero_quarto, entrada, r.checkout, r.codigo)) {
        printf("Conflito de disponibilidade no periodo.\n");
        return;
    }

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
    printf("Observacoes (opcional): ");
    ler_string(h.observacoes, MAX_OBS);
    h.finalizada = 0;

    f = fopen(ARQ_HOSPEDAGENS, "ab");
    if (f == NULL) {
        msg_erro("Erro ao registrar hospedagem.");
        return;
    }
    fwrite(&h, sizeof(Hospedagem), 1, f);
    fclose(f);

    r.status = RESERVA_FINALIZADA;
    pos = posicao_reserva(r.codigo);
    f = fopen(ARQ_RESERVAS, "rb+");
    if (f != NULL && pos >= 0) {
        fseek(f, pos * sizeof(Reserva), SEEK_SET);
        fwrite(&r, sizeof(Reserva), 1, f);
        fclose(f);
    }

    printf("%s[OK]%s Check-in realizado! Codigo da hospedagem: %s%d%s\n", COR_VERDE, COR_RESET, COR_BRANCO, h.codigo, COR_RESET);
    printf("%sQuarto %d ocupado.%s\n", COR_AMARELO, h.numero_quarto, COR_RESET);
}

void checkin_direto(void) {
    Hospedagem h;
    Hospede hosp;
    Quarto q;
    Data saida_prevista;
    FILE *f;

    titulo_secao("CHECK-IN DIRETO (SEM RESERVA)");
    h.codigo_hospede = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(h.codigo_hospede, &hosp)) {
        msg_erro("Hospede nao encontrado.");
        return;
    }
    if (hospede_esta_hospedado(h.codigo_hospede)) {
        msg_erro("Hospede ja esta hospedado.");
        return;
    }
    h.numero_quarto = ler_inteiro("Numero do quarto: ");
    if (!buscar_quarto_por_numero(h.numero_quarto, &q)) {
        msg_erro("Quarto nao encontrado.");
        return;
    }
    if (q.status == QUARTO_MANUTENCAO) {
        msg_erro("Quarto em manutencao.");
        return;
    }
    if (quarto_tem_hospedagem_ativa(h.numero_quarto)) {
        msg_erro("Quarto ja ocupado.");
        return;
    }

    ler_data("Data de entrada", &h.entrada_real);
    ler_data("Data prevista de saida", &saida_prevista);
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
    printf("Observacoes (opcional): ");
    ler_string(h.observacoes, MAX_OBS);
    h.finalizada = 0;

    printf("Valor estimado: R$ %.2f\n", h.valor_final);
    if (!confirmar("Confirmar check-in? (s/n): ")) {
        msg_aviso("Operacao cancelada.");
        return;
    }

    f = fopen(ARQ_HOSPEDAGENS, "ab");
    if (f == NULL) {
        msg_erro("Erro ao salvar hospedagem.");
        return;
    }
    fwrite(&h, sizeof(Hospedagem), 1, f);
    fclose(f);
    printf("%s[OK]%s Check-in realizado! Codigo: %s%d%s\n", COR_VERDE, COR_RESET, COR_BRANCO, h.codigo, COR_RESET);
}

void checkout(void) {
    int codigo, pos, dias;
    Hospedagem h;
    Quarto q;
    float valor_calc;
    FILE *f;

    titulo_secao("CHECK-OUT");
    codigo = ler_inteiro("Codigo da hospedagem: ");
    if (!buscar_hospedagem_por_codigo(codigo, &h)) {
        msg_erro("Hospedagem nao encontrada.");
        return;
    }
    if (h.finalizada) {
        printf("Hospedagem ja foi finalizada.\n");
        return;
    }
    pos = posicao_hospedagem(codigo);
    imprimir_hospedagem(&h);

    ler_data("Data real de saida", &h.saida_real);
    if (comparar_datas(h.entrada_real, h.saida_real) >= 0) {
        printf("Data de saida deve ser posterior a entrada.\n");
        return;
    }

    dias = dias_entre(h.entrada_real, h.saida_real);
    if (dias <= 0) {
        dias = 1;
    }
    if (buscar_quarto_por_numero(h.numero_quarto, &q)) {
        valor_calc = q.valor_diaria * (float)dias;
    } else {
        valor_calc = h.valor_final;
    }

    printf("Diarias: %d\n", dias);
    printf("Valor calculado: R$ %.2f\n", valor_calc);
    if (confirmar("Ajustar valor manualmente? (s/n): ")) {
        h.valor_final = ler_float("Valor total final (R$): ");
    } else {
        h.valor_final = valor_calc;
    }
    h.forma_pagamento = ler_forma_pagamento();
    printf("Observacoes finais (opcional): ");
    ler_string(h.observacoes, MAX_OBS);
    h.finalizada = 1;

    f = fopen(ARQ_HOSPEDAGENS, "rb+");
    if (f == NULL) {
        msg_erro("Erro ao atualizar hospedagem.");
        return;
    }
    fseek(f, pos * sizeof(Hospedagem), SEEK_SET);
    fwrite(&h, sizeof(Hospedagem), 1, f);
    fclose(f);

    msg_ok("Check-out realizado!");
    printf("%sQuarto %d liberado.%s\n", COR_VERDE, h.numero_quarto, COR_RESET);
    printf("Valor total: R$ %.2f | Pagamento: ", h.valor_final);
    imprimir_forma_pagamento(h.forma_pagamento);
    printf("\n");
}

void listar_hospedagens(void) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    int total = 0;

    titulo_secao("LISTA DE HOSPEDAGENS");
    if (f == NULL) {
        printf("Nenhuma hospedagem registrada.\n");
        return;
    }
    printf("%sCod | Hosp. | Quarto | Entrada | Saida | Valor | Situacao%s\n", COR_AZUL, COR_RESET);
    printf("---------------------------------------------------------\n");
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        printf("%d | %d | %d | ", h.codigo, h.codigo_hospede, h.numero_quarto);
        imprimir_data(h.entrada_real);
        printf(" | ");
        if (h.finalizada) {
            imprimir_data(h.saida_real);
        } else {
            printf("Em curso");
        }
        printf(" | R$ %.2f | ", h.valor_final);
        if (h.finalizada) {
            printf("Finalizada\n");
        } else {
            printf("Andamento\n");
        }
        total = total + 1;
    }
    fclose(f);
    if (total == 0) {
        printf("Nenhuma hospedagem registrada.\n");
    } else {
        printf("\nTotal: %d hospedagem(ns).\n", total);
    }
}

void buscar_hospedagem(void) {
    int codigo;
    Hospedagem h;
    titulo_secao("BUSCAR HOSPEDAGEM");
    codigo = ler_inteiro("Codigo da hospedagem: ");
    if (buscar_hospedagem_por_codigo(codigo, &h)) {
        imprimir_hospedagem(&h);
    } else {
        msg_erro("Hospedagem nao encontrada.");
    }
}

void menu_hospedagens(void) {
    int opcao;
    do {
        limpar_tela();
        titulo_menu("CHECK-IN / CHECK-OUT");
        item_menu(1, "Check-in a partir de reserva");
        item_menu(2, "Check-in direto");
        item_menu(3, "Check-out");
        item_menu(4, "Listar hospedagens");
        item_menu(5, "Buscar hospedagem");
        item_menu(0, "Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            checkin_por_reserva();
        } else if (opcao == 2) {
            checkin_direto();
        } else if (opcao == 3) {
            checkout();
        } else if (opcao == 4) {
            listar_hospedagens();
        } else if (opcao == 5) {
            buscar_hospedagem();
        } else if (opcao != 0) {
            msg_aviso("Opcao invalida.");
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}

/* -------------------- relatorios -------------------- */

void relatorio_reservas_periodo(void) {
    Data inicio, fim;
    FILE *f;
    Reserva r;
    int encontrados = 0;

    titulo_secao("RESERVAS POR PERIODO");
    ler_data("Inicio do periodo", &inicio);
    ler_data("Fim do periodo", &fim);
    if (comparar_datas(inicio, fim) > 0) {
        msg_erro("Periodo invalido.");
        return;
    }

    f = fopen(ARQ_RESERVAS, "rb");
    if (f == NULL) {
        printf("Nenhuma reserva encontrada.\n");
        return;
    }
    printf("\nCod | Hosp. | Quarto | Check-in | Check-out | Status\n");
    printf("----------------------------------------------------\n");
    while (fread(&r, sizeof(Reserva), 1, f) == 1) {
        if (comparar_datas(r.checkin, inicio) >= 0 && comparar_datas(r.checkin, fim) <= 0) {
            printf("%d | %d | %d | ", r.codigo, r.codigo_hospede, r.numero_quarto);
            imprimir_data(r.checkin);
            printf(" | ");
            imprimir_data(r.checkout);
            printf(" | ");
            imprimir_status_reserva(r.status);
            printf("\n");
            encontrados = encontrados + 1;
        }
    }
    fclose(f);
    if (encontrados == 0) {
        printf("Nenhuma reserva encontrada no periodo.\n");
    } else {
        printf("\nTotal: %d reserva(s).\n", encontrados);
    }
}

void relatorio_hospedes_atuais(void) {
    FILE *f = fopen(ARQ_HOSPEDAGENS, "rb");
    Hospedagem h;
    Hospede hosp;
    int encontrados = 0;

    titulo_secao("HOSPEDES ATUALMENTE HOSPEDADOS");
    printf("Cod.H | Nome | Quarto | Entrada\n");
    printf("--------------------------------\n");
    if (f == NULL) {
        printf("Nenhum hospede hospedado no momento.\n");
        return;
    }
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        if (h.finalizada == 0) {
            if (!buscar_hospede_por_codigo(h.codigo_hospede, &hosp)) {
                strcpy(hosp.nome, "(desconhecido)");
            }
            printf("%d | %s | %d | ", h.codigo_hospede, hosp.nome, h.numero_quarto);
            imprimir_data(h.entrada_real);
            printf("\n");
            encontrados = encontrados + 1;
        }
    }
    fclose(f);
    if (encontrados == 0) {
        printf("Nenhum hospede hospedado no momento.\n");
    } else {
        printf("\nTotal: %d hospede(s) hospedado(s).\n", encontrados);
    }
}

void relatorio_quartos_disponiveis_data(void) {
    Data data, dia_seguinte;
    FILE *f;
    Quarto q;
    int encontrados = 0;

    titulo_secao("QUARTOS DISPONIVEIS EM UMA DATA");
    ler_data("Data", &data);
    data_mais_um_dia(data, &dia_seguinte);

    f = fopen(ARQ_QUARTOS, "rb");
    if (f == NULL) {
        printf("Nenhum quarto cadastrado.\n");
        return;
    }
    printf("\nNumero | Tipo | Diaria | Capacidade\n");
    printf("------------------------------------\n");
    while (fread(&q, sizeof(Quarto), 1, f) == 1) {
        if (q.status != QUARTO_MANUTENCAO) {
            if (quarto_disponivel_periodo(q.numero, data, dia_seguinte, 0)) {
                printf("%d | ", q.numero);
                imprimir_tipo_quarto(q.tipo);
                printf(" | R$ %.2f | %d\n", q.valor_diaria, q.capacidade);
                encontrados = encontrados + 1;
            }
        }
    }
    fclose(f);
    if (encontrados == 0) {
        printf("Nenhum quarto disponivel nesta data.\n");
    } else {
        printf("\nTotal: %d quarto(s) disponivel(is).\n", encontrados);
    }
}

void relatorio_faturamento_periodo(void) {
    Data inicio, fim;
    FILE *f;
    Hospedagem h;
    float total_fat = 0.0f;
    int contagem = 0;

    titulo_secao("FATURAMENTO POR PERIODO");
    printf("(Considera hospedagens com check-out no periodo)\n");
    ler_data("Inicio do periodo", &inicio);
    ler_data("Fim do periodo", &fim);
    if (comparar_datas(inicio, fim) > 0) {
        msg_erro("Periodo invalido.");
        return;
    }

    f = fopen(ARQ_HOSPEDAGENS, "rb");
    if (f == NULL) {
        printf("Nenhuma hospedagem finalizada no periodo.\n");
        return;
    }
    printf("\nCod | Hosp. | Quarto | Saida | Valor | Pagamento\n");
    printf("-----------------------------------------------\n");
    while (fread(&h, sizeof(Hospedagem), 1, f) == 1) {
        if (h.finalizada) {
            if (comparar_datas(h.saida_real, inicio) >= 0 &&
                comparar_datas(h.saida_real, fim) <= 0) {
                printf("%d | %d | %d | ", h.codigo, h.codigo_hospede, h.numero_quarto);
                imprimir_data(h.saida_real);
                printf(" | R$ %.2f | ", h.valor_final);
                imprimir_forma_pagamento(h.forma_pagamento);
                printf("\n");
                total_fat = total_fat + h.valor_final;
                contagem = contagem + 1;
            }
        }
    }
    fclose(f);
    if (contagem == 0) {
        printf("Nenhuma hospedagem finalizada no periodo.\n");
    } else {
        printf("\nHospedagens: %d\n", contagem);
        printf("%sFaturamento total: R$ %.2f%s\n", COR_VERDE, total_fat, COR_RESET);
    }
}

void relatorio_historico_hospede(void) {
    int codigo;
    Hospede hosp;
    FILE *fr, *fh;
    Reserva r;
    Hospedagem h;
    int achou_r = 0, achou_h = 0;

    titulo_secao("HISTORICO DE HOSPEDAGENS DO HOSPEDE");
    codigo = ler_inteiro("Codigo do hospede: ");
    if (!buscar_hospede_por_codigo(codigo, &hosp)) {
        msg_erro("Hospede nao encontrado.");
        return;
    }
    imprimir_hospede(&hosp);

    printf("\n-- Reservas --\n");
    fr = fopen(ARQ_RESERVAS, "rb");
    if (fr != NULL) {
        while (fread(&r, sizeof(Reserva), 1, fr) == 1) {
            if (r.codigo_hospede == codigo) {
                printf("Reserva %d | Quarto %d | ", r.codigo, r.numero_quarto);
                imprimir_data(r.checkin);
                printf(" a ");
                imprimir_data(r.checkout);
                printf(" | ");
                imprimir_status_reserva(r.status);
                printf(" | R$ %.2f\n", r.valor_estimado);
                achou_r = 1;
            }
        }
        fclose(fr);
    }
    if (!achou_r) {
        printf("Nenhuma reserva.\n");
    }

    printf("\n-- Hospedagens --\n");
    fh = fopen(ARQ_HOSPEDAGENS, "rb");
    if (fh != NULL) {
        while (fread(&h, sizeof(Hospedagem), 1, fh) == 1) {
            if (h.codigo_hospede == codigo) {
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
        fclose(fh);
    }
    if (!achou_h) {
        printf("Nenhuma hospedagem.\n");
    }
}

void menu_relatorios(void) {
    int opcao;
    do {
        limpar_tela();
        titulo_menu("CONSULTAS E RELATORIOS");
        item_menu(1, "Reservas por periodo");
        item_menu(2, "Hospedes atualmente hospedados");
        item_menu(3, "Quartos disponiveis em uma data");
        item_menu(4, "Faturamento por periodo");
        item_menu(5, "Historico de um hospede");
        item_menu(0, "Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            relatorio_reservas_periodo();
        } else if (opcao == 2) {
            relatorio_hospedes_atuais();
        } else if (opcao == 3) {
            relatorio_quartos_disponiveis_data();
        } else if (opcao == 4) {
            relatorio_faturamento_periodo();
        } else if (opcao == 5) {
            relatorio_historico_hospede();
        } else if (opcao != 0) {
            msg_aviso("Opcao invalida.");
        }
        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);
}

/* -------------------- main -------------------- */

void menu_principal(void) {
    int opcao;
    do {
        limpar_tela();
        titulo_menu("SISTEMA DE GERENCIAMENTO DE POUSADA");
        printf("  %sControle de hospedes, quartos, reservas e estadias%s\n\n", COR_AZUL, COR_RESET);
        item_menu(1, "Cadastro de Hospedes");
        item_menu(2, "Cadastro de Quartos");
        item_menu(3, "Gerenciar Reservas");
        item_menu(4, "Check-in / Check-out");
        item_menu(5, "Consultas e Relatorios");
        item_menu(0, "Sair");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            menu_hospedes();
        } else if (opcao == 2) {
            menu_quartos();
        } else if (opcao == 3) {
            menu_reservas();
        } else if (opcao == 4) {
            menu_hospedagens();
        } else if (opcao == 5) {
            menu_relatorios();
        } else if (opcao == 0) {
            printf("\n%sAte logo! Obrigado por usar o sistema.%s\n", COR_CIANO, COR_RESET);
        } else {
            msg_aviso("Opcao invalida.");
            pausar();
        }
    } while (opcao != 0);
}

int main(void) {
    menu_principal();
    return 0;
}
