#undef NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lista_dupla.h"

static int testes_passaram = 0;

#define RODAR(teste)                          \
    do {                                      \
        teste();                              \
        testes_passaram++;                    \
        printf("  [ok] %s\n", #teste);        \
    } while (0)

#define TAM(v) (sizeof(v) / sizeof((v)[0]))

/* Gerador pseudoaleatorio xorshift32: deterministico em qualquer plataforma. */
static uint32_t semente = 2463534242u;
static uint32_t aleatorio(void)
{
    semente ^= semente << 13;
    semente ^= semente >> 17;
    semente ^= semente << 5;
    return semente;
}

/* Contexto para comparar a lista com um vetor esperado via callback.
 * 'passo' = +1 percorre o vetor para frente, -1 de tras para frente. */
typedef struct {
    const int *esperado;
    size_t n;
    size_t vistos;
    int reverso;
} Comparacao;

static void comparar(int valor, void *contexto)
{
    Comparacao *c = contexto;
    assert(c->vistos < c->n);
    size_t i = c->reverso ? c->n - 1 - c->vistos : c->vistos;
    assert(valor == c->esperado[i]);
    c->vistos++;
}

/* Confere conteudo nos dois sentidos (garante que ant e prox estao
 * coerentes), o tamanho e o acesso por posicao. */
static void conferir(const ListaDupla *l, const int *esperado, size_t n)
{
    Comparacao frente = {esperado, n, 0, 0};
    lista_percorrer(l, comparar, &frente);
    assert(frente.vistos == n);

    Comparacao tras = {esperado, n, 0, 1};
    lista_percorrer_reverso(l, comparar, &tras);
    assert(tras.vistos == n);

    assert(lista_tamanho(l) == n);
    assert(lista_vazia(l) == (n == 0));
    for (size_t i = 0; i < n; i++) {
        int v;
        assert(lista_obter(l, i, &v) && v == esperado[i]);
    }
    int v = 12345;
    assert(!lista_obter(l, n, &v) && v == 12345);
}

static ListaDupla *lista_de(const int *v, size_t n)
{
    ListaDupla *l = lista_criar();
    assert(l != NULL);
    for (size_t i = 0; i < n; i++) {
        assert(lista_inserir_fim(l, v[i]));
    }
    return l;
}

static void teste_lista_vazia(void)
{
    ListaDupla *l = lista_criar();
    assert(l != NULL);
    int v = 7;
    assert(!lista_remover_inicio(l, &v));
    assert(!lista_remover_fim(l, &v));
    assert(!lista_remover_posicao(l, 0, &v));
    assert(!lista_remover_valor(l, 1));
    assert(!lista_buscar(l, 1, NULL));
    assert(v == 7);
    lista_inverter(l);
    lista_ordenar(l);
    conferir(l, NULL, 0);
    lista_destruir(l);
}

static void teste_um_elemento(void)
{
    ListaDupla *l = lista_criar();
    assert(l != NULL);
    assert(lista_inserir_posicao(l, 0, 1));
    const int e[] = {1};
    conferir(l, e, 1);
    lista_inverter(l);
    lista_ordenar(l);
    conferir(l, e, 1);

    int v;
    assert(lista_remover_fim(l, &v) && v == 1);
    conferir(l, NULL, 0);
    assert(lista_inserir_fim(l, 2));
    assert(lista_remover_inicio(l, &v) && v == 2);
    conferir(l, NULL, 0);
    lista_destruir(l);
}

static void teste_inserir(void)
{
    ListaDupla *l = lista_criar();
    assert(l != NULL);
    assert(lista_inserir_fim(l, 20));
    assert(lista_inserir_inicio(l, 10));
    assert(lista_inserir_fim(l, 40));
    assert(lista_inserir_posicao(l, 2, 30));  /* meio */
    assert(lista_inserir_posicao(l, 0, 0));   /* inicio */
    assert(lista_inserir_posicao(l, 5, 50));  /* fim */
    assert(!lista_inserir_posicao(l, 7, 99)); /* alem do fim */
    const int e[] = {0, 10, 20, 30, 40, 50};
    conferir(l, e, TAM(e));
    lista_destruir(l);
}

static void teste_remover(void)
{
    const int base[] = {1, 2, 3, 4, 5, 6, 7};
    ListaDupla *l = lista_de(base, TAM(base));
    int v;
    assert(lista_remover_inicio(l, &v) && v == 1);
    assert(lista_remover_fim(l, &v) && v == 7);
    assert(lista_remover_posicao(l, 1, &v) && v == 3); /* lado da frente */
    assert(lista_remover_posicao(l, 2, &v) && v == 5); /* lado de tras */
    assert(!lista_remover_posicao(l, 3, &v));
    const int e1[] = {2, 4, 6};
    conferir(l, e1, TAM(e1));

    assert(lista_remover_valor(l, 6));
    assert(!lista_remover_valor(l, 6));
    assert(lista_remover_valor(l, 2));
    const int e2[] = {4};
    conferir(l, e2, TAM(e2));
    assert(lista_remover_posicao(l, 0, NULL));
    conferir(l, NULL, 0);
    lista_destruir(l);
}

static void teste_remover_valor_primeira_ocorrencia(void)
{
    const int base[] = {5, 1, 5, 2, 5};
    ListaDupla *l = lista_de(base, TAM(base));
    assert(lista_remover_valor(l, 5));
    const int e[] = {1, 5, 2, 5};
    conferir(l, e, TAM(e));
    lista_destruir(l);
}

static void teste_buscar_e_obter(void)
{
    const int base[] = {8, 3, 8, -1};
    ListaDupla *l = lista_de(base, TAM(base));
    size_t pos = 99;
    assert(lista_buscar(l, 8, &pos) && pos == 0);
    assert(lista_buscar(l, -1, &pos) && pos == 3);
    assert(!lista_buscar(l, 42, &pos) && pos == 3);
    int v;
    assert(lista_obter(l, 3, &v) && v == -1);
    assert(lista_obter(l, 1, NULL));
    lista_destruir(l);
}

static void teste_inverter(void)
{
    const int base[] = {1, 2, 3, 4, 5};
    ListaDupla *l = lista_de(base, TAM(base));
    lista_inverter(l);
    const int e[] = {5, 4, 3, 2, 1};
    conferir(l, e, TAM(e));
    /* Depois de inverter, as insercoes nas pontas continuam corretas. */
    assert(lista_inserir_inicio(l, 6));
    assert(lista_inserir_fim(l, 0));
    const int e2[] = {6, 5, 4, 3, 2, 1, 0};
    conferir(l, e2, TAM(e2));
    lista_inverter(l);
    lista_inverter(l);
    conferir(l, e2, TAM(e2));
    lista_destruir(l);
}

static int comparar_int(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static void teste_ordenar(void)
{
    const int base[] = {5, -2, 9, 0, 5, 3, -2, 8, 1};
    ListaDupla *l = lista_de(base, TAM(base));
    lista_ordenar(l);
    const int e[] = {-2, -2, 0, 1, 3, 5, 5, 8, 9};
    conferir(l, e, TAM(e));
    lista_ordenar(l); /* ja ordenada */
    conferir(l, e, TAM(e));
    lista_inverter(l);
    lista_ordenar(l); /* ordem decrescente */
    conferir(l, e, TAM(e));
    lista_destruir(l);
}

/* Compara com qsort para varios tamanhos (inclusive nao potencias de 2). */
static void teste_ordenar_aleatorio(void)
{
    enum { MAX = 1000 };
    static int v[MAX];
    for (size_t n = 0; n <= MAX; n = n < 20 ? n + 1 : n * 3 + 7) {
        for (size_t i = 0; i < n; i++) {
            v[i] = (int)(aleatorio() % 200) - 100;
        }
        ListaDupla *l = lista_de(v, n);
        lista_ordenar(l);
        qsort(v, n, sizeof v[0], comparar_int);
        conferir(l, v, n);
        lista_destruir(l);
    }
}

static void teste_concatenar(void)
{
    const int a[] = {1, 2, 3};
    const int b[] = {4, 5};
    ListaDupla *la = lista_de(a, TAM(a));
    ListaDupla *lb = lista_de(b, TAM(b));
    lista_concatenar(la, lb);
    const int e[] = {1, 2, 3, 4, 5};
    conferir(la, e, TAM(e));
    conferir(lb, NULL, 0);

    /* A origem esvaziada continua utilizavel. */
    assert(lista_inserir_fim(lb, 9));
    const int e2[] = {9};
    conferir(lb, e2, TAM(e2));

    /* Concatenar em lista vazia, concatenar vazia e consigo mesma. */
    ListaDupla *vazia = lista_criar();
    assert(vazia != NULL);
    lista_concatenar(vazia, lb);
    conferir(vazia, e2, TAM(e2));
    conferir(lb, NULL, 0);
    lista_concatenar(la, lb);
    conferir(la, e, TAM(e));
    lista_concatenar(la, la);
    conferir(la, e, TAM(e));

    lista_destruir(vazia);
    lista_destruir(la);
    lista_destruir(lb);
}

/* Bateria aleatoria: cada operacao e aplicada na lista e em um vetor-modelo,
 * e os dois sao comparados periodicamente. */
static void teste_aleatorio_contra_modelo(void)
{
    enum { MAX = 600 };
    static int modelo[MAX];
    size_t n = 0;
    ListaDupla *l = lista_criar();
    assert(l != NULL);

    for (int op = 0; op < 20000; op++) {
        uint32_t r = aleatorio();
        int valor = (int)(aleatorio() % 50);
        int v;
        switch (r % 8) {
        case 0:
        case 1: {
            if (n == MAX) break;
            size_t pos = aleatorio() % (n + 1);
            assert(lista_inserir_posicao(l, pos, valor));
            memmove(&modelo[pos + 1], &modelo[pos], (n - pos) * sizeof modelo[0]);
            modelo[pos] = valor;
            n++;
            break;
        }
        case 2:
            if (n == MAX) break;
            assert(lista_inserir_inicio(l, valor));
            memmove(&modelo[1], &modelo[0], n * sizeof modelo[0]);
            modelo[0] = valor;
            n++;
            break;
        case 3:
            if (n == MAX) break;
            assert(lista_inserir_fim(l, valor));
            modelo[n++] = valor;
            break;
        case 4: {
            if (n == 0) {
                assert(!lista_remover_posicao(l, 0, &v));
                break;
            }
            size_t pos = aleatorio() % n;
            assert(lista_remover_posicao(l, pos, &v) && v == modelo[pos]);
            memmove(&modelo[pos], &modelo[pos + 1], (n - pos - 1) * sizeof modelo[0]);
            n--;
            break;
        }
        case 5: {
            size_t i = 0;
            while (i < n && modelo[i] != valor) i++;
            bool achou = i < n;
            assert(lista_remover_valor(l, valor) == achou);
            if (achou) {
                memmove(&modelo[i], &modelo[i + 1], (n - i - 1) * sizeof modelo[0]);
                n--;
            }
            break;
        }
        case 6:
            if (n > 0) {
                assert(lista_remover_fim(l, &v) && v == modelo[n - 1]);
                n--;
            }
            break;
        default:
            if (r % 64 == 7) {
                lista_inverter(l);
                for (size_t i = 0; i < n / 2; i++) {
                    int t = modelo[i];
                    modelo[i] = modelo[n - 1 - i];
                    modelo[n - 1 - i] = t;
                }
            } else if (r % 64 == 15) {
                lista_ordenar(l);
                qsort(modelo, n, sizeof modelo[0], comparar_int);
            }
            break;
        }
        if (op % 97 == 0) {
            conferir(l, modelo, n);
        }
    }
    conferir(l, modelo, n);
    lista_destruir(l);
}

static void teste_limpar(void)
{
    const int base[] = {1, 2, 3};
    ListaDupla *l = lista_de(base, TAM(base));
    lista_limpar(l);
    conferir(l, NULL, 0);
    assert(lista_inserir_fim(l, 4));
    const int e[] = {4};
    conferir(l, e, TAM(e));
    lista_destruir(l);
}

static void teste_ponteiros_nulos(void)
{
    int v;
    assert(!lista_inserir_inicio(NULL, 1));
    assert(!lista_inserir_fim(NULL, 1));
    assert(!lista_inserir_posicao(NULL, 0, 1));
    assert(!lista_remover_inicio(NULL, &v));
    assert(!lista_remover_fim(NULL, &v));
    assert(!lista_remover_posicao(NULL, 0, &v));
    assert(!lista_remover_valor(NULL, 1));
    assert(!lista_buscar(NULL, 1, NULL));
    assert(!lista_obter(NULL, 0, &v));
    assert(lista_tamanho(NULL) == 0);
    assert(lista_vazia(NULL));
    lista_inverter(NULL);
    lista_ordenar(NULL);
    lista_concatenar(NULL, NULL);
    lista_percorrer(NULL, NULL, NULL);
    lista_percorrer_reverso(NULL, NULL, NULL);
    lista_limpar(NULL);
    lista_destruir(NULL);
}

int main(void)
{
    puts("Testes da lista duplamente encadeada:");
    RODAR(teste_lista_vazia);
    RODAR(teste_um_elemento);
    RODAR(teste_inserir);
    RODAR(teste_remover);
    RODAR(teste_remover_valor_primeira_ocorrencia);
    RODAR(teste_buscar_e_obter);
    RODAR(teste_inverter);
    RODAR(teste_ordenar);
    RODAR(teste_ordenar_aleatorio);
    RODAR(teste_concatenar);
    RODAR(teste_aleatorio_contra_modelo);
    RODAR(teste_limpar);
    RODAR(teste_ponteiros_nulos);
    printf("%d testes passaram\n", testes_passaram);
    return 0;
}
