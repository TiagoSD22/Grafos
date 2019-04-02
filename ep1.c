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

typedef struct arc{
    char label[10];
    int origem;
    int destino;
    int custo;
}arco;

typedef struct grafo{
    int nVertices;
    int nArcos;
    arco* arcos;
    vertice** vertices;
}Grafo;

//rotinas para manipulação do heap
typedef struct heap{
    arco* arcos;
    int tamanho;
    int capacidade;
}HeapMinimo;

HeapMinimo* CriarHeap(int espaco){
    HeapMinimo* h = (HeapMinimo*) malloc(sizeof(HeapMinimo));
    h->tamanho=0;
    h->capacidade = espaco;
    h->arcos = (arco*) malloc(espaco * sizeof(arco));
    return h;
}

boolean heapVazia(HeapMinimo* h){
    return h->tamanho == 0;
}

void PercolacaoAscendente(HeapMinimo* h, int index){
    arco aux;
    int posicaoPai = (index - 1) / 2;

    if(h->arcos[posicaoPai].custo > h->arcos[index].custo){
        aux = h->arcos[posicaoPai];
        h->arcos[posicaoPai] = h->arcos[index];
        h->arcos[index] = aux;
        PercolacaoAscendente(h, posicaoPai);
    }
}

void PercolacaoDescendente(HeapMinimo* h, int posicaoPai){
    int filhoEsquerdo = posicaoPai*2 + 1;
    int filhoDireito = posicaoPai*2 + 2;
    int min;
    arco aux;

    if(filhoEsquerdo >= h->tamanho || filhoEsquerdo <0)
        filhoEsquerdo = -1;
    if(filhoDireito >= h->tamanho || filhoDireito <0)
        filhoDireito = -1;

    if(filhoEsquerdo != -1 && h->arcos[filhoEsquerdo].custo < h->arcos[posicaoPai].custo)
        min = filhoEsquerdo;
    else
        min = posicaoPai;

    if(filhoDireito != -1 && h->arcos[filhoDireito].custo < h->arcos[min].custo)
        min = filhoDireito;

    if(min != posicaoPai){
        aux = h->arcos[min];
        h->arcos[min] = h->arcos[posicaoPai];
        h->arcos[posicaoPai] = aux;

        PercolacaoDescendente(h, min);
    }
}

void inserirHeap(HeapMinimo* h, arco nova){
    if( h->tamanho < h->capacidade){
        h->arcos[h->tamanho] = nova;
        PercolacaoAscendente(h, h->tamanho);
        h->tamanho++;
    }
}

void removerHeap(HeapMinimo* h, arco remover){
    int i,posicaoPai;
    boolean achado = FALSO;
    i = 0;
    while(!achado && i < h->tamanho){
        if(strcmp(h->arcos[i].label, remover.label) == 0)
            achado = VERDADEIRO;
        i++;
    }
    i--;
    h->arcos[i] = h->arcos[h->tamanho - 1];
    h->tamanho--;
    posicaoPai = i / 2;
    if(i == 0 || h->arcos[posicaoPai].custo < h->arcos[i].custo)
        PercolacaoDescendente(h, i);
    else
        PercolacaoAscendente(h, i);
}

arco extrairMinimo(HeapMinimo* h){
    arco min;
    min = h->arcos[0];
    h->arcos[0] = h->arcos[h->tamanho-1];
    h->tamanho--;
    PercolacaoDescendente(h, 0);
    return min;
}
//fim das rotinas para manipulação do heap

Grafo* iniciarGrafo(int nVertices, int nArcos){
    Grafo* g;
    g = (Grafo*) malloc(sizeof(Grafo));
    g->vertices = (vertice**) malloc(nVertices * sizeof(vertice*));
    g->arcos = (arco*) malloc(nArcos * sizeof(arco));
    g->nVertices = 0;
    g->nArcos = 0;
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
    while(!achada && i < (*g)->nArcos){
        if((*g)->arcos[i].origem == u && (*g)->arcos[i].destino == v)
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
    arco novoArco;
    novoArco.origem = u;
    novoArco.destino = v;
    char buffer[4];
    buffer[0] = '\0';
    sprintf(buffer, "%d", (*g)->nArcos);
    strcpy(novoArco.label,"A");
    strcat(novoArco.label, buffer);
    novoArco.custo = custoUV;
    (*g)->arcos[(*g)->nArcos] = novoArco;
    (*g)->nArcos++;
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
    free((*g)->arcos);
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
        if(nomeArquivo[strlen(nomeArquivo) - 1] == '\n')
            nomeArquivo[strlen(nomeArquivo) - 1] = '\0';

        arquivo = fopen(nomeArquivo,"r");

        if(arquivo == NULL)
            printf("\nArquivo nao encontrado! Tente novamente!\n\n");
        else
            arquivoEncontrado = VERDADEIRO;
    }while(!arquivoEncontrado);
    return arquivo;
}

Grafo* montarGrafo(FILE* arquivoGrafo, int* origem, int* destino, boolean* possuiArcoNegativo){
    Grafo* g;
    int nArcos, nVertices, verticeU, verticeV, custoUV,linhasLidas;
    fscanf(arquivoGrafo, "%d %d %d %d", &nVertices, &nArcos, origem, destino);
    g = iniciarGrafo(nVertices, nArcos);
    linhasLidas = 0;
    while(linhasLidas < nArcos){
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

void atualizarHeap(HeapMinimo* h, int* conjuntoZ, int tamanhoZ, Grafo** g, float* custos){
    int i,j;
    arco* tmp = (arco*) malloc( h->tamanho * sizeof(arco));
    j = 0;
    for(i = 0; i < h->tamanho; i++)
        if(pertence(&conjuntoZ, h->arcos[i].origem, tamanhoZ) && pertence(&conjuntoZ, h->arcos[i].destino, tamanhoZ)){
            tmp[j] = h->arcos[i];
            j++;
        }
    for(i = 0; i < j; i++)
        removerHeap(h, tmp[i]);
    free(tmp);
    
    i = j = 0;
    while(i < (*g)->vertices[encontrarVertice(g, conjuntoZ[tamanhoZ - 1])]->grau){
        if((*g)->arcos[j].origem == conjuntoZ[tamanhoZ - 1]){
            if(!pertence(&conjuntoZ, (*g)->arcos[j].destino, tamanhoZ)){
                arco novaFronteira = (*g)->arcos[j];
                novaFronteira.custo += (int)custos[encontrarVertice(g, novaFronteira.origem)];
                inserirHeap(h, novaFronteira);
            }
            i++;
        }
        j++;
    }
}

void Dijkstra(Grafo** g, vertice* origem, vertice* destino, int** anterior, float** custos){
    int i, j, tamanhoZ;
    int* conjuntoZ;
    arco minima;
    for(i = 0; i < (*g)->nVertices; i++){
        (*anterior)[(*g)->vertices[i]->indice] = 0;
        (*custos)[(*g)->vertices[i]->indice] = INFINITY;
    }
    (*custos)[origem->indice] = 0;
    conjuntoZ = (int*) malloc((*g)->nVertices * sizeof(int));
    conjuntoZ[0] = origem->valor;
    tamanhoZ = 1;
    i = j = 0;
    HeapMinimo* h = CriarHeap((*g)->nArcos);
    while(i < origem->grau){
        if((*g)->arcos[j].origem == origem->valor){
            inserirHeap(h, (*g)->arcos[j]);
            i++;
        }
        j++;
    }
    while(!heapVazia(h) && !pertence(&conjuntoZ, destino->valor, tamanhoZ)){
        minima = extrairMinimo(h);
        conjuntoZ[tamanhoZ] = minima.destino;
        tamanhoZ++;
        (*custos)[encontrarVertice(g, minima.destino)] = minima.custo;
        (*anterior)[encontrarVertice(g, minima.destino)] = minima.origem;
        atualizarHeap(h, conjuntoZ, tamanhoZ, g, *custos);
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
    for(i = tamanhoCaminho - 1; i > 0; i--)
        printf("%d -> ", aux[i]);
    printf("%d]\nCusto do caminho: %d. Tamanho do caminho: %d vertices.\n\n", aux[0], (int)((*custos)[encontrarVertice(g, verticeDestino)]), tamanhoCaminho);
}

int main(int narg, char* argv[]){
    Grafo* g;
    FILE* arquivoGrafo;
    int verticeOrigem, verticeDestino;
    boolean possuiArcoNegativo = FALSO;
    if(narg > 1){
        arquivoGrafo = fopen(argv[1], "r");
        if(!arquivoGrafo)
            printf("\nArquivo nao encontrado! Tente novamente!\n\n");
    }
    else
        arquivoGrafo = carregarArquivo();
    g = montarGrafo(arquivoGrafo, &verticeOrigem, &verticeDestino, &possuiArcoNegativo);
    if(g->nArcos > 0 && (encontrarVertice(&g, verticeOrigem) != -1) && (encontrarVertice(&g, verticeDestino) != -1)){
        if(possuiArcoNegativo)
            printf("\n\nO grafo informado possui pelo menos 1 arco com custo negativo, nao posso calcular o caminho minimo de %d a %d usando o algoritmo de Dijkstra!\n\n",verticeOrigem, verticeDestino);
        else{
            int* anterior = (int*) malloc(g->nVertices * sizeof(int));
            float* custos = (float*) malloc(g->nVertices * sizeof(float));
            Dijkstra(&g, g->vertices[encontrarVertice(&g, verticeOrigem)], g->vertices[encontrarVertice(&g, verticeDestino)], &anterior, &custos);
            if(custos[g->vertices[encontrarVertice(&g, verticeDestino)]->indice] == INFINITY)
                printf("\n\nO custo para ir de %d a %d no grafo é INFINITO, pois nao existe caminho entre tais vertices!\n\n", verticeOrigem, verticeDestino);
            else
                mostrarCaminho(&g, verticeOrigem, verticeDestino, &anterior, &custos);
            free(anterior);
            free(custos);
        }
    }
    else
       printf("\n\nO custo para ir de %d a %d no grafo é INFINITO, pois nao existe caminho entre tais vertices!\n\n", verticeOrigem, verticeDestino); 
    destruirGrafo(&g);
    return 0;
}