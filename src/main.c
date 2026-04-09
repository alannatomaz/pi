#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <arquivo.csv>\n", argv[0]);
        return 1;
    }

    if (!carregar_csv(argv[1]))
    {
        return 1;
    }

    printf("Registros carregados: %d\n", total_registros);
    return 0;
}