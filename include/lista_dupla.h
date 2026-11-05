#ifndef LISTA_DUPLA_H
#define LISTA_DUPLA_H

/*
 * Lista duplamente encadeada circular de inteiros com no sentinela.
 *
 * O sentinela e um no "falso" embutido na propria lista: o primeiro elemento
 * e sentinela.prox e o ultimo e sentinela.ant. Como a lista nunca tem
 * ponteiros NULL, inserir e remover em qualquer ponto usam o mesmo codigo,
 * sem casos especiais para cabeca ou cauda. O tamanho e mantido em O(1).
 * Posicoes sao indices base 0.
 */

#include <stdbool.h>
#include <stddef.h>

/* Tipo opaco: os nos e a lista ficam escondidos em src/lista_dupla.c. */
typedef struct ListaDupla ListaDupla;

/* Funcao chamada para cada elemento nos percursos. */
typedef void (*ListaVisitante)(int valor, void *contexto);

/* Cria uma lista vazia. Retorna NULL se faltar memoria. O(1) */
ListaDupla *lista_criar(void);

/* Libera todos os nos e a propria lista. Aceita NULL. O(n) */
void lista_destruir(ListaDupla *l);

/* Remove todos os elementos, mantendo a lista utilizavel. O(n) */
void lista_limpar(ListaDupla *l);

/* Insere no inicio. Retorna false se faltar memoria. O(1) */
bool lista_inserir_inicio(ListaDupla *l, int valor);

/* Insere no fim. Retorna false se faltar memoria. O(1) */
bool lista_inserir_fim(ListaDupla *l, int valor);

/* Insere de modo que o novo elemento ocupe a posicao 'pos' (0..tamanho).
 * Caminha a partir da extremidade mais proxima.
 * Retorna false se pos > tamanho ou faltar memoria. O(min(pos, n - pos)) */
bool lista_inserir_posicao(ListaDupla *l, size_t pos, int valor);

/* Remove o primeiro elemento e grava em *saida (se saida != NULL).
 * Retorna false se a lista estiver vazia. O(1) */
bool lista_remover_inicio(ListaDupla *l, int *saida);

/* Remove o ultimo elemento. Retorna false se a lista estiver vazia. O(1) */
bool lista_remover_fim(ListaDupla *l, int *saida);

/* Remove o elemento da posicao 'pos' (0..tamanho-1).
 * Retorna false se a posicao for invalida. O(min(pos, n - pos)) */
bool lista_remover_posicao(ListaDupla *l, size_t pos, int *saida);

/* Remove a primeira ocorrencia de 'valor'. Retorna false se nao existir. O(n) */
bool lista_remover_valor(ListaDupla *l, int valor);

/* Procura a primeira ocorrencia de 'valor'; grava a posicao em *posicao
 * (se posicao != NULL). Retorna false se nao encontrar. O(n) */
bool lista_buscar(const ListaDupla *l, int valor, size_t *posicao);

/* Le o elemento da posicao 'pos'. Retorna false se a posicao for invalida.
 * O(min(pos, n - pos)) */
bool lista_obter(const ListaDupla *l, size_t pos, int *saida);

/* Inverte a ordem trocando ant/prox de cada no (inclusive o sentinela).
 * Nao aloca memoria. O(n) */
void lista_inverter(ListaDupla *l);

/* Ordena em ordem crescente com merge sort de lista (bottom-up, estavel).
 * Apenas religa ponteiros: nao aloca memoria e nao usa recursao.
 * O(n log n) tempo, O(1) espaco extra. */
void lista_ordenar(ListaDupla *l);

/* Move todos os elementos de 'origem' para o fim de 'destino'.
 * 'origem' fica vazia (mas continua valida). Nao aloca memoria. O(1)
 * Nao faz nada se destino == origem. */
void lista_concatenar(ListaDupla *destino, ListaDupla *origem);

/* Numero de elementos. O(1) */
size_t lista_tamanho(const ListaDupla *l);

/* true se a lista nao tiver elementos. O(1) */
bool lista_vazia(const ListaDupla *l);

/* Chama visitante(valor, contexto) do primeiro ao ultimo elemento. O(n) */
void lista_percorrer(const ListaDupla *l, ListaVisitante visitante, void *contexto);

/* Chama visitante(valor, contexto) do ultimo ao primeiro elemento. O(n) */
void lista_percorrer_reverso(const ListaDupla *l, ListaVisitante visitante,
                             void *contexto);

#endif /* LISTA_DUPLA_H */
