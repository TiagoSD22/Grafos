# Grafos
Repositório para o trabalho de Grafos

Simples trabalho abordando a implementação do glorioso algoritmo de Dijkstra para caminhos de custo mínimo em um grafo dirigido. Este trabalho foi proposto em uma disciplina de Grafos durante minha graduação.

Para compilar o programa utilize uma versão do GCC (>= 7.3). O arquivo executável pode ser chamado passando um argumento extra como parâmetro, este parâmetro deve ser o nome de um arquivo em extensão '.txt' contendo a representação de um grafo dirigido no seguinte formato:

n m s t
u1 v1 c1
u2 v2 c2
...
um vm cm

em que n é a quantidade de vértices, m a quantidade de arcos, s é o vértice origem, t é o vértice destino, ui e vi são a origem e o destino do arco i e ci é o custo da aresta i. Portanto, para um arquivo chamado grafo.txt pode-se executar o programa chamado nome-do-programa grafo.txt. Caso o programa seja executado sem passar um arquivo como argumento, este poderá ser informado via linha de comando na primeira tela que o programa abrirá. Tal arquivo deve estar no mesmo local do executável do programa.
