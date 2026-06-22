#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "produto.h"
#include "hash.h"

#define N_BUSCAS_POR_BLOCO 250
#define N_RODADAS 3

int busca_sequencial(Produto *vetor, int total, int id_buscado)
{
    for (int i = 0; i < total; i++)
    {
        if (vetor[i].id == id_buscado)
        {
            return i;
        }
    }
    return -1;
}

int carregar_csv(const char *caminho, Produto **vetor)
{
    FILE *fp = fopen(caminho, "r");
    if (fp == NULL)
    {
        printf("Erro: nao foi possivel abrir o arquivo '%s'.\n", caminho);
        return 0;
    }

    char linha[256];
    fgets(linha, sizeof(linha), fp);

    int total = 0;
    int capacidade = 100;
    *vetor = (Produto *)malloc(capacidade * sizeof(Produto));

    if (*vetor == NULL)
    {
        fclose(fp);
        return -1;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL)
    {
        if (total == capacidade)
        {
            capacidade *= 2;
            *vetor = (Produto *)realloc(*vetor, capacidade * sizeof(Produto));
        }

        Produto p;
        if (sscanf(linha, "%d,%50[^,],%30[^,],%f", &p.id, p.nome, p.categoria, &p.valor) == 4)
        {
            (*vetor)[total] = p;
            total++;
        }
    }

    fclose(fp);
    return total;
}

void preparar_amostras(Produto *vetor, int total, int *ids_teste)
{
    FILE *f = fopen("amostras.txt", "r");

    if (f)
    {
        for (int i = 0; i < 1000; i++)
        {
            fscanf(f, "%d", &ids_teste[i]);
        }
        fclose(f);
        printf("Amostras carregadas de 'amostras.txt'.\n");
    }
    else
    {
        f = fopen("amostras.txt", "w");
        srand(time(NULL));

        for (int i = 0; i < 1000; i++)
        {
            if (i < 250)
                ids_teste[i] = vetor[rand() % (total / 4)].id;
            else if (i < 500)
                ids_teste[i] = vetor[total / 4 + rand() % (total / 2)].id;
            else if (i < 750)
                ids_teste[i] = vetor[total * 3 / 4 + rand() % (total / 4)].id;
            else
                ids_teste[i] = -999;

            fprintf(f, "%d\n", ids_teste[i]);
        }
        fclose(f);
        printf("Novas amostras geradas e salvas em 'amostras.txt'.\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <caminho_do_arquivo.csv>\n", argv[0]);
        return 1;
    }

    Produto *vetor = NULL;
    int total = carregar_csv(argv[1], &vetor);

    if (total <= 0)
    {
        printf("Erro ao carregar dados.\n");
        return 1;
    }

    printf("Dataset carregado com sucesso! Total de registros: %d\n\n", total);

    /* ---- TESTE TEMPORARIO DA TABELA HASH ---- */
    int m = total;
    NoHash **tabela = criar_tabela_hash(m);
    long colisoes = 0;

    for (int i = 0; i < total; i++)
    {
        hash_inserir(tabela, m, vetor[i], &colisoes);
    }

    printf("Tabela hash carregada: %d registros, %ld colisoes\n", total, colisoes);
    printf("Teste de busca id=1: %d\n", hash_buscar(tabela, m, 1));
    printf("Teste de busca id=999999 (inexistente): %d\n\n", hash_buscar(tabela, m, 999999));

    hash_liberar(tabela, m);
    /* ---- FIM DO TESTE TEMPORARIO ---- */

    int *ids_teste = (int *)malloc(1000 * sizeof(int));
    if (ids_teste == NULL)
    {
        free(vetor);
        return 1;
    }

    preparar_amostras(vetor, total, ids_teste);

    double tempo_rodada[N_RODADAS];
    double media_inicio[N_RODADAS];
    double media_meio[N_RODADAS];
    double media_final[N_RODADAS];
    double media_inexistente[N_RODADAS];

    int r, i;
    for (r = 0; r < N_RODADAS; r++)
    {
        printf("\n==========================================\n");
        printf("  Rodada %d\n", r + 1);
        printf("==========================================\n");

        clock_t t_ini, t_fim;
        double t_bloco;

        /* Inicio */
        t_ini = clock();
        for (i = 0; i < N_BUSCAS_POR_BLOCO; i++)
            busca_sequencial(vetor, total, ids_teste[i]);
        t_fim = clock();
        t_bloco = (double)(t_fim - t_ini) / CLOCKS_PER_SEC;
        media_inicio[r] = t_bloco / N_BUSCAS_POR_BLOCO;
        printf("  Inicio      : %d buscas | Media: %.6f s/busca\n", N_BUSCAS_POR_BLOCO, media_inicio[r]);

        /* Meio */
        t_ini = clock();
        for (i = 250; i < 250 + N_BUSCAS_POR_BLOCO; i++)
            busca_sequencial(vetor, total, ids_teste[i]);
        t_fim = clock();
        t_bloco = (double)(t_fim - t_ini) / CLOCKS_PER_SEC;
        media_meio[r] = t_bloco / N_BUSCAS_POR_BLOCO;
        printf("  Meio        : %d buscas | Media: %.6f s/busca\n", N_BUSCAS_POR_BLOCO, media_meio[r]);

        /* Final */
        t_ini = clock();
        for (i = 500; i < 500 + N_BUSCAS_POR_BLOCO; i++)
            busca_sequencial(vetor, total, ids_teste[i]);
        t_fim = clock();
        t_bloco = (double)(t_fim - t_ini) / CLOCKS_PER_SEC;
        media_final[r] = t_bloco / N_BUSCAS_POR_BLOCO;
        printf("  Final       : %d buscas | Media: %.6f s/busca\n", N_BUSCAS_POR_BLOCO, media_final[r]);

        /* Inexistente */
        t_ini = clock();
        for (i = 750; i < 750 + N_BUSCAS_POR_BLOCO; i++)
            busca_sequencial(vetor, total, ids_teste[i]);
        t_fim = clock();
        t_bloco = (double)(t_fim - t_ini) / CLOCKS_PER_SEC;
        media_inexistente[r] = t_bloco / N_BUSCAS_POR_BLOCO;
        printf("  Inexistente : %d buscas | Media: %.6f s/busca\n", N_BUSCAS_POR_BLOCO, media_inexistente[r]);

        tempo_rodada[r] = (media_inicio[r] + media_meio[r] + media_final[r] + media_inexistente[r]) * N_BUSCAS_POR_BLOCO;
        printf("------------------------------------------\n");
        printf("  Tempo total da rodada : %.6f s\n", tempo_rodada[r]);
        printf("  Media por busca       : %.6f s\n", tempo_rodada[r] / (N_BUSCAS_POR_BLOCO * 4));
    }

    /* Resumo final */
    double soma_rodadas = 0;
    for (r = 0; r < N_RODADAS; r++)
        soma_rodadas += tempo_rodada[r];

    printf("\n==========================================\n");
    printf("  Resumo Final\n");
    printf("==========================================\n");
    for (r = 0; r < N_RODADAS; r++)
        printf("  Media de tempo - Rodada %d : %.6f s/busca\n", r + 1, tempo_rodada[r] / (N_BUSCAS_POR_BLOCO * 4));
    printf("  Media geral               : %.6f s/busca\n", (soma_rodadas / N_RODADAS) / (N_BUSCAS_POR_BLOCO * 4));
    printf("==========================================\n");

    free(ids_teste);
    free(vetor);

    printf("\nTestes concluidos com sucesso.\n");
    return 0;
}