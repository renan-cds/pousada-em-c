# Sistema de Gerenciamento de Pousada (C)

Sistema completo em linguagem C para controle de hóspedes, quartos, reservas e hospedagens de uma pequena pousada, usando apenas **structs** e **arquivos binários** (sem banco de dados).

## Requisitos

- GCC com suporte a C11
- Make

## Compilar e executar

```bash
make
./pousada
```

Ou:

```bash
make run
```

## Testes automatizados

```bash
make test
```

Há também um roteiro interativo em `tests/fluxo_completo.input`:

```bash
rm -f data/*.dat
./pousada < tests/fluxo_completo.input
```

## Estrutura

```
include/     headers (tipos, módulos)
src/         implementação
data/        arquivos .dat gerados em tempo de execução
tests/       testes e roteiro de entrada
```

### Arquivos de dados

| Arquivo | Conteúdo |
|---------|----------|
| `data/hospedes.dat` | Cadastro de hóspedes |
| `data/quartos.dat` | Cadastro de quartos |
| `data/reservas.dat` | Reservas |
| `data/hospedagens.dat` | Check-ins / check-outs |
| `data/contadores.dat` | Próximos códigos automáticos |

## Menu principal

1. Cadastro de Hóspedes  
2. Cadastro de Quartos  
3. Gerenciar Reservas  
4. Check-in / Check-out  
5. Consultas e Relatórios  
0. Sair  

## Regras importantes

- Disponibilidade por período: o dia de check-out libera o quarto para nova entrada (`[check-in, check-out)`).
- Valor estimado = diárias × valor do quarto.
- Exclusão valida se o registro está em uso (reservas/hospedagens).
- CPF único; datas coerentes; capacidade do quarto respeitada.
- Quarto em manutenção não pode ser reservado.

## Relatórios

- Reservas por período  
- Hóspedes atualmente hospedados  
- Quartos disponíveis em uma data  
- Faturamento por período  
- Histórico de um hóspede  
