#include <stdio.h>
#include <string.h>
#include "arquivo.h"
#include "util.h"

FILE *abrir_arquivo(const char *caminho, const char *modo) {
    FILE *f = fopen(caminho, modo);
    if (f == NULL && (strcmp(modo, "rb+") == 0 || strcmp(modo, "r+b") == 0)) {
        f = fopen(caminho, "wb+");
        if (f != NULL) {
            fclose(f);
            f = fopen(caminho, "rb+");
        }
    }
    return f;
}

long contar_registros(const char *caminho, size_t tamanho_registro) {
    FILE *f = fopen(caminho, "rb");
    long bytes;
    if (f == NULL) {
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return 0;
    }
    bytes = ftell(f);
    fclose(f);
    if (bytes <= 0 || tamanho_registro == 0) {
        return 0;
    }
    return bytes / (long)tamanho_registro;
}

int ler_registro_pos(const char *caminho, size_t tamanho, long pos, void *destino) {
    FILE *f = fopen(caminho, "rb");
    if (f == NULL) {
        return 0;
    }
    if (fseek(f, pos * (long)tamanho, SEEK_SET) != 0) {
        fclose(f);
        return 0;
    }
    if (fread(destino, tamanho, 1, f) != 1) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

int escrever_registro_pos(const char *caminho, size_t tamanho, long pos, const void *origem) {
    FILE *f = abrir_arquivo(caminho, "rb+");
    if (f == NULL) {
        return 0;
    }
    if (fseek(f, pos * (long)tamanho, SEEK_SET) != 0) {
        fclose(f);
        return 0;
    }
    if (fwrite(origem, tamanho, 1, f) != 1) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

int acrescentar_registro(const char *caminho, size_t tamanho, const void *origem) {
    FILE *f = fopen(caminho, "ab");
    if (f == NULL) {
        return 0;
    }
    if (fwrite(origem, tamanho, 1, f) != 1) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

Contadores carregar_contadores(void) {
    Contadores c = {1, 1, 1};
    FILE *f = fopen(ARQ_CONTADORES, "rb");
    if (f == NULL) {
        salvar_contadores(c);
        return c;
    }
    if (fread(&c, sizeof(Contadores), 1, f) != 1) {
        c.prox_hospede = 1;
        c.prox_reserva = 1;
        c.prox_hospedagem = 1;
    }
    fclose(f);
    return c;
}

void salvar_contadores(Contadores c) {
    FILE *f = fopen(ARQ_CONTADORES, "wb");
    if (f == NULL) {
        return;
    }
    fwrite(&c, sizeof(Contadores), 1, f);
    fclose(f);
}

int gerar_codigo_hospede(void) {
    Contadores c = carregar_contadores();
    int codigo = c.prox_hospede++;
    salvar_contadores(c);
    return codigo;
}

int gerar_codigo_reserva(void) {
    Contadores c = carregar_contadores();
    int codigo = c.prox_reserva++;
    salvar_contadores(c);
    return codigo;
}

int gerar_codigo_hospedagem(void) {
    Contadores c = carregar_contadores();
    int codigo = c.prox_hospedagem++;
    salvar_contadores(c);
    return codigo;
}
