#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

int hash_func(int id, int m)
{
    return ((id / 1000) + (id % 1000)) % m;
}

NoHash **criar_tabela_hash(int m)
{
    NoHash **tabela = (NoHash **)calloc(m, sizeof(NoHash *));
    return tabela;
}

void hash_inserir(NoHash **tabela, int m, Produto p, long *colisoes)
{
    int idx = hash_func(p.id, m);

    if (tabela[idx] != NULL)
    {
        (*colisoes)++;
    }

    NoHash *novo = (NoHash *)malloc(sizeof(NoHash));
    novo->dado = p;
    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

int hash_buscar(NoHash **tabela, int m, int id_buscado)
{
    int idx = hash_func(id_buscado, m);

    NoHash *atual = tabela[idx];
    while (atual != NULL)
    {
        if (atual->dado.id == id_buscado)
        {
            return atual->dado.id;
        }
        atual = atual->prox;
    }

    return -1;
}

void hash_liberar(NoHash **tabela, int m)
{
    for (int i = 0; i < m; i++)
    {
        NoHash *atual = tabela[i];
        while (atual != NULL)
        {
            NoHash *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(tabela);
}