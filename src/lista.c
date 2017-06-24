#include "../include/lista.h"

// inicia lista
LISTA *initLista() {
    return NULL;
}

// insere no fim da lista e retorna posi��o
LISTA *insertLista(LISTA *lista, void *dado) {
    LISTA *add, *nodo;
    int iterator = 0;

    nodo = lista;

    // inicializa add
    add = malloc(sizeof(LISTA));
    add->dados = dado;
    add->ant = NULL;
    add->prox = NULL;

    if (emptyLista(lista) == 1) { // se a lista estiver vazia coloca no inicio
        return add;
    } else {
        while (nodo->prox != NULL) { // senao acha o fim
            nodo = nodo->prox;
            iterator++;
        }

        nodo->prox = add; // e coloca no fim
        add->ant = nodo;

        return lista;
    }
}

// retorna 1 se a lista esta vazia, senao retorna 0
int emptyLista(LISTA *lista) {
    if (lista == NULL) {
        return 1;
    } else {
        return 0;
    }
}

// remove da lista noda na posicao pos, retorna 1 se conseguiu, senao retorna 0
LISTA *removeLista(LISTA *lista, int pos) {
    LISTA *nodo;
    nodo = lista;
    int i;

    if (emptyLista(lista) == 0) {
        if (pos == 0) {
            if (nodo->prox == NULL) {
                free(nodo);

                return NULL;
            } else {
                nodo = nodo->prox;
                free(nodo->ant);

                return nodo;
            }
        } else {
            for (i = 0; i < pos; i++) {
                if (nodo->prox == NULL) {
                    return lista;
                } // fim da lista -> nao ha elemento pos na lista
                else {
                    nodo = nodo->prox;
                } // pula pro proximos ate chegar em pos
            }

            nodo->ant->prox = nodo->prox;
            if (nodo->prox != NULL)
                nodo->prox->ant = nodo->ant; // encadeia nodos vizinhos
            free(nodo);
            return lista;
        }
    } else {
        return lista;
    } // lista vazia -> nao ha oq remover
}

// retorna o dado da lista, senao retorna NULL
void *getNodeLista(LISTA *lista, int pos) {
    LISTA *nodo;
    nodo = lista;
    int i;

    if (emptyLista(lista) == 0) {
        for (i = 0; i < pos; i++) {
            if (nodo->prox == NULL) {
                return NULL;
            } // fim da lista -> nao ha elemento pos na lista
            else {
                nodo = nodo->prox;
            } // pula pro proximos ate chegar em pos
        }
        return nodo->dados;
    } else {
        return NULL;
    }
}
// retorna o proximo nodo
LISTA *getNextNodeLista(LISTA *lista) {
    return lista->prox;
}

// retorna o nodo anterior
LISTA *getPrevNodeLista(LISTA *lista) {
    return lista->ant;
}

// retorna o primeiro nodo da lista, senao retorna NULL
LISTA *getFirstNodeLista(LISTA *lista) {
    LISTA *nodo;
    nodo = lista;

    if (emptyLista(nodo) == 1)
        return NULL;
    else {
        while (nodo->ant != NULL) {
            nodo = nodo->ant;
        }
        return nodo;
    }
}

// retorna o ultimo nodo da lista, senao retorna NULL
LISTA *getLastNodeLista(LISTA *lista) {
    LISTA *nodo;
    nodo = lista;

    if (emptyLista(nodo) == 1) {
        return NULL;
    } else {
        while (nodo->prox != NULL) {
            nodo = nodo->prox;
        }
        return nodo;
    }
}

// destroi a lista
void destroyLista(LISTA *lista) {
    LISTA *nodo;
    nodo = lista;

    while (nodo->prox != NULL) {
        nodo = nodo->prox;
        free(nodo->ant);
    }
    free(nodo);
}
