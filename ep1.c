/*
 *  Sobre este codigo:
 *  Este programa implementa o glorioso algoritmo de Dijkstra para obter um caminho de custo minimo entre um par de vertices
 *  de um grafo dirigido simples e que nao contenha nenhum arco de custo negativo.

 *  @Autor  : Tiago Siqueira Dionizio <tiagosdionizio@gmail.com>
 *  @Coautor: Paulo Victor da Silva Mourão <pvictormourao@gmail.com>
 *  @Coautor: Marcelo de Sousa Alves <marcelo.egcomp@gmail.com>
 *  @Coautor: Andersson Claudio Alves Santana <anderssonclaudio0@gmail.com>

 *  Data de atualizacao : 5 de Abril de 2019

 *  Modo de uso:
 *  Deve-se primeiramente compilar o arquivo fonte ep1.c com um compilador GCC de versão, preferivelmente, >= 7.3 e executar o 
 *  arquivo binario com o comando ./nome_do_programa(distros Linux ou MacOS) ou apenas nome_do_programa(distros Windows).
 *  O executavel pode ser chamado com um argumento extra que informa o nome de um arquivo com extensao .txt que contem 
 *  a representacao de um grafo dirigido, por exemplo, para um arquivo de nome grafo.txt, pode-se executar o programa chamando:
 *      ./nome_do_programa grafo.txt
 *  Caso o programa seja executado sem o argumento extra, o nome do arquivo contendo o grafo podera ser informado via linha de
 *  comando na primeira tela de execucao do programa. Apos o programa abrir o arquivo e montar o grafo lido, ele ira executar
 *  o algoritmo de Dijkstra para encontrar um caminho de custo minimo entre os vertices de origem e destino especificados na 
 *  primeira linha do arquivo aberto. Ao final do algoritmo sera mostrado em tela, caso exista, o caminho encontrado e seu custo.

 *  Restricao de uso:
 *  Possuir um compilador GCC instalado, caso contrario nao sera possivel compilar o codigo para gerar o executavel.
 *  O arquivo que contem a representacao do grafo deve possuir o seguinte modelo:
 *  n m s t
 *  u1 v1 c1
 *  u2 v2 c2
 *  ...
 *  um vm cm
 
 *  em que n é a quantidade de vértices, m a quantidade de arcos, s é o vértice origem, t é o vértice destino, 
 *  ui e vi são a origem e o destino do arco i e ci é o custo da aresta i.
 *  O arquivo que contem a representacao do grafo nao deve possuir arcos com custo negativo para garantir a corretude do algoritmo.

 *  Referencia para o algoritmo: 
 *  https://www.ime.usp.br/~pf/algoritmos_para_grafos/aulas/dijkstra.html
*/

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

//Foi utilizada a estrutura de lista de adjacencias para armazenar o grafo e suas informacoes
typedef struct grafo{
    int nVertices;
    int nArcos;
    arco* arcos;
    vertice** vertices;
}Grafo;

//Inicio das rotinas para manipulacao de um Heap binario
typedef struct heap{
    arco* arcos;
    int tamanho;
    int capacidade;
}HeapMinimo;

/*
 * Esta funcao cria um heap binario com o espaco passado como parametro
 * Entrada: um inteiro m
 * Saida: um heap com tamanho m alocado em memoria dinamica para comportar m arcos
*/
HeapMinimo* CriarHeap(int espaco){
    HeapMinimo* h = (HeapMinimo*) malloc(sizeof(HeapMinimo));
    h->tamanho=0;
    h->capacidade = espaco;
    h->arcos = (arco*) malloc(espaco * sizeof(arco));
    return h;
}

//Funcao que retorna VERDADEIRO caso o heap esteja vazio ou FALSO, caso contrario
boolean heapVazia(HeapMinimo* h){
    return h->tamanho == 0;
}

/*
 * As funcoes de percolacao servem para equilibrar o heap e mante-lo com sua estrutura de heap binario crescente sempre
 * que ocorre alguma operacao de insercao ou remocao de arcos. A percolacao ascendente equilibra o heap de baixo para cima em
 * caso de um no filho se torne menor do que seu pai.
 * Entrada: um heap h e um inteiro i que indica a posicao do no por onde a percolacao sera iniciada
 * Saida: o heap h equilibrado na forma crescente
*/
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

/*
 * A funcao de percolacao descendente e similar a ascendente, entretanto o sentido da percolao segue de cima par baixo, para os 
 * casos em que um no pai se torne maior do que um de seus filhos.
 * Entrada: umm heap h e um inteiro i que indica a posicao do no por onde a percolacao sera iniciada
 * Saida: o heap h equilibrado de forma crescente
*/
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

//Esta funcao insere um novo arco no heap
void inserirHeap(HeapMinimo* h, arco nova){
    if( h->tamanho < h->capacidade){
        h->arcos[h->tamanho] = nova;
        PercolacaoAscendente(h, h->tamanho);
        h->tamanho++;
    }
}

//Esta funcao remove um arco do heap
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

/*
 * Esta funcao remove o no raiz do heap, este no contera o menor valor do heap, uma vez que este esta ordenado crescentemente.
 * Entrada: um heap h
 * Saida: um arco a que contem o menor custo dentre todos os arcos armazenados no heap h
*/
arco extrairMinimo(HeapMinimo* h){
    arco min;
    min = h->arcos[0];
    h->arcos[0] = h->arcos[h->tamanho-1];
    h->tamanho--;
    PercolacaoDescendente(h, 0);
    return min;
}
//Fim das rotinas para manipulacao de Heap binario

/*
 * Esta funcao serve para alocar espaco para um grafo
 * Entrada: um inteiro n e um inteiro m
 * Saida: um grafo G com espaco alocado em memoria para caber n vertices e m arcos
*/
Grafo* iniciarGrafo(int nVertices, int nArcos){
    Grafo* g;
    g = (Grafo*) malloc(sizeof(Grafo));
    g->vertices = (vertice**) malloc(nVertices * sizeof(vertice*));
    g->arcos = (arco*) malloc(nArcos * sizeof(arco));
    g->nVertices = 0;
    g->nArcos = 0;
}

/*
 * Esta funcao auxilia para encontrar a posicao de um vertice contendo um determinado valor dentro do grafo
 * Entrada: um ponteiro para um grafo g e um inteiro u que representa o valor a ser buscado no grafo
 * Saida: um inteiro i que representa a posicao do vertice com valor u dentro da lista de vertices do grafo g 
 * Obs: a passagem de referencia com o ponteiro extra nesta e em todas as demais funcoes envolvendo grafos
 * nao somente pela necessidade de modificar diretamente o grafo original, mas tambem para evitar a copia de toda a 
 * estrutura do grafo.
*/
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

//Esta funcao retorna VERDADEIRO caso um vertice de valor u exista no grafo G e FALSO caso contrario
boolean possuiVertice(Grafo** g, int verticeValor){
    return encontrarVertice(g, verticeValor) != -1;
}

//Esta funcao adiciona um novo vertice u na lista de vertices do grafo g
void adicionarVertice(Grafo** g, int verticeValor){
    vertice* novoVertice = (vertice*) malloc(sizeof(vertice));
    novoVertice->valor = verticeValor;
    novoVertice->prox = NULL;
    novoVertice->grau = 0;
    novoVertice->indice = (*g)->nVertices;
    (*g)->nVertices++;
    (*g)->vertices[(*g)->nVertices - 1] = novoVertice;
}

//Esta funcao retorna VERDADEIRO caso exista um arco ligando u a v no grafo g e FALSO caso contrario
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

/*
 * Esta funcao adiciona um novo arco uv na lista de arcos do grafo g
 * Entrada um ponteiro para um ponteiro de grafo g, um inteiro u, um inteiro v e um inteiro c
 * Saida: nada, entretanto, como o grafo g e passado por referencia, ao final da funcao, g contera um arco a mais de custo c
 * que tera como origem o vertice contendo o valor u e como destino o vertice contendo o valor v
*/
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
    /*Cada arco possui uma label no formato Ai em que i e a posicao que o arco ocupa na lista de arcos do grafo 
    tendo i como valor inicial 1.*/
    strcat(novoArco.label, buffer);
    novoArco.custo = custoUV;
    (*g)->arcos[(*g)->nArcos] = novoArco;
    (*g)->nArcos++;
}

//Esta funcao libera o espaco alocado em memoria para o grafo g
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

/*
 * Esta funcao solicita ao usuario um nome de arquivo com extensao .txt que contem a representacao de um grafo no formato
 * especificado no cabecalho desse codigo. Apos encontrar e carregar o arquivo, este e retornado ao fim da funcao
 */
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

/*
 * Esta funcao monta um grafo a partir do grafo lido no arquivo passado como parametro
 * Entrada: um arquivo f que contem a representacao do grafo, bem como os vertices origem e destino para os quais
 * deseja-se encontrar um caminho de custo minimo, um ponteiro para inteiro u que armazenara o valor do vertice origem,
 * um ponteiro para inteiro v que armazenara o valor do vertice destino e um ponteiro para boolean que atuara como flag
 * para identificar posteriormente se o grafo passado contem algum arco de custo negativo para alertar o usuario do risco
 * da nao corretude do algoritmo caso isso aconteca.
*/
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

/*
 * Esta funcao retorna VERDADEIRO caso um determinado elemento exista em um conjunto representado por um vetor v e 
 * FALSO caso contrario
 * Entrada: um ponteiro para um vetor v, um inteiro e um inteiro n que indica o tamanho do vetor v
 * Saida: VERDADEIRO caso o valor e exista no vetor v e FALSO caso ele nao exista
 * Obs: o uso do ponteiro extra para o vetor foi feito para evitar passar uma copia de todos os elementos do vetor
*/ 
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

/*
 * Esta funcao atualiza o heap binario a cada iteracao do algoritmo de Dijkstra, removendo os arcos outrora de fronteira e que
 * agora sao internos e adicionando arcos que eram externos a regiao Z e passaram a ser de fronteira.
*/
void atualizarHeap(HeapMinimo* h, int* conjuntoZ, int tamanhoZ, Grafo** g, float* custos){
    int i,j;
    arco* tmp = (arco*) malloc( h->tamanho * sizeof(arco));
    j = 0;
    /*
     Este laco remove todos os arcos que na atual iteracao sao internos a regiao Z, como nao podemos alterar diretamente o heap
     cada vez que detectamos um arco interno que deve ser removido, este e armazenado em um vetor de arcos auxiliar e ao final
     do laco, removemos todos os arcos deste vetor do heap binario
    */
    for(i = 0; i < h->tamanho; i++)
        if(pertence(&conjuntoZ, h->arcos[i].origem, tamanhoZ) && pertence(&conjuntoZ, h->arcos[i].destino, tamanhoZ)){
            tmp[j] = h->arcos[i];
            j++;
        }
    //removendo or arcos internos do heap   
    for(i = 0; i < j; i++)
        removerHeap(h, tmp[i]);
    free(tmp);
    
    /*
     Este laco procura dentre os arcos do grafo g aqueles cuja origem parte do ultimo vertice adicionado a regiao Z e a origem
     e algum valor fora de Z, caso isso ocorra, significa que este arco, na atual iteracao, e um arco de fronteira e que sera 
     armazenado no heap h, antes de armazenarmos no heap, alteramos o valor do custo do arco, considere que o arco seja uv,
     o seu novo custo sera o custo associado a u, valor este que estara armazenado no vetor de custos passado como parametro da 
     funcao mais o custo inicial do arco, como a passagem do grafo g e feita por referencia, para nao alterarmos diretamente 
     o valor do arco na estrutura de g, o que causaria incoerencia em iteracoes seguintes, cada arco de fronteira uv e armazenado
     primeiramente em uma variavel auxiliar novaFronteira, seu custo entao e alterado e logo em seguida inserimos o novo arco de 
     fronteira no heap h, assim, garantimos que a cada iteracao do algoritmo de Dijkstra o primeiro elemento do heap sera
     justamente o arco de menor custo da fronteira de Z.
    */
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

/*
 * O algoritmo de Dijkstra calcula o caminho de custo minimo entre os vertices origem e destino passados como parametro.
 * Entrada: um ponteiro de ponteiro para um grafo g, um ponteiro para um vertice u que representa a origem do caminho,
 * um ponteiro para um vertice v que representa o destino do caminho, um ponteiro de ponteiro de inteiro anterior que serve
 * para remontar ao final do algoritmo o caminho, caso ele exista, de u a v e um ponteiro de ponteiro de float custos para
 * armazenar os custos para sair de u e chegar a cada vertices vi pelo qual o algoritmo visitar
 * Saida: nada, entretanto, ao final do algoritmo, como os vetores anterior e custos foram passados por referencia, neles 
 * estarao as informacoes necessarias para reconstruir o caminho de custo minimo de u a v e o custo desse caminho.
 * Obs: os ponteiros extras para o grafo e os vetores anteior e custos foram usados para evitar passar uma copia de toda a
 * estrutura do grafo e porque precisamos armazenar as duas informacoes de anteriores e custos no referidos vetores o que seria
 * impossivel de retornar, pois uma funcao em c deve ter retorno minimo, 
 * logo, armazenamos essas informacoes por passagem de referencia.
 * Como o algoritmo funciona: inicialmente o vetor de custos e preenchido com INFINITY para todos os vertices, indicando
 * que nao sabemos o custo para, partindo da origem u, chegar a cada vertice vi, o vetor anteior e inicializado com 0 para todos
 * o vertices, contudo o custo e o anterior da origem sao ambos 0. A cada iteracao do algoritmo escolhemos o arco de menor custo 
 * da fronteira Z que se inicia apenas com o vertice origem, seja uv tal arco, incluimos v em Z, anotamos que o anterior de v e u,
 * que o custo para chegar a v e o custo para chegar a u + o custo de uv e atualizamos os arcos da fronteira de Z.
 * Quando o algoritmo para, se o vertice destino estiver em Z significa que temos um caminho entre a origem e destino, caso contrario
 * dizemos que o custo para chegarmos ao destino e INFINITO, pois nao existe tal caminho.
*/
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
    i = 0;
    j = 0;
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

//Esta funcao mostra em tela o caminho de custo minimo entre os vertices origem e destino e o custo de tal caminho
void mostrarCaminho(Grafo** g, int verticeOrigem, int verticeDestino, int** anterior, float** custos){
    int vAtual, i, tamanhoCaminho;
    int aux[(*g)->nVertices];
    vAtual = verticeDestino;
    tamanhoCaminho = 0;
    i = 0;
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

/*
 * A rotina principal do programa abre um arquivo passado como parametro e monta um grafo com as informacoes lidas no arquivo,
 * caso algum arco do grafo possua custo negativo o programa aborta prematuramente, pois nao podemos garantir a corretude do 
 * algoritmo de Dijkstra para esse caso, caso contrario, sera executado o algoritmo para obter o caminho de custo minimo entre
 * a origem e destino especificados no arquivo, caso este caminho exista este sera mostrado em tela, bem como seu custo, caso
 * contrario sera mostrada uma mesagem informado que nao existe um caminho entre os referidos vertices e que, consequentemente,
 * o custo para chegar ao destino e INFINITO. Existem tambem algumas validacoes para casos de grafos especificos, por exemplo,
 * quando nao existe nenhum arcos partindo do vertice origem ou chegando ao vertice destino, casos estes quem que, obviamente,
 * nao havera caminho entre origem e destino.
*/
int main(int narg, char* argv[]){
    Grafo* g;
    FILE* arquivoGrafo;
    int verticeOrigem, verticeDestino;
    boolean possuiArcoNegativo = FALSO;
    if(narg > 1){ //caso o programa seja chamado com o argumento extra do nome de um arquivo .txt
        arquivoGrafo = fopen(argv[1], "r");
        if(!arquivoGrafo){
            printf("\nArquivo nao encontrado! Tente novamente!\n\n");
            return 0;
        }
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
