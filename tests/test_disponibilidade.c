/*
 * Teste das regras de data e disponibilidade (perfil Algoritmos II).
 * Compilar: gcc -Wall -o test_disponibilidade tests/test_disponibilidade.c
 */

#include <stdio.h>

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

typedef enum {
    RESERVA_ATIVA = 1,
    RESERVA_CANCELADA
} StatusReserva;

typedef struct {
    int codigo;
    int numero_quarto;
    Data checkin;
    Data checkout;
    StatusReserva status;
} Reserva;

int ano_bissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int data_para_dias(Data d) {
    int dias_mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int total = 0;
    int a, m;
    for (a = 1900; a < d.ano; a++) {
        total = total + (ano_bissexto(a) ? 366 : 365);
    }
    if (ano_bissexto(d.ano)) {
        dias_mes[2] = 29;
    }
    for (m = 1; m < d.mes; m++) {
        total = total + dias_mes[m];
    }
    return total + d.dia;
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

int periodos_sobrepoem(Data a_ini, Data a_fim, Data b_ini, Data b_fim) {
    return comparar_datas(a_ini, b_fim) < 0 && comparar_datas(b_ini, a_fim) < 0;
}

int quarto_livre(Reserva *reservas, int n, int numero, Data ini, Data fim, int ignorar) {
    int i;
    for (i = 0; i < n; i++) {
        if (reservas[i].status != RESERVA_ATIVA) {
            continue;
        }
        if (reservas[i].numero_quarto != numero) {
            continue;
        }
        if (ignorar > 0 && reservas[i].codigo == ignorar) {
            continue;
        }
        if (periodos_sobrepoem(ini, fim, reservas[i].checkin, reservas[i].checkout)) {
            return 0;
        }
    }
    return 1;
}

Data d(int dia, int mes, int ano) {
    Data x;
    x.dia = dia;
    x.mes = mes;
    x.ano = ano;
    return x;
}

int falhas = 0;

void ok(char *nome, int cond) {
    if (cond) {
        printf("[OK] %s\n", nome);
    } else {
        printf("[FALHA] %s\n", nome);
        falhas = falhas + 1;
    }
}

int main(void) {
    Reserva reservas[2];
    float valor;

    printf("=== Testes (perfil sala) ===\n\n");

    ok("dias_entre 2 diarias", dias_entre(d(10, 11, 2026), d(12, 11, 2026)) == 2);
    ok("comparar_datas ordem", comparar_datas(d(10, 11, 2026), d(12, 11, 2026)) < 0);
    ok("bissexto 2024", ano_bissexto(2024));
    ok("nao bissexto 2025", !ano_bissexto(2025));

    valor = 150.0f * (float)dias_entre(d(10, 11, 2026), d(12, 11, 2026));
    ok("valor 2*150", valor > 299.0f && valor < 301.0f);

    reservas[0].codigo = 1;
    reservas[0].numero_quarto = 101;
    reservas[0].checkin = d(10, 11, 2026);
    reservas[0].checkout = d(12, 11, 2026);
    reservas[0].status = RESERVA_ATIVA;

    ok("bloqueia sobreposicao",
       !quarto_livre(reservas, 1, 101, d(11, 11, 2026), d(13, 11, 2026), 0));
    ok("libera no checkout",
       quarto_livre(reservas, 1, 101, d(12, 11, 2026), d(14, 11, 2026), 0));
    ok("livre antes",
       quarto_livre(reservas, 1, 101, d(8, 11, 2026), d(10, 11, 2026), 0));
    ok("ignora propria reserva",
       quarto_livre(reservas, 1, 101, d(10, 11, 2026), d(12, 11, 2026), 1));

    reservas[0].status = RESERVA_CANCELADA;
    ok("livre apos cancelamento",
       quarto_livre(reservas, 1, 101, d(10, 11, 2026), d(12, 11, 2026), 0));

    printf("\n=== Resultado: %d falha(s) ===\n", falhas);
    return falhas == 0 ? 0 : 1;
}
