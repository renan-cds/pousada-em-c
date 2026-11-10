# Sistema de Gerenciamento de Pousada (C)

Sistema de pousada em **Linguagem C** para **Windows (Dev-C++ / MinGW)**, alinhado ao conteúdo de **Algoritmos II**.

Arquivo principal: `pousada.c`

## Compilar e executar (Windows)

No Dev-C++: abra `pousada.c` e compile/execute (F11 / F10).

Pelo terminal (MinGW):

```bat
gcc -Wall -o pousada.exe pousada.c
pousada.exe
```

## Interface Windows

- `conio.h` → `getch()` (pressione qualquer tecla)
- `system("cls")` → limpa a tela
- Cores ANSI no `printf` (melhor no **Windows Terminal** ou Prompt do Windows 10+)

## Arquivos gerados

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

## Regra de disponibilidade

Período **[check-in, check-out)**: o dia da saída já libera o quarto.

Exemplo: reserva 10/11 a 12/11 ocupa 10 e 11; em 12/11 outra reserva pode entrar.

## Roteiro rápido de demonstração

1. Cadastrar 2 hóspedes e 3 quartos  
2. Reservar quarto 101 de 10/11 a 12/11  
3. Tentar o mesmo quarto em 11/11–13/11 → deve falhar  
4. Verificar disponibilidade em 12/11–14/11 → livre  
5. Check-in → check-out → faturamento  
6. (Opcional) Marcar no-show em outra reserva ativa  

## Observações de sala

- Datas no estilo `dma` (dias desde 1900 + bissexto)  
- CPF por ASCII (`'0'`..`'9'`)  
- `ler_data` via ponteiro; códigos = maior + 1  
- Exclusão de hóspede só com reserva ativa ou hospedagem em curso  
- No-show libera o quarto  
- Regenere os `.dat` na máquina da apresentação (Windows)
