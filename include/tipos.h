#ifndef TIPOS_H
#define TIPOS_H

#define MAX_NOME 80
#define MAX_CPF 15
#define MAX_TELEFONE 20
#define MAX_EMAIL 80
#define MAX_OBS 200
#define MAX_TIPO_QUARTO 30

#define ARQ_HOSPEDES "data/hospedes.dat"
#define ARQ_QUARTOS "data/quartos.dat"
#define ARQ_RESERVAS "data/reservas.dat"
#define ARQ_HOSPEDAGENS "data/hospedagens.dat"
#define ARQ_CONTADORES "data/contadores.dat"

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
    int ativo; /* 1 = ativo, 0 = excluido (soft delete) */
} Hospede;

typedef struct {
    int numero;
    TipoQuarto tipo;
    float valor_diaria;
    int capacidade;
    StatusQuarto status;
    int ativo;
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
    int ativo;
} Reserva;

typedef struct {
    int codigo;
    int codigo_reserva; /* 0 se check-in direto */
    int codigo_hospede;
    int numero_quarto;
    Data entrada_real;
    Data saida_real; /* dia/mes/ano = 0 se ainda hospedado */
    float valor_final;
    FormaPagamento forma_pagamento;
    char observacoes[MAX_OBS];
    int finalizada; /* 0 = em andamento, 1 = checkout realizado */
    int ativo;
} Hospedagem;

typedef struct {
    int prox_hospede;
    int prox_reserva;
    int prox_hospedagem;
} Contadores;

#endif
