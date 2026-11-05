#include "lista_dupla.h"

#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *ant;
    struct No *prox;
} No;

/* O sentinela fica embutido na struct: criar a lista e uma unica alocacao.
 * Seu campo 'valor' nunca e lido. */
struct ListaDupla {
    No sentinela;
    size_t tamanho;
};

/* ---------------------------------------------------------------------- */
/* Primitivas internas: toda insercao/remocao passa por estas duas funcoes. */

/* Liga 'novo' entre 'antes' e antes->prox. Funciona igual para inicio, meio
 * e fim, pois o sentinela fecha o circulo. */
static void ligar_depois(ListaDupla *l, No *antes, No *novo)
{
    No *depois = antes->prox;
    novo->ant = antes;
    novo->prox = depois;
    antes->prox = novo;
    depois->ant = novo;
    l->tamanho++;
}

/* Desliga 'no' (que nao pode ser o sentinela), devolve o valor e libera. */
static int desligar(ListaDupla *l, No *no)
{
    int valor = no->valor;
    no->ant->prox = no->prox;
    no->prox->ant = no->ant;
    free(no);
    l->tamanho--;
    return valor;
}

static bool inserir_depois(ListaDupla *l, No *antes, int valor)
{
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        return false;
    }
    novo->valor = valor;
    ligar_depois(l, antes, novo);
    return true;
}

/* Devolve o no da posicao 'pos' (0..tamanho-1) caminhando pelo lado mais
 * curto. Com pos == tamanho devolve o proprio sentinela (util para inserir). */
static No *no_na_posicao(const ListaDupla *l, size_t pos)
{
    No *s = (No *)&l->sentinela;
    No *atual;
    if (pos <= l->tamanho / 2) {
        atual = s->prox;
        while (pos-- > 0) {
            atual = atual->prox;
        }
    } else {
        atual = s;
        for (size_t passos = l->tamanho - pos; passos > 0; passos--) {
            atual = atual->ant;
        }
    }
    return atual;
}

/* ---------------------------------------------------------------------- */

static void iniciar_vazia(ListaDupla *l)
{
    l->sentinela.valor = 0;
    l->sentinela.ant = &l->sentinela;
    l->sentinela.prox = &l->sentinela;
    l->tamanho = 0;
}

ListaDupla *lista_criar(void)
{
    ListaDupla *l = malloc(sizeof *l);
    if (l != NULL) {
        iniciar_vazia(l);
    }
    return l;
}

void lista_limpar(ListaDupla *l)
{
    if (l == NULL) {
        return;
    }
    No *s = &l->sentinela;
    No *atual = s->prox;
    while (atual != s) {
        No *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    iniciar_vazia(l);
}

void lista_destruir(ListaDupla *l)
{
    if (l == NULL) {
        return;
    }
    lista_limpar(l);
    free(l);
}

bool lista_inserir_inicio(ListaDupla *l, int valor)
{
    return l != NULL && inserir_depois(l, &l->sentinela, valor);
}

bool lista_inserir_fim(ListaDupla *l, int valor)
{
    return l != NULL && inserir_depois(l, l->sentinela.ant, valor);
}

bool lista_inserir_posicao(ListaDupla *l, size_t pos, int valor)
{
    if (l == NULL || pos > l->tamanho) {
        return false;
    }
    /* O novo no entra antes do no que hoje ocupa 'pos'. */
    No *alvo = no_na_posicao(l, pos);
    return inserir_depois(l, alvo->ant, valor);
}

bool lista_remover_inicio(ListaDupla *l, int *saida)
{
    if (l == NULL || l->tamanho == 0) {
        return false;
    }
    int v = desligar(l, l->sentinela.prox);
    if (saida != NULL) {
        *saida = v;
    }
    return true;
}

bool lista_remover_fim(ListaDupla *l, int *saida)
{
    if (l == NULL || l->tamanho == 0) {
        return false;
    }
    int v = desligar(l, l->sentinela.ant);
    if (saida != NULL) {
        *saida = v;
    }
    return true;
}

bool lista_remover_posicao(ListaDupla *l, size_t pos, int *saida)
{
    if (l == NULL || pos >= l->tamanho) {
        return false;
    }
    int v = desligar(l, no_na_posicao(l, pos));
    if (saida != NULL) {
        *saida = v;
    }
    return true;
}

bool lista_remover_valor(ListaDupla *l, int valor)
{
    if (l == NULL) {
        return false;
    }
    No *s = &l->sentinela;
    for (No *atual = s->prox; atual != s; atual = atual->prox) {
        if (atual->valor == valor) {
            desligar(l, atual);
            return true;
        }
    }
    return false;
}

bool lista_buscar(const ListaDupla *l, int valor, size_t *posicao)
{
    if (l == NULL) {
        return false;
    }
    const No *s = &l->sentinela;
    size_t i = 0;
    for (const No *atual = s->prox; atual != s; atual = atual->prox, i++) {
        if (atual->valor == valor) {
            if (posicao != NULL) {
                *posicao = i;
            }
            return true;
        }
    }
    return false;
}

bool lista_obter(const ListaDupla *l, size_t pos, int *saida)
{
    if (l == NULL || pos >= l->tamanho) {
        return false;
    }
    if (saida != NULL) {
        *saida = no_na_posicao(l, pos)->valor;
    }
    return true;
}

void lista_inverter(ListaDupla *l)
{
    if (l == NULL) {
        return;
    }
    /* Trocar ant <-> prox em todos os nos, sentinela incluso, inverte o
     * sentido do circulo. O laco anda pelo 'ant', que apos a troca e o
     * antigo 'prox'. */
    No *s = &l->sentinela;
    No *atual = s;
    do {
        No *tmp = atual->prox;
        atual->prox = atual->ant;
        atual->ant = tmp;
        atual = atual->ant;
    } while (atual != s);
}

/* Merge sort bottom-up (algoritmo de Simon Tatham) sobre uma lista simples
 * terminada em NULL, usando apenas o campo 'prox'. Em cada passada funde
 * pares de sublistas de tamanho k; para quando houver uma unica fusao. */
static No *ordenar_simples(No *lista)
{
    if (lista == NULL) {
        return NULL;
    }
    for (size_t k = 1;; k *= 2) {
        No *p = lista;
        No *cauda = NULL;
        size_t fusoes = 0;
        lista = NULL;

        while (p != NULL) {
            fusoes++;
            /* q avanca k nos a partir de p: [p, q) e a primeira sublista. */
            No *q = p;
            size_t tam_p = 0;
            while (tam_p < k && q != NULL) {
                tam_p++;
                q = q->prox;
            }
            size_t tam_q = k;

            while (tam_p > 0 || (tam_q > 0 && q != NULL)) {
                No *e;
                if (tam_p == 0) {
                    e = q; q = q->prox; tam_q--;
                } else if (tam_q == 0 || q == NULL || p->valor <= q->valor) {
                    /* '<=' preserva a ordem de iguais: ordenacao estavel. */
                    e = p; p = p->prox; tam_p--;
                } else {
                    e = q; q = q->prox; tam_q--;
                }
                if (cauda != NULL) {
                    cauda->prox = e;
                } else {
                    lista = e;
                }
                cauda = e;
            }
            p = q;
        }
        cauda->prox = NULL;
        if (fusoes <= 1) {
            return lista;
        }
    }
}

void lista_ordenar(ListaDupla *l)
{
    if (l == NULL || l->tamanho < 2) {
        return;
    }
    No *s = &l->sentinela;

    /* 1) Abre o circulo: vira uma lista simples terminada em NULL. */
    s->ant->prox = NULL;

    /* 2) Ordena religando so os 'prox'. */
    No *primeiro = ordenar_simples(s->prox);

    /* 3) Reconstroi os 'ant' e fecha o circulo pelo sentinela. */
    No *anterior = s;
    for (No *atual = primeiro; atual != NULL; atual = atual->prox) {
        anterior->prox = atual;
        atual->ant = anterior;
        anterior = atual;
    }
    anterior->prox = s;
    s->ant = anterior;
}

void lista_concatenar(ListaDupla *destino, ListaDupla *origem)
{
    if (destino == NULL || origem == NULL || destino == origem ||
        origem->tamanho == 0) {
        return;
    }
    No *sd = &destino->sentinela;
    No *so = &origem->sentinela;
    No *primeiro = so->prox;
    No *ultimo = so->ant;

    /* Encaixa o bloco [primeiro..ultimo] entre o ultimo de destino e sd. */
    sd->ant->prox = primeiro;
    primeiro->ant = sd->ant;
    ultimo->prox = sd;
    sd->ant = ultimo;
    destino->tamanho += origem->tamanho;

    iniciar_vazia(origem);
}

size_t lista_tamanho(const ListaDupla *l)
{
    return l == NULL ? 0 : l->tamanho;
}

bool lista_vazia(const ListaDupla *l)
{
    return lista_tamanho(l) == 0;
}

void lista_percorrer(const ListaDupla *l, ListaVisitante visitante, void *contexto)
{
    if (l == NULL || visitante == NULL) {
        return;
    }
    const No *s = &l->sentinela;
    for (const No *atual = s->prox; atual != s; atual = atual->prox) {
        visitante(atual->valor, contexto);
    }
}

void lista_percorrer_reverso(const ListaDupla *l, ListaVisitante visitante,
                             void *contexto)
{
    if (l == NULL || visitante == NULL) {
        return;
    }
    const No *s = &l->sentinela;
    for (const No *atual = s->ant; atual != s; atual = atual->ant) {
        visitante(atual->valor, contexto);
    }
}
