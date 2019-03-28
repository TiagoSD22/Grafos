#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define VERDADEIRO 1
#define FALSO 0

typedef short boolean;

typedef struct vert{
    int valor;
    int grau;
    struct vert* prox;
}vertice;

typedef struct arestc{
    char label[10];
    int origem;
    int destino;
    int custo;
}aresta;

typedef struct grafo{
    int nVertices;
    int nArestas;
    aresta* arestas;
    vertice** vertices;
}Grafo;

Grafo* iniciarGrafo(int nVertices, int nArestas){
    Grafo* g;
    g = (Grafo*) malloc(sizeof(Grafo));
    g->vertices = (vertice**) malloc(nVertices * sizeof(vertice*));
    g->arestas = (aresta*) malloc(nArestas * sizeof(aresta));
    g->nVertices = 0;
    g->nArestas = 0;
}

int encontrarVertice(Grafo** g, int u){
    int i = 0;
    while((*g)->vertices[i]->valor != u)
        i++;
    return i;
}

boolean possuiVertice(Grafo** g, int verticeValor){
    boolean achado = FALSO;
    int i = 0;
    while((!achado) && (i < (*g)->nVertices)){
        if((*g)->vertices[i]->valor == verticeValor){
            achado = VERDADEIRO;
        }
        i++;
    }
    return achado;
}

void adicionarVertice(Grafo** g, int verticeValor){
    vertice* novoVertice = (vertice*) malloc(sizeof(vertice));
    novoVertice->valor = verticeValor;
    novoVertice->prox = NULL;
    novoVertice->grau = 0;
    (*g)->nVertices++;
    (*g)->vertices[(*g)->nVertices - 1] = novoVertice;
}

boolean possuiArco(Grafo** g, int u, int v){
    int i = 0;
    while((*g)->vertices[i]->valor != u)
        i++;
    boolean achada = FALSO;
    vertice* aux = (*g)->vertices[i];
    while((!achada) && (aux->prox != NULL)){
        if(aux->valor == v)
            achada = VERDADEIRO;
        aux = aux->prox;
    }
    return achada;
}

void adicionarArco(Grafo **g, int u, int v, int custoUV){
    int posVerticeU = encontrarVertice(g, u);
    (*g)->vertices[posVerticeU]->grau++;
    vertice *novoVerticeAdjacente = (vertice *)malloc(sizeof(vertice));
    novoVerticeAdjacente->valor = v;
    novoVerticeAdjacente->prox = NULL;
    vertice *aux = (*g)->vertices[posVerticeU];
    while (aux->prox != NULL)
        aux = aux->prox;
    aux->prox = novoVerticeAdjacente;
    aresta novaAresta;
    novaAresta.origem = u;
    novaAresta.destino = v;
    char buffer[4];
    buffer[0] = '\0';
    sprintf(buffer, "%d", (*g)->nArestas);
    strcpy(novaAresta.label,"A");
    strcat(novaAresta.label, buffer);
    novaAresta.custo = custoUV;
    (*g)->arestas[(*g)->nArestas] = novaAresta;
    (*g)->nArestas++;
}

void destruirGrafo(Grafo** g){
    if((*g)->vertices != NULL){
        int i;
        for (i = 0; i < (*g)->nVertices; i++){
            vertice* iterador = (*g)->vertices[i];
            while (iterador->prox != NULL){
                vertice* aux = iterador;
                iterador = iterador->prox;
                free(aux);
            }
        }
        free((*g)->vertices);
    }
    free((*g)->arestas);
    free(*g);
}

void mostrarGrafo(Grafo** g){
    int i;
    printf("Grafo: \n\n");
    for(i = 0; i < (*g)->nVertices; i++){
        printf("V%d: %d ", i, (*g)->vertices[i]->valor);
        vertice* v = (*g)->vertices[i];
        while(v->prox != NULL){
            printf(" -> %d", v->prox->valor);
            v = v->prox;
        }
        printf("| Grau: %d\n",(*g)->vertices[i]->grau);
    }
    printf("\nVertices: %d Arestas: %d\n", (*g)->nVertices, (*g)->nArestas);
    for(i = 0; i < (*g)->nArestas; i++)
        printf("Aresta %s: %d -> %d (Custo: %d)\n\n",(*g)->arestas[i].label, (*g)->arestas[i].origem, (*g)->arestas[i].destino, (*g)->arestas[i].custo);
}

FILE* carregarArquivo(){
    char nomeArquivo[32];
    FILE* arquivo;
    boolean arquivoEncontrado = FALSO;
    do{
        printf("Qual o arquivo voce deseja abrir(utilize a extensao do arquivo ao final. Ex: grafo.txt)?");
        printf("\nArquivo: ");
        fflush(stdin);
        nomeArquivo[0]='\0';
        fgets(nomeArquivo,32,stdin);
        if(nomeArquivo[strlen(nomeArquivo) - 1] == '\n'){
            nomeArquivo[strlen(nomeArquivo) - 1] = '\0';
        }

        arquivo = fopen(nomeArquivo,"r");

        if(arquivo == NULL){
            printf("\nArquivo nao encontrado! Tente novamente!\n\n");
        }
        else{
            arquivoEncontrado = VERDADEIRO;
        }
    }while(!arquivoEncontrado);
    return arquivo;
}

Grafo* montarGrafo(FILE* arquivoGrafo, int* origem, int* destino, boolean* possuiArcoNegativo){
    Grafo* g;
    int nArestas, nVertices, verticeU, verticeV, custoUV,linhasLidas;
    fscanf(arquivoGrafo, "%d %d %d %d", &nVertices, &nArestas, origem, destino);
    g = iniciarGrafo(nVertices, nArestas);
    linhasLidas = 0;
    while(linhasLidas < nArestas){
        fscanf(arquivoGrafo, "%d %d %d", &verticeU, &verticeV, &custoUV);
        if(custoUV < 0)
            *possuiArcoNegativo = VERDADEIRO;
        if(!possuiVertice(&g, verticeU))
            adicionarVertice(&g, verticeU);
        if(!possuiVertice(&g, verticeV))
            adicionarVertice(&g, verticeV);
        if(!possuiArco(&g, verticeU, verticeV))
            adicionarArco(&g, verticeU, verticeV, custoUV);
        linhasLidas++;
    }
    return g;
}

boolean pertence(int** conjunto, int elemento, int tamanhoConjunto){
    int i = 0;
    boolean achada = FALSO;
    while(!achada && i < tamanhoConjunto)
        if((*conjunto)[i] == elemento)
            achada = VERDADEIRO;
    return achada;
}

void Dijkstra(Grafo** g, int origem, int destino, int** anterior, float** custos){
    int i, tamanhoZ;
    int* conjuntoZ;
    for(i = 0; i < (*g)->nVertices; i++){
        (*anterior)[i] = 0;
        (*custos)[i] = INFINITY;
    }
    custos[origem - 1] = 0;
    conjuntoZ = (int*) malloc((*g)->nVertices * sizeof(int));
    conjuntoZ[0] = origem;
    tamanhoZ = 1;
    while(!pertence(&conjuntoZ, destino, tamanhoZ)){

    }
}

int main(int narg, char* argv[]){
    Grafo* g;
    FILE* arquivoGrafo;
    int verticeOrigem, verticeDestino;
    boolean possuiArcoNegativo = FALSO;
    if(narg > 1)
        arquivoGrafo = fopen(argv[1], "r");
    else
        arquivoGrafo = carregarArquivo();
    g = montarGrafo(arquivoGrafo, &verticeOrigem, &verticeDestino, &possuiArcoNegativo);
    if(possuiArcoNegativo){
        printf("\n\nO grafo informado possui pelo menos 1 arco com custo negativo, nao posso calcular o caminho minimo de %d a %d usando o algoritmo de Dijkstra!\n\n",verticeOrigem, verticeDestino);
    } 
    else{
        int* anterior = (int*) malloc(g->nVertices * sizeof(int));
        float* custos = (float*) malloc(g->nVertices * sizeof(float));
        Dijkstra(&g, verticeOrigem, verticeDestino, &anterior, &custos);
    }
    printf("\n\nFim\n\n");
    destruirGrafo(&g);
    return 0;
}
