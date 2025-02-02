#include <iostream>
#include <time.h>
#include <memory.h>
#include <cmath>
#include "trab1.h"

using namespace std;

const char *arquivo_saida = "solucao_hubs.txt";
int main(int argc, char* argv[])
{
    // Zerando a matriz de distâncias.
    memset(&matriz_distancia, 0, sizeof(matriz_distancia));

    // Nome do arquivo passado por CLI
    const char* arquivo_entrada = argv[1];
    ler_arquivo(arquivo_entrada);
   
    // testar_dados(arquivo_saida);

    preencher_matriz_distancia();
    //imprimir_matriz_distancia();

    // Numeros de hubs passado por CLI
    num_hubs = atoi(argv[2]);
    
    int hubs_gulosa[MAX_HUB];
    double custo_gulosa;
    // printf("Hubs Gulosa:\n");

    heu_cons_gulosa(num_hubs, hubs_gulosa, &custo_gulosa);

    Solucao solucao_lida, solucao_copiada;
    ler_solucao_arquivo("solucao_hubs.txt", &solucao_lida);

    //clonar_solucao(&solucao_lida, &solucao_copiada);
    
    //imprimir_solucao_lida(solucao_copiada);

    // Calculo da FO a partir do arquivo de entrada.
    double fo = calcular_custo_maximo(solucao_lida.hubs, num_hubs);
    printf("%lf", fo);

    return 0;

}

void ler_arquivo(const char *arq)
{
    FILE *f = fopen(arq, "r");
    fscanf(f, "%d", &num_nos);
    for (int i = 0; i < num_nos; i++)
        fscanf(f, "%lf %lf", &vet_nos[i].x, &vet_nos[i].y);
}

void testar_dados(const char *arq)
{
    FILE *f;
    if (strcmp(arq, "") == 0)
        f = stdout;
    else {
        f = fopen(arq, "w");
        fprintf(f, "%d\n", num_nos);
    }
        
    for (int i = 0; i < num_nos; i++)
        fprintf(f, "%lf %lf\n", vet_nos[i].x, vet_nos[i].y);
}

void preencher_matriz_distancia()
{
    for (int i = 0; i < num_nos; i++)
    {
        // j+1, pois como as distâncias são iguais entre i,j e j,i,
        // não é necessário recalcular distâncias já calculadas anteriormente.
        for (int j = i + 1; j < num_nos; j++)
        {
            double distancia = sqrt(pow((vet_nos[i].x - vet_nos[j].x), 2) + pow((vet_nos[i].y - vet_nos[j].y), 2));
            matriz_distancia[i][j] = distancia;
            matriz_distancia[j][i] = distancia;
        }
    }
}

void imprimir_matriz_distancia()
{
    printf("\n------ Matriz de distâncias: ------\n");
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
            printf("%lf ", matriz_distancia[i][j]);
        printf("\n");
    }
}

void heu_cons_gulosa(int num_hubs, int hubs_final[], double* custo_gulosa) {
    int hubs_selecionados[MAX_HUB]; // Conjunto de hubs selecionados.
    memset(&hubs_selecionados, 0, sizeof(hubs_selecionados));
    int hub_selecionado = 0; // hub selecionado na iteração.

    for(int i = 0; i < num_hubs; i++) { 
        double melhor_custo = __DBL_MAX__;
        int melhor_candidato = -1;
        // Avaliação de todos os nós não selecionados.
        // Todos os nós são candidatos em potencial.
        for(int m = 0; m < num_nos; m++) {
            int is_no_selecionado = 0;
            for(int k = 0; k < hub_selecionado; k++) {
                    if(hubs_selecionados[k] == m) {
                        is_no_selecionado = 1;
                        break;
                    }
                }
                if(is_no_selecionado) continue;

                // Vetor auxiliar para guardar os hubs temporariamente.
                int hubs_temporarios[MAX_HUB];
                for(int h = 0; h < hub_selecionado; h++) 
                    hubs_temporarios[h] = hubs_selecionados[h];
                hubs_temporarios[hub_selecionado] = m;

                // Calculo do custo máximo para este candidato.
                double maior_atual = calcular_custo_maximo(hubs_temporarios, hub_selecionado + 1);

                if(maior_atual < melhor_custo) {
                    melhor_custo = maior_atual;
                    melhor_candidato = m;
                }
        }
        if(melhor_candidato != -1) {
            // Adiciona o hub selecionado ao vetor
            hubs_selecionados[hub_selecionado++] = melhor_candidato;
        }
    }
    // Resultados
    for(int i = 0; i < num_hubs; i++) {
        hubs_final[i] = hubs_selecionados[i]; 
    }
    *custo_gulosa = calcular_custo_maximo(hubs_selecionados, num_hubs);
    salvar_resultados(arquivo_saida, hubs_final, *custo_gulosa);
}

double calcular_custo_maximo(int hubs_selecionados[], int qtd_hubs) {
    double custo_maximo = 0.0;
    int hubs_proximos[MAX_NO];

    // Cálculo dos hubs mais próximos.
    for(int i = 0; i < num_nos; i++) {
        hubs_proximos[i] = encontrar_hub_mais_proximo(i, hubs_selecionados, qtd_hubs);
    }
    // Cálculo do custo máximo
    for(int i = 0; i < num_nos; i++) {
        for(int j = 0; j < num_nos; j++) {
            int hub_i = hubs_proximos[i];
            int hub_j = hubs_proximos[j];
            double custo = BETA * matriz_distancia[i][hub_i] 
                            + ALPHA * matriz_distancia[hub_i][hub_j]
                            + LAMBDA * matriz_distancia[hub_j][j];
            
            

            if(custo > custo_maximo)
                custo_maximo = custo;
        }
    }
    return custo_maximo;
}

int encontrar_hub_mais_proximo(int no, int hubs_selecionados[], int qtd_hubs) {

    double menor_distancia = __DBL_MAX__;
    int mais_proximo = -1;
    for(int i = 0; i < qtd_hubs; i++) {
        double distancia = matriz_distancia[no][hubs_selecionados[i]];
        if(distancia < menor_distancia) {
            menor_distancia = distancia;
            mais_proximo = hubs_selecionados[i];
        }
    }
    return mais_proximo;
}

void salvar_resultados(const char *arquivo_saida, int hubs_final[], double custo_gulosa) {
    FILE *f = fopen(arquivo_saida, "w");
    if (!f) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    if (strcmp(arquivo_saida, "") == 0){
        f = stdout;
    }
    else {
        fprintf(f, "n: %d\tp: %d\n", num_nos, num_hubs);
        fprintf(f, "FO: %.2lf\n", custo_gulosa);
        fprintf(f, "HUBS: [");
        for (int i = 0; i < num_hubs; i++) {
            fprintf(f, "%d%s", hubs_final[i], (i < num_hubs - 1) ? ", " : "");
        }
        fprintf(f, "]\n\n");
        
        fprintf(f, "OR\tH1\tH2\tDS\tCUSTO\n");
        for (int i = 0; i < num_nos; i++) {
            int hub1 = hubs_final[i % num_hubs];
            int hub2 = hubs_final[(i + 1) % num_hubs];
            int destino = (i + 2) % num_nos;
            double custo = BETA * matriz_distancia[i][hub1] 
                            + ALPHA * matriz_distancia[hub1][hub2]
                            + LAMBDA * matriz_distancia[hub2][destino];
            fprintf(f, "%d\t%d\t%d\t%d\t%.2lf\n", i, hub1, hub2, destino, custo);
        }
    }
    fclose(f);
}

void ler_solucao_arquivo(const char* arquivo, Solucao* solucao) {
    FILE* f = fopen(arquivo, "r");
    if (!f) {
        printf("Erro ao abrir o arquivo de solução!\n");
        return;
    }

    // Lê o número de nós e hubs
    fscanf(f, "n: %d p: %d\n", &solucao->qtd_nos, &solucao->qtd_nos);

    // Lê o valor da função objetivo
    fscanf(f, "FO: %lf\n", &solucao->fo);

    // Lê os hubs
    char linha[1000];
    fgets(linha, sizeof(linha), f); // Lê a linha HUBS: [...]
    
    // Inicializa o array de hubs com -1
    memset(solucao->hubs, -1, sizeof(solucao->hubs));
    
    char* token = strtok(linha, "[]"); // Remove HUBS: [
    token = strtok(NULL, "[]"); // Pega o conteúdo dentro dos colchetes
    
    // Parse dos hubs
    int idx = 0;
    token = strtok(token, ",");
    while (token != NULL && idx < MAX_HUB) {
        solucao->hubs[idx++] = atoi(token);
        token = strtok(NULL, ",");
    }

    // Pula a linha em branco e o cabeçalho
    fgets(linha, sizeof(linha), f);
    fgets(linha, sizeof(linha), f);

    // Lê as rotas
    int i = 0;
    while (fscanf(f, "%d %d %d %d %lf\n", 
                  &solucao->rotas[i].origem,
                  &solucao->rotas[i].h1,
                  &solucao->rotas[i].h2,
                  &solucao->rotas[i].destino,
                  &solucao->rotas[i].custo) == 5) {
        i++;
        if (i >= MAX_NO) break;
    }

    fclose(f);
}

void imprimir_solucao_lida(Solucao& solucao_lida) {
    // Imprime a solução lida para verificar
    printf("n: %d\tp: %d\n", solucao_lida.qtd_nos, solucao_lida.qtd_hubs);
    printf("FO: %.2lf\n", solucao_lida.fo);
    printf("HUBS: [");
    for (int i = 0; i < num_hubs; i++) {
        if (solucao_lida.hubs[i] == -1) break;
        printf("%d%s", solucao_lida.hubs[i], (i < num_hubs - 1) ? ", " : "");
    }
    printf("]\n\n");
    
    printf("Algumas rotas lidas:\n");
    printf("OR\tH1\tH2\tDS\tCUSTO\n");
    for (int i = 0; i < num_nos; i++) { // Imprime as 5 primeiras rotas como exemplo
        printf("%d\t%d\t%d\t%d\t%.2lf\n", 
               solucao_lida.rotas[i].origem,
               solucao_lida.rotas[i].h1,
               solucao_lida.rotas[i].h2,
               solucao_lida.rotas[i].destino,
               solucao_lida.rotas[i].custo);
    }
}

void clonar_solucao(Solucao* de, Solucao* para) {
    memcpy(para, de, sizeof(Solucao));
}