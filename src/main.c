#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_BUSCAS 1000
#define N_RODADAS 3

typedef struct
{
    int id;
    char nome[51];
    char categoria[31];
    float valor;
} Produto;

int carregar_csv(const char *caminho, Produto **vetor)
{
    FILE *fp = fopen(caminho, "r");
    if (fp == NULL)
    {
        printf("Erro: nao foi possivel abrir o arquivo '%s'.\n", caminho);
        return 0;
    }

    char linha[256];

    /* Pula o cabecalho */
    fgets(linha, sizeof(linha), fp);

    int total = 0;
    int capacidade = 100;

    *vetor = (Produto *)malloc(capacidade * sizeof(Produto));
    if (*vetor == NULL)
    {
        printf("Erro: falha ao alocar memoria.\n");
        fclose(fp);
        return -1;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL)
    {
        if (total == capacidade)
        {
            capacidade = capacidade * 2;
            *vetor = (Produto *)realloc(*vetor, capacidade * sizeof(Produto));
            if (*vetor == NULL)
            {
                printf("Erro: falha ao realocar memoria.\n");
                fclose(fp);
                return -1;
            }
        }

        Produto p;
        if (sscanf(linha, "%d,%50[^,],%30[^,],%f",
                   &p.id, p.nome, p.categoria, &p.valor) == 4)
        {
            (*vetor)[total] = p;
            total++;
        }
    }

    fclose(fp);
    return total;
}

int busca_sequencial(Produto *vetor, int total, int id_buscado)
{
    int i;
    for (i = 0; i < total; i++)
    {
        if (vetor[i].id == id_buscado)
        {
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    Produto *vetor = NULL;
    int total;
    int r, i;
    double tempo_medio[N_RODADAS];
    double soma_medias = 0.0;
    clock_t inicio, fim;

    if (argc < 2)
    {
        printf("Uso: %s <arquivo.csv>\n", argv[0]);
        return 1;
    }

    printf("Carregando dataset...\n");
    total = carregar_csv(argv[1], &vetor);
    if (total < 0)
    {
        return 1;
    }
    printf("Registros carregados: %d\n\n", total);

    srand(42); /* semente fixa para resultados reproduziveis */

    printf("%-8s  %-20s  %-22s\n", "Rodada", "Tempo Total (s)", "Tempo Medio/Busca (us)");
    printf("%-8s  %-20s  %-22s\n", "------", "---------------", "---------------------");

    for (r = 0; r < N_RODADAS; r++)
    {
        inicio = clock();

        for (i = 0; i < N_BUSCAS; i++)
        {
            int id;
            int regiao = i % 4;

            if (regiao == 0)
            {
                /* Inicio: sorteia entre as primeiras 1/4 posicoes */
                id = vetor[rand() % (total / 4)].id;
            }
            else if (regiao == 1)
            {
                /* Meio: sorteia entre 1/4 e 3/4 */
                id = vetor[total / 4 + rand() % (total / 2)].id;
            }
            else if (regiao == 2)
            {
                /* Final: sorteia entre as ultimas 1/4 posicoes */
                id = vetor[total * 3 / 4 + rand() % (total / 4)].id;
            }
            else
            {
                /* Inexistente */
                id = -999;
            }

            busca_sequencial(vetor, total, id);
        }

        fim = clock();

        double tempo_total = (double)(fim - inicio) / CLOCKS_PER_SEC;
        tempo_medio[r] = tempo_total / N_BUSCAS;
        soma_medias += tempo_medio[r];

        printf("%-8d  %-20.6f  %-22.4f\n",
               r + 1,
               tempo_total,
               tempo_medio[r] * 1e6);
    }

    double media_final = soma_medias / N_RODADAS;

    printf("\n\n\n");
    printf("  Resumo Final\n\n");
    printf("  Registros no vetor  : %d\n", total);
    printf("  Buscas por rodada   : %d\n", N_BUSCAS);
    printf("  Rodadas executadas  : %d\n", N_RODADAS);
    printf("  Tempo medio final   : %.4f microssegundos/busca\n", media_final * 1e6);

    free(vetor);
    return 0;
}