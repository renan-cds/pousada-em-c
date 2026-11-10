# Sistema de Gerenciamento de Pousada (C)

Sistema de pousada em **Linguagem C**, alinhado ao conteúdo de **Algoritmos II**:
`struct`, `typedef`, `enum`, funções, menus, `string.h`, `fgets`/`scanf`
com limpeza de buffer (`getchar`), e arquivos binários (`fopen`, `fread`, `fwrite`,
`fseek`, `feof`, `remove`, `rename`).

Um único arquivo-fonte: `pousada.c`.

## Compilar e executar

```bash
gcc -Wall -o pousada pousada.c
./pousada
```

Ou:

```bash
make
make run
```

## Arquivos gerados

Na pasta do programa:

| Arquivo | Conteúdo |
|---------|----------|
| `hospedes.dat` | Hóspedes |
| `quartos.dat` | Quartos |
| `reservas.dat` | Reservas |
| `hospedagens.dat` | Hospedagens |

Códigos automáticos: maior código do arquivo + 1.  
Exclusão física: temporário + `remove` + `rename`.

## Menu

1. Cadastro de Hóspedes  
2. Cadastro de Quartos  
3. Gerenciar Reservas  
4. Check-in / Check-out  
5. Consultas e Relatórios  
0. Sair  

## Testes

```bash
make test
rm -f *.dat
./pousada < tests/fluxo_completo.input
```

## Observações de sala

- Datas no estilo `dma` (dias desde 1900 + bissexto), como na L02  
- Digito de CPF checado por ASCII (`'0'`..`'9'`)  
- `ler_data` preenche via ponteiro; códigos = maior do arquivo + 1  
- Disponibilidade por período `[check-in, check-out)`  
- Valor = diárias × valor do quarto  
- Compilação: `gcc -Wall -o pousada pousada.c`
