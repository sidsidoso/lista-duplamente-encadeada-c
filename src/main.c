#include <stdio.h>
#include <stdlib.h>

#include "lista_dupla.h"

static void imprimir_valor(int valor, void *contexto)
{
    (void)contexto;
    printf("%d <-> ", valor);
}

static void somar(int valor, void *contexto)
{
    *(long long *)contexto += valor;
}

static void mostrar(const char *rotulo, const ListaDupla *l)
{
    printf("%-32s [%zu] S <-> ", rotulo, lista_tamanho(l));
    lista_percorrer(l, imprimir_valor, NULL);
    puts("S");
}

static void mostrar_reverso(const char *rotulo, const ListaDupla *l)
{
    printf("%-32s [%zu] S <-> ", rotulo, lista_tamanho(l));
    lista_percorrer_reverso(l, imprimir_valor, NULL);
    puts("S");
}

static int falhar(ListaDupla *a, ListaDupla *b)
{
    fprintf(stderr, "erro: memoria insuficiente\n");
    lista_destruir(a);
    lista_destruir(b);
    return EXIT_FAILURE;
}

int main(void)
{
    ListaDupla *l = lista_criar();
    ListaDupla *outra = lista_criar();
    if (l == NULL || outra == NULL) {
        return falhar(l, outra);
    }

    puts("== Lista duplamente encadeada circular com sentinela (S) ==");
    mostrar("lista criada:", l);

    bool ok = true;
    for (int v = 10; v <= 30; v += 10) {
        ok = ok && lista_inserir_fim(l, v);
    }
    mostrar("inserir_fim 10, 20, 30:", l);
    ok = ok && lista_inserir_inicio(l, 5);
    mostrar("inserir_inicio 5:", l);
    ok = ok && lista_inserir_posicao(l, 2, 15);
    mostrar("inserir_posicao(2, 15):", l);
    ok = ok && lista_inserir_posicao(l, lista_tamanho(l), 35);
    mostrar("inserir_posicao(fim, 35):", l);
    if (!ok) {
        return falhar(l, outra);
    }
    mostrar_reverso("percorrer_reverso:", l);

    size_t pos;
    int v;
    if (lista_buscar(l, 20, &pos)) {
        printf("\nbuscar 20: encontrado na posicao %zu\n", pos);
    }
    if (!lista_buscar(l, 99, NULL)) {
        puts("buscar 99: nao encontrado");
    }
    if (lista_obter(l, 4, &v)) {
        printf("obter(4): %d\n\n", v);
    }

    char rotulo[64];
    if (lista_remover_inicio(l, &v)) {
        snprintf(rotulo, sizeof rotulo, "remover_inicio (saiu %d):", v);
        mostrar(rotulo, l);
    }
    if (lista_remover_fim(l, &v)) {
        snprintf(rotulo, sizeof rotulo, "remover_fim (saiu %d):", v);
        mostrar(rotulo, l);
    }
    if (lista_remover_posicao(l, 1, &v)) {
        snprintf(rotulo, sizeof rotulo, "remover_posicao(1) (saiu %d):", v);
        mostrar(rotulo, l);
    }
    lista_remover_valor(l, 30);
    mostrar("remover_valor(30):", l);
    lista_inverter(l);
    mostrar("inverter:", l);

    const int extras[] = {42, -7, 8, 15, 0, 23};
    for (size_t i = 0; i < sizeof extras / sizeof extras[0]; i++) {
        ok = ok && lista_inserir_fim(outra, extras[i]);
    }
    if (!ok) {
        return falhar(l, outra);
    }
    putchar('\n');
    mostrar("outra lista:", outra);
    lista_concatenar(l, outra);
    mostrar("concatenar(l, outra) em O(1):", l);
    mostrar("outra (esvaziada):", outra);
    lista_ordenar(l);
    mostrar("ordenar (merge sort):", l);

    long long soma = 0;
    lista_percorrer(l, somar, &soma);
    printf("\nsoma via callback: %lld\n", soma);

    lista_destruir(outra);
    lista_destruir(l);
    return EXIT_SUCCESS;
}
