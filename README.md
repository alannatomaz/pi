# Projeto Integrador — Busca Sequencial x Tabela Hash

Trabalho da disciplina Projeto Integrador.

## Estrutura do projeto

- `src/main.c` — programa principal, leitura do dataset e medição da busca sequencial
- `src/produto.h` — struct `Produto` compartilhada entre os arquivos
- `src/hash.h` / `src/hash.c` — implementação da Tabela Hash com tratamento de colisões (encadeamento)
- `data/` — datasets utilizados nos testes
- `amostras.txt` — ids de teste gerados automaticamente (início, meio, final, inexistentes)
- `docs/` — Relatórios (PDF) FASE 1 e FASE 2
- 
## Fase I — Busca Sequencial

Implementação de busca sequencial em vetor, com medição de tempo via `clock()`.

### Compilação

```bash
gcc -o busca src/main.c
```

### Execução

```bash
./busca data/dataset4.csv
```

## Fase II — Tabela Hash

Implementação de Tabela Hash com tratamento de colisões por encadeamento, usando a função hash definida pelo grupo (F3 — Dobra Simples de Dois Blocos):

h(x) = ((x/1000) + (x%1000)) % m

x = id do registro

m = quantidade de registros do dataset

O tamanho da tabela (`m`) é igual ao volume de dados do dataset testado.

### Compilação

```bash
gcc -o busca src/main.c src/hash.c
```

### Execução

```bash
./busca data/dataset4.csv
```

### Resultados experimentais (dataset4.csv)

| Métrica                       | Valor         |
| ----------------------------- | ------------- |
| Total de registros carregados | 400.009       |
| Total de colisões             | 398.611       |
| Tempo total das buscas (Hash) | 0,091000 s    |
| Tempo médio por busca (Hash)  | 0,00003033 s  |

### Comparação Busca Sequencial x Tabela Hash

| Métrica                   | Busca Sequencial | Tabela Hash   |
| ------------------------- | ---------------- | ------------- |
| Tempo médio               | 0.002117 s       | 0,00003033 s  |
| Complexidade teórica      | O(n)             | O(1) amortizado / O(n) pior caso |
| Observações experimentais | _A PREENCHER_    | ~80,91x mais rápida, apesar de 99,6% de colisões |

> A análise interpretativa completa (desempenho teórico x prático, eficiência da função hash, vantagens e limitações) está detalhada no artigo científico do grupo.
