#include <stdlib.h>

struct nodoLista
{
    void *dados;            // Ponteiro para os dados do NODO
    struct nodoLista *ant;  // Ponteiro para o nodo anterior
    struct nodoLista *prox; // Ponteiro para o nodo posterior
};
typedef struct nodoLista LISTA;

// inicia lista
LISTA * initLista();

// insere no fim da lista e retorna posicao
LISTA* insertLista(LISTA *lista, void *dado);

// retorna 1 se a lista esta vazia, senao retorna 0
int emptyLista(LISTA *lista);

// remove da lista noda na posicao pos, retorna 1 se conseguiu, senao retorna 0
LISTA* removeLista(LISTA *lista, int pos);

// retorna o dado da lista, senao retorna NULL
void * getNodeLista(LISTA *lista, int pos);

// retorna o proximo nodo
LISTA * getNextNodeLista(LISTA *lista);

// retorna o nodo anterior
LISTA * getPrevNodeLista(LISTA *lista);

// retorna o primeiro nodo da lista, senao retorna NULL
LISTA * getFirstNodeLista(LISTA *lista);

// retorna o ultimo nodo da lista, senao retorna NULL
LISTA * getLastNodeLista(LISTA *lista);

// destroi a lista
void destroyLista(LISTA *lista);