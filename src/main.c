#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGISTROS 500000

typedef struct
{
    int id;
    char nome[51];
    char categoria[31];
    float valor;
} Produto;

Produto vetor[MAX_REGISTROS];
int total_registros = 0;

int carregar_csv(const char *caminho)
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

    while (fgets(linha, sizeof(linha), fp) != NULL)
    {
        if (total_registros >= MAX_REGISTROS)
        {
            printf("Aviso: limite de registros atingido.\n");
            break;
        }

        Produto p;
        if (sscanf(linha, "%d,%50[^,],%30[^,],%f",
                   &p.id, p.nome, p.categoria, &p.valor) == 4)
        {
            vetor[total_registros] = p;
            total_registros++;
        }
    }

    fclose(fp);
    return 1;
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