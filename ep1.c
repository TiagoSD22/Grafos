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
    int indice;
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

//rotinas para manipulação do heap
typedef struct heap{
    aresta* arestas;
    int tamanho;
    int capacidade;
}HeapMinimo;

HeapMinimo* CriarHeap(int espaco){
    HeapMinimo* h = (HeapMinimo*) malloc(sizeof(HeapMinimo));
    h->tamanho=0;
    h->capacidade = espaco;
    h->arestas = (aresta*) malloc(espaco * sizeof(aresta));
    return h;
}

boolean heapVazia(HeapMinimo* h){
    return h->tamanho == 0;
}

void PercolacaoAscendente(HeapMinimo* h, int index){
    aresta aux;
    int posicaoPai = (index - 1) / 2;

    if(h->arestas[posicaoPai].custo > h->arestas[index].custo){
        aux = h->arestas[posicaoPai];
        h->arestas[posicaoPai] = h->arestas[index];
        h->arestas[index] = aux;
        PercolacaoAscendente(h, posicaoPai);
    }
}

void PercolacaoDescendente(HeapMinimo* h, int posicaoPai){
    int filhoEsquerdo = posicaoPai*2 + 1;
    int filhoDireito = posicaoPai*2 + 2;
    int min;
    aresta aux;

    if(filhoEsquerdo >= h->tamanho || filhoEsquerdo <0)
        filhoEsquerdo = -1;
    if(filhoDireito >= h->tamanho || filhoDireito <0)
        filhoDireito = -1;

    if(filhoEsquerdo != -1 && h->arestas[filhoEsquerdo].custo < h->arestas[posicaoPai].custo)
        min = filhoEsquerdo;
    else
        min = posicaoPai;

    if(filhoDireito != -1 && h->arestas[filhoDireito].custo < h->arestas[min].custo)
        min = filhoDireito;

    if(min != posicaoPai){
        aux = h->arestas[min];
        h->arestas[min] = h->arestas[posicaoPai];
        h->arestas[posicaoPai] = aux;

        PercolacaoDescendente(h, min);
    }
}

void inserirHeap(HeapMinimo* h, aresta nova){
    if( h->tamanho < h->capacidade){
        h->arestas[h->tamanho] = nova;
        PercolacaoAscendente(h, h->tamanho);
        h->tamanho++;
    }
}

void removerHeap(HeapMinimo* h, aresta remover){
    int i,posicaoPai;
    boolean achado = FALSO;
    i = 0;
    while(!achado && i < h->tamanho){
        if(strcmp(h->arestas[i].label, remover.label) == 0)
            achado = VERDADEIRO;
        i++;
    }
    i--;
    h->arestas[i] = h->arestas[h->tamanho - 1];
    h->tamanho--;
    posicaoPai = i / 2;
    if(i == 0 || h->arestas[posicaoPai].custo < h->arestas[i].custo)
        PercolacaoDescendente(h, i);
    else
        PercolacaoAscendente(h, i);
}

aresta extrairMinimo(HeapMinimo* h){
    aresta min;
    min = h->arestas[0];
    h->arestas[0] = h->arestas[h->tamanho-1];
    h->tamanho--;
    PercolacaoDescendente(h, 0);
    return min;
}
//fim das rotinas para manipulação do heap

Grafo* iniciarGrafo(int nVertices, int nArestas){
    Grafo* g;
    g = (Grafo*) malloc(sizeof(Grafo));
    g->vertices = (vertice**) malloc(nVertices * sizeof(vertice*));
    g->arestas = (aresta*) malloc(nArestas * sizeof(aresta));
    g->nVertices = 0;
    g->nArestas = 0;
}

int encontrarVertice(Grafo** g, int verticeValor){
    int i = 0;
    boolean achada = FALSO;
    while(!achada && i < (*g)->nVertices){
        if((*g)->vertices[i]->valor == verticeValor)
            achada = VERDADEIRO;
        else 
            i++;
    }
    if(achada)
        return i;
    return -1;
}

boolean possuiVertice(Grafo** g, int verticeValor){
    return encontrarVertice(g, verticeValor) != -1;
}

void adicionarVertice(Grafo** g, int verticeValor){
    vertice* novoVertice = (vertice*) malloc(sizeof(vertice));
    novoVertice->valor = verticeValor;
    novoVertice->prox = NULL;
    novoVertice->grau = 0;
    novoVertice->indice = (*g)->nVertices;
    (*g)->nVertices++;
    (*g)->vertices[(*g)->nVertices - 1] = novoVertice;
}

boolean possuiArco(Grafo** g, int u, int v){
    int i = 0;
    boolean achada = FALSO;
    while(!achada && i < (*g)->nArestas){
        if((*g)->arestas[i].origem == u && (*g)->arestas[i].destino == v)
            achada = VERDADEIRO;
        i++;
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
    while(!achada && i < tamanhoConjunto){
        if((*conjunto)[i] == elemento)
            achada = VERDADEIRO;
        i++;
    }
    return achada;
}

void atualizarHeap(HeapMinimo* h, int* conjuntoZ, int tamanhoZ, Grafo** g){
    int i,j;
    for(i = 0; i < h->tamanho; i++){
        if(pertence(&conjuntoZ, h->arestas[i].origem, tamanhoZ) && pertence(&conjuntoZ, h->arestas[i].destino, tamanhoZ)){
            removerHeap(h, h->arestas[i]);
        }
    }

    i = j = 0;
    while(i < (*g)->vertices[encontrarVertice(g, conjuntoZ[tamanhoZ - 1])]->grau){
        if((*g)->arestas[j].origem == conjuntoZ[tamanhoZ - 1]){
            if(!pertence(&conjuntoZ, (*g)->arestas[j].destino, tamanhoZ)){
                inserirHeap(h, (*g)->arestas[j]);
            }
            i++;
        }
        j++;
    }
}

void Dijkstra(Grafo** g, vertice* origem, vertice* destino, int** anterior, float** custos){
    int i, j, tamanhoZ;
    int* conjuntoZ;
    aresta minima;
    for(i = 0; i < (*g)->nVertices; i++){
        (*anterior)[(*g)->vertices[i]->indice] = 0;
        (*custos)[(*g)->vertices[i]->indice] = INFINITY;
    }
    (*custos)[origem->indice] = 0;
    conjuntoZ = (int*) malloc((*g)->nVertices * sizeof(int));
    conjuntoZ[0] = origem->valor;
    tamanhoZ = 1;
    i = j = 0;
    HeapMinimo* h = CriarHeap((*g)->nArestas);
    while(i < origem->grau){
        if((*g)->arestas[j].origem == origem->valor){
            inserirHeap(h, (*g)->arestas[j]);
            i++;
        }
        j++;
    }
    while(!heapVazia(h) && !pertence(&conjuntoZ, destino->valor, tamanhoZ)){
        minima = extrairMinimo(h);
        conjuntoZ[tamanhoZ] = minima.destino;
        tamanhoZ++;
        (*custos)[encontrarVertice(g, minima.destino)] = (*custos)[encontrarVertice(g, minima.origem)] + minima.custo;
        (*anterior)[encontrarVertice(g, minima.destino)] = minima.origem;
        atualizarHeap(h, conjuntoZ, tamanhoZ, g);
    }
    free(conjuntoZ);
    free(h);
}

void mostrarCaminho(Grafo** g, int verticeOrigem, int verticeDestino, int** anterior, float** custos){
    int vAtual, i, tamanhoCaminho;
    int aux[(*g)->nVertices];
    vAtual = verticeDestino;
    tamanhoCaminho = i = 0;
    while(vAtual != verticeOrigem){
        aux[i] = vAtual;
        vAtual = (*anterior)[encontrarVertice(g, vAtual)];
        i++;
    }
    i++;
    tamanhoCaminho = i;
    aux[i - 1] = vAtual;
    printf("\n\nCaminho minimo do vertice %d para o vertice %d: [", verticeOrigem, verticeDestino);
    for(i = tamanhoCaminho - 1; i > 0; i--){
        printf("%d -> ", aux[i]);
    }
    printf("%d]\nCusto do caminho: %d. Tamanho do caminho: %d vertices.\n\n", aux[0], (int)((*custos)[encontrarVertice(g, verticeDestino)]), tamanhoCaminho);
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
        Dijkstra(&g, g->vertices[encontrarVertice(&g, verticeOrigem)], g->vertices[encontrarVertice(&g, verticeDestino)], &anterior, &custos);
        if(custos[g->vertices[encontrarVertice(&g, verticeDestino)]->indice] == INFINITY){
            printf("\n\nO custo para ir de %d a %d no grafo é INFINITO, pois nao existe caminho entre tais vertices!\n\n", verticeOrigem, verticeDestino);
        }
        else{
            mostrarCaminho(&g, verticeOrigem, verticeDestino, &anterior, &custos);
        }
        free(anterior);
        free(custos);
    }
    destruirGrafo(&g);
    return 0;
}
