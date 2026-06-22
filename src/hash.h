#ifndef HASH_H
#define HASH_H

#include "produto.h"

typedef struct NoHash
{
    Produto dado;
    struct NoHash *prox;
} NoHash;

NoHash **criar_tabela_hash(int m);
int hash_func(int id, int m);
void hash_inserir(NoHash **tabela, int m, Produto p, long *colisoes);
int hash_buscar(NoHash **tabela, int m, int id_buscado);
void hash_liberar(NoHash **tabela, int m);

#endif