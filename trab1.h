#ifndef TRAB1_H_INCLUDED
#define TRAB1_H_INCLUDED

#define MAX_NO 200
#define MAX_HUB 100
#define NUM_HUBS 50
#define BETA 1.0
#define ALPHA 0.75
#define LAMBDA 1.0

typedef struct No {
    double x;
    double y;
} No;

typedef struct Rota {
    int origem;
    int h1;
    int h2;
    int destino;
    double custo;
} Rota;

typedef struct Solucao {
    double fo;
    int hubs[MAX_HUB];
    Rota rotas[MAX_NO];
} Solucao;

// Dados de entrada
No vet_nos[MAX_NO]; // Vetor com a informação dos nós 
double matriz_distancia[MAX_NO][MAX_NO]; // Matriz com as distâncias entre os pontos
int vet_hubs[MAX_NO];
int vet_nao_hubs[MAX_NO];
int vet_hubs_temporarios[MAX_HUB];
int num_nos;
int num_hubs;
double custo;

// Funções
void ler_arquivo(const char* arq);
void testar_dados(const char* arq);

double calcular_distancia(No a, No b);
void preencher_matriz_distancia();
void imprimir_matriz_distancia();

int encontrar_hub_mais_proximo(int no, int hubs_selecionados[], int qtd_hubs);
double calcular_custo_maximo(int hubs_selecionados[], int qtd_hubs);
int is_hub(int hub);

// Heurística gulosa
void heu_cons_gulosa(int num_hubs, int hubs_final[], double* custo_gulosa);
void salvar_resultados(int hubs_final[], double custo_gulosa);
void ler_solucao_arquivo(const char* arquivo, Solucao* solucao);

#endif //TRAB1_H_INCLUDED