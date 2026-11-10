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

## Regra de disponibilidade (diferencial)

Período no formato **[check-in, check-out)**: o dia da saída já libera o quarto.

Exemplo: reserva 10/11 a 12/11 ocupa os dias 10 e 11; em 12/11 outra reserva pode entrar.

## Roteiro rápido de demonstração

1. Cadastrar 2 hóspedes e 3 quartos  
2. Reservar quarto 101 de 10/11 a 12/11  
3. Tentar reservar o mesmo quarto em 11/11–13/11 → deve falhar  
4. Verificar disponibilidade em 12/11–14/11 → livre  
5. Check-in da reserva → check-out → ver faturamento  
6. (Opcional) Marcar no-show em outra reserva ativa  

## Observações de sala

- Datas no estilo `dma` (dias desde 1900 + bissexto), como na L02  
- Digito de CPF checado por ASCII (`'0'`..`'9'`)  
- `ler_data` preenche via ponteiro; códigos = maior do arquivo + 1  
- Exclusão de hóspede só bloqueia com reserva **ativa** ou hospedagem **em curso**  
- No-show libera o quarto (status da reserva)  
- `scanf` validado (pede de novo se a leitura falhar)  
- Compilação: `gcc -Wall -o pousada pousada.c`  
- No laboratório Windows: recompile e regenere os `.dat` na máquina da apresentação
