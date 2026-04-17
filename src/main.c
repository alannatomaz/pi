#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_BUSCAS_POR_BLOCO 250 // 250 * 4 categorias = 1000 buscas totais
#define N_RODADAS 3

typedef struct {
    int id;
    char nome[51];
    char categoria[31];
    float valor;
} Produto;

// --- FUNÇÃO DE BUSCA SEQUENCIAL ---
int busca_sequencial(Produto *vetor, int total, int id_buscado) {
    for (int i = 0; i < total; i++) {
        if (vetor[i].id == id_buscado) {
            return i; // Retorna o índice onde encontrou
        }
    }
    return -1; // Não encontrado
}

// --- FUNÇÃO PARA CARREGAR O CSV (Alocação Dinâmica) ---
int carregar_csv(const char *caminho, Produto **vetor) {
    FILE *fp = fopen(caminho, "r");
    if (fp == NULL) {
        printf("Erro: nao foi possivel abrir o arquivo '%s'.\n", caminho);
        return 0;
    }

    char linha[256];
    fgets(linha, sizeof(linha), fp); // Pula o cabeçalho

    int total = 0;
    int capacidade = 100;
    *vetor = (Produto *)malloc(capacidade * sizeof(Produto));

    if (*vetor == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (total == capacidade) {
            capacidade *= 2;
            *vetor = (Produto *)realloc(*vetor, capacidade * sizeof(Produto));
        }

        Produto p;
        // Lógica de leitura separada por vírgulas
        if (sscanf(linha, "%d,%50[^,],%30[^,],%f", &p.id, p.nome, p.categoria, &p.valor) == 4) {
            (*vetor)[total] = p;
            total++;
        }
    }

    fclose(fp);
    return total;
}

// --- FUNÇÃO PARA PREPARAR OU CARREGAR AMOSTRAS FIXAS ---
void preparar_amostras(Produto *vetor, int total, int *ids_teste) {
    FILE *f = fopen("amostras.txt", "r");
    
    if (f) {
        // Se o arquivo já existe, carrega os IDs dele
        for (int i = 0; i < 1000; i++) {
            fscanf(f, "%d", &ids_teste[i]);
        }
        fclose(f);
        printf("Amostras carregadas de 'amostras.txt'.\n");
    } else {
        // Se não existe, sorteia e salva para as próximas execuções
        f = fopen("amostras.txt", "w");
        srand(time(NULL)); 
        
        for (int i = 0; i < 1000; i++) {
            if (i < 250)      ids_teste[i] = vetor[rand() % (total / 4)].id;              // Inicio
            else if (i < 500) ids_teste[i] = vetor[total / 4 + rand() % (total / 2)].id;  // Meio
            else if (i < 750) ids_teste[i] = vetor[total * 3 / 4 + rand() % (total / 4)].id; // Final
            else              ids_teste[i] = -999;                                        // Inexistente

            fprintf(f, "%d\n", ids_teste[i]);
        }
        fclose(f);
        printf("Novas amostras geradas e salvas em 'amostras.txt'.\n");
    }
}

// --- FUNÇÃO DE TESTE POR CATEGORIA ---
void executar_teste_bloco(Produto *vetor, int total, int *ids, int inicio_indice, const char *nome_bloco) {
    double soma_tempos = 0;
    printf("\n>>> Testando Bloco: %s\n", nome_bloco);
    printf("------------------------------------------\n");

    for (int r = 0; r < N_RODADAS; r++) {
        clock_t t_inicio = clock();
        
        for (int i = inicio_indice; i < inicio_indice + N_BUSCAS_POR_BLOCO; i++) {
            busca_sequencial(vetor, total, ids[i]);
        }
        
        clock_t t_fim = clock();
        double tempo_total_rodada = (double)(t_fim - t_inicio) / CLOCKS_PER_SEC;
        soma_tempos += tempo_total_rodada;

        printf("Rodada %d: Tempo Total = %.6f s | Media = %.8f s\n", 
               r + 1, tempo_total_rodada, tempo_total_rodada / N_BUSCAS_POR_BLOCO);
    }

    double media_final_bloco = (soma_tempos / N_RODADAS) / N_BUSCAS_POR_BLOCO;
    printf("MEDIA FINAL DO BLOCO %s: %.8f segundos por busca\n", nome_bloco, media_final_bloco);
}

// --- FUNÇÃO PRINCIPAL (Sempre ao final para evitar erros de declaração) ---
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <caminho_do_arquivo.csv>\n", argv[0]);
        return 1;
    }

    Produto *vetor = NULL;
    int total = carregar_csv(argv[1], &vetor);

    if (total <= 0) {
        printf("Erro ao carregar dados.\n");
        return 1;
    }

    printf("Dataset carregado com sucesso! Total de registros: %d\n", total);

    // Alocação dinâmica para o vetor de IDs de teste (1000 inteiros)
    int *ids_teste = (int *)malloc(1000 * sizeof(int));
    if (ids_teste == NULL) {
        free(vetor);
        return 1;
    }

    // Prepara os IDs que serão usados em todos os testes
    preparar_amostras(vetor, total, ids_teste);

    // Executa o protocolo experimental por categoria
    executar_teste_bloco(vetor, total, ids_teste, 0,   "INICIO");
    executar_teste_bloco(vetor, total, ids_teste, 250, "MEIO");
    executar_teste_bloco(vetor, total, ids_teste, 500, "FIM");
    executar_teste_bloco(vetor, total, ids_teste, 750, "INEXISTENTE");

    // Liberação de memória
    free(ids_teste);
    free(vetor);

    printf("\nTestes concluidos com sucesso.\n");
    return 0;
}