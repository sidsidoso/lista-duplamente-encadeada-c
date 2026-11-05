# Lista Duplamente Encadeada Circular com Sentinela em C

[![CI](https://github.com/sidsidoso/lista-duplamente-encadeada-c/actions/workflows/ci.yml/badge.svg)](https://github.com/sidsidoso/lista-duplamente-encadeada-c/actions/workflows/ci.yml)
![Linguagem](https://img.shields.io/badge/linguagem-C11-blue.svg)
[![Licença: MIT](https://img.shields.io/badge/licen%C3%A7a-MIT-green.svg)](LICENSE)

Implementação de uma **lista duplamente encadeada circular** de inteiros em C11
com **nó sentinela**: não existe nenhum ponteiro `NULL` dentro da lista, então
inserir e remover no início, no meio ou no fim usam exatamente o mesmo código.
Inclui percurso nos dois sentidos, inversão trocando ponteiros, **merge sort de
lista** em O(n log n) sem alocar memória e **concatenação em O(1)**.
Projeto desenvolvido para a disciplina de **Estrutura de Dados**.

---

## Sumário

- [Conceito](#conceito)
- [Operações e complexidade](#operações-e-complexidade)
- [Decisões de implementação e otimizações](#decisões-de-implementação-e-otimizações)
- [Estrutura de pastas](#estrutura-de-pastas)
- [Como compilar, executar e testar](#como-compilar-executar-e-testar)
- [Exemplo de saída](#exemplo-de-saída)
- [Referências](#referências)

---

## Conceito

Cada nó guarda o valor e **dois** ponteiros: para o anterior (`ant`) e para o
próximo (`prox`).

```c
typedef struct No {
    int valor;
    struct No *ant;
    struct No *prox;
} No;
```

O **sentinela** `S` é um nó "falso" que fica embutido na própria estrutura
`ListaDupla`. Ele fecha o círculo: o primeiro elemento é `S.prox` e o último é
`S.ant`. A lista vazia é apenas o sentinela apontando para si mesmo.

```
 lista vazia:        lista com 10, 20, 30:

    +---+             +---+    +----+    +----+    +----+
 +->| S |<-+       +->| S |<-->| 10 |<-->| 20 |<-->| 30 |<-+
 |  +---+  |       |  +---+    +----+    +----+    +----+  |
 +---------+       +-----------------------------------------+
 ant = prox = S      S.ant = 30  e  30.prox = S  (fecha o círculo)
```

**Inserção sem casos especiais**: para inserir `novo` depois de `a` (seja `a`
o sentinela, um nó do meio ou o último), religam-se sempre os mesmos quatro
ponteiros:

```
 antes:   [a] <---> [b]

 1) novo->ant  = a          3) a->prox = novo
 2) novo->prox = b          4) b->ant  = novo

 depois:  [a] <---> [novo] <---> [b]
```

Inserir no início é "inserir depois de `S`"; inserir no fim é "inserir depois
de `S.ant`". Remover é o inverso: `no->ant->prox = no->prox` e
`no->prox->ant = no->ant`.

**Inversão trocando ponteiros**: basta trocar `ant` e `prox` de **todos** os
nós, inclusive do sentinela. O círculo passa a ser percorrido no sentido
contrário, sem mover nenhum valor:

```
 antes:   S -> 10 -> 20 -> 30 -> S      (seguindo prox)
 troca:   em cada nó, ant <-> prox
 depois:  S -> 30 -> 20 -> 10 -> S
```

**Concatenação em O(1)**: o bloco inteiro da lista `B` é encaixado entre o
último nó de `A` e o sentinela de `A`; `B` volta a ficar vazia.

```
 A: SA <-> 1 <-> 2 <-> SA        B: SB <-> 3 <-> 4 <-> SB

 A: SA <-> 1 <-> 2 <-> 3 <-> 4 <-> SA        B: SB (vazia)
```

**Merge sort de lista**: o círculo é aberto (vira lista simples terminada em
`NULL`), ordenado religando apenas os `prox` com o merge sort *bottom-up*, e
depois os `ant` são reconstruídos numa única passada.

```
 k=1:  [5][2][9][1][7]   ->  [2 5][1 9][7]
 k=2:  [2 5][1 9][7]     ->  [1 2 5 9][7]
 k=4:  [1 2 5 9][7]      ->  [1 2 5 7 9]      (uma única fusão: fim)
```

## Operações e complexidade

| Operação                       | Função                     | Tempo            | Espaço extra |
|--------------------------------|----------------------------|------------------|--------------|
| Criar                          | `lista_criar`              | O(1)             | O(1)         |
| Destruir / limpar              | `lista_destruir`, `lista_limpar` | O(n)       | O(1)         |
| Inserir no início / fim        | `lista_inserir_inicio`, `lista_inserir_fim` | O(1) | O(1)  |
| Inserir na posição *k*         | `lista_inserir_posicao`    | O(min(k, n−k))   | O(1)         |
| Remover do início / fim        | `lista_remover_inicio`, `lista_remover_fim` | O(1) | O(1)  |
| Remover da posição *k*         | `lista_remover_posicao`    | O(min(k, n−k))   | O(1)         |
| Remover por valor              | `lista_remover_valor`      | O(n)             | O(1)         |
| Buscar valor                   | `lista_buscar`             | O(n)             | O(1)         |
| Obter elemento da posição *k*  | `lista_obter`              | O(min(k, n−k))   | O(1)         |
| Inverter                       | `lista_inverter`           | O(n)             | O(1)         |
| Ordenar (merge sort)           | `lista_ordenar`            | O(n log n)       | O(1)         |
| Concatenar duas listas         | `lista_concatenar`         | **O(1)**         | O(1)         |
| Tamanho / vazia                | `lista_tamanho`, `lista_vazia` | O(1)         | O(1)         |
| Percorrer (frente / trás)      | `lista_percorrer`, `lista_percorrer_reverso` | O(n) | O(1) |

Espaço total: **O(n)** — um nó (valor + 2 ponteiros) por elemento, mais o
sentinela, que não exige alocação separada.

Comparação com a lista **simplesmente** encadeada: remover o último elemento
cai de O(n) para O(1), e o acesso por posição pode partir da ponta mais
próxima (no máximo n/2 passos).

## Decisões de implementação e otimizações

- **Sentinela embutido** (`No sentinela` dentro de `struct ListaDupla`): criar
  a lista é uma única alocação e nenhuma operação precisa testar `NULL`,
  "lista vazia" ou "é a cabeça/cauda?". Toda inserção e remoção passa por duas
  primitivas (`ligar_depois` e `desligar`).
- **Tamanho armazenado**: `lista_tamanho` é O(1), as posições são validadas
  antes de qualquer percurso e permitem decidir **de qual lado começar**
  (`no_na_posicao` anda no máximo ⌈n/2⌉ nós).
- **Inversão trocando ponteiros**: em vez de trocar valores, troca `ant` e
  `prox` em cada nó (sentinela incluso). É O(n), não aloca nada e mantém os
  nós no mesmo endereço.
- **Merge sort de lista bottom-up** (algoritmo de Simon Tatham): iterativo,
  sem recursão (não estoura a pilha), **sem alocar memória**, O(n log n) no
  pior caso e **estável** (usa `<=` na fusão). Ordena só pelos `prox` e
  reconstrói os `ant` em uma passada final.
- **Concatenação O(1)**: como o sentinela dá acesso direto ao primeiro e ao
  último nó das duas listas, basta religar quatro ponteiros e somar os
  tamanhos. A lista de origem é reiniciada como vazia e continua utilizável.
- **Percurso com callback + contexto (`void *`)** nos dois sentidos, sem
  expor os nós ao usuário.
- **Tipo opaco** e verificação de todo `malloc`: em falta de memória a
  função retorna `false` e a lista continua consistente.
- Testes com uma **bateria aleatória de 20 000 operações** comparada a um
  vetor-modelo, conferindo a lista para frente e para trás (o que valida a
  coerência de `ant` e `prox`), e merge sort comparado com `qsort`.

## Estrutura de pastas

```
lista-duplamente-encadeada-c/
├── .github/workflows/ci.yml     # integração contínua (testes com sanitizers + demo)
├── include/lista_dupla.h        # API pública (struct opaca + documentação)
├── src/lista_dupla.c            # implementação
├── src/main.c                   # programa de demonstração
├── tests/test_lista_dupla.c     # testes unitários com assert
├── Makefile
├── LICENSE
└── README.md
```

## Como compilar, executar e testar

### Linux / macOS (com `make`)

```bash
make            # compila demo e testes em build/
make run        # executa a demonstração
make test       # executa os testes
make test SANITIZE=1   # testes com AddressSanitizer + UBSan
make clean      # remove build/
```

### Windows (MinGW-w64 / GCC, sem `make`)

```bat
mkdir build
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/lista_dupla.c src/main.c -o build/demo.exe
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/lista_dupla.c tests/test_lista_dupla.c -o build/test_lista_dupla.exe
build\demo.exe
build\test_lista_dupla.exe
```

(Com o MinGW, `mingw32-make` também funciona no lugar de `make`.)

Saída esperada dos testes:

```
Testes da lista duplamente encadeada:
  [ok] teste_lista_vazia
  [ok] teste_um_elemento
  [ok] teste_inserir
  [ok] teste_remover
  [ok] teste_remover_valor_primeira_ocorrencia
  [ok] teste_buscar_e_obter
  [ok] teste_inverter
  [ok] teste_ordenar
  [ok] teste_ordenar_aleatorio
  [ok] teste_concatenar
  [ok] teste_aleatorio_contra_modelo
  [ok] teste_limpar
  [ok] teste_ponteiros_nulos
13 testes passaram
```

## Exemplo de saída

Saída real de `make run` (`S` representa o sentinela):

```
== Lista duplamente encadeada circular com sentinela (S) ==
lista criada:                    [0] S <-> S
inserir_fim 10, 20, 30:          [3] S <-> 10 <-> 20 <-> 30 <-> S
inserir_inicio 5:                [4] S <-> 5 <-> 10 <-> 20 <-> 30 <-> S
inserir_posicao(2, 15):          [5] S <-> 5 <-> 10 <-> 15 <-> 20 <-> 30 <-> S
inserir_posicao(fim, 35):        [6] S <-> 5 <-> 10 <-> 15 <-> 20 <-> 30 <-> 35 <-> S
percorrer_reverso:               [6] S <-> 35 <-> 30 <-> 20 <-> 15 <-> 10 <-> 5 <-> S

buscar 20: encontrado na posicao 3
buscar 99: nao encontrado
obter(4): 30

remover_inicio (saiu 5):         [5] S <-> 10 <-> 15 <-> 20 <-> 30 <-> 35 <-> S
remover_fim (saiu 35):           [4] S <-> 10 <-> 15 <-> 20 <-> 30 <-> S
remover_posicao(1) (saiu 15):    [3] S <-> 10 <-> 20 <-> 30 <-> S
remover_valor(30):               [2] S <-> 10 <-> 20 <-> S
inverter:                        [2] S <-> 20 <-> 10 <-> S

outra lista:                     [6] S <-> 42 <-> -7 <-> 8 <-> 15 <-> 0 <-> 23 <-> S
concatenar(l, outra) em O(1):    [8] S <-> 20 <-> 10 <-> 42 <-> -7 <-> 8 <-> 15 <-> 0 <-> 23 <-> S
outra (esvaziada):               [0] S <-> S
ordenar (merge sort):            [8] S <-> -7 <-> 0 <-> 8 <-> 10 <-> 15 <-> 20 <-> 23 <-> 42 <-> S

soma via callback: 111
```

## Referências

- CORMEN, T. H.; LEISERSON, C. E.; RIVEST, R. L.; STEIN, C. *Algoritmos: Teoria e Prática*. 3. ed. Rio de Janeiro: Elsevier, 2012. (Cap. 10.2 — Listas ligadas e sentinelas.)
- KNUTH, D. E. *The Art of Computer Programming*, v. 1: *Fundamental Algorithms*. 3. ed. Addison-Wesley, 1997. (Seção 2.2.5 — Doubly linked lists.)
- TATHAM, S. *Mergesort For Linked Lists*. Disponível em: <https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html>.
- CELES, W.; CERQUEIRA, R.; RANGEL, J. L. *Introdução a Estruturas de Dados: com técnicas de programação em C*. 2. ed. Elsevier, 2016.

## Licença

Distribuído sob a licença MIT. Veja [LICENSE](LICENSE).
