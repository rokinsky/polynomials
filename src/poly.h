/** @file
    Interfejs klasy wielomianów

    @author Jakub Pawlewicz <pan@mimuw.edu.pl>, 
    Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-04-24, 2017-06-03
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

/** Maksymalny stopien jednomianu */
#define POLY_EXP_MAX INT_MAX

/** Maksymalny stopien dla DEG_BY */
#define POLY_DEG_MAX UINT_MAX

/** Maksywalna wartość współczynnika */
#define POLY_COEFF_MAX LONG_MAX

/** Minimalna wartość współczynnika */
#define POLY_COEFF_MIN LONG_MIN

/** Typ współczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykładników wielomianu */
typedef int poly_exp_t;

/** Lista jednomianów */
typedef struct Node* List;

/**
 * Struktura przechowująca wielomian
 * Wielomian ma listę jednomianów w przypadku wielomianu normalnego,
 * wpp ma pustą listę i stałą.
 */
typedef struct Poly
{
    poly_coeff_t c; ///< @f$x_0@f$
    List l; ///<  lista jednomianów
} Poly;

/**
  * Struktura przechowująca jednomian
  * Jednomian ma postać `p * x^e`.
  * Współczynnik `p` może też być wielomianem.
  * Będzie on traktowany jako wielomian nad kolejną zmienną (nie nad x).
  */
typedef struct Mono
{
    Poly p; ///< współczynnik
    poly_exp_t exp; ///< wykładnik
} Mono;

/**
 * Struktura przchowująca element listy jednomianów.
 */
typedef struct Node
{
    Mono m; ///< jednomian
    struct Node *next; ///< następny jednomian
} Node;

/**
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c)
{
    return (Poly) {.c = c, .l = NULL};
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
static inline Poly PolyZero()
{
    return PolyFromCoeff(0);
}

/**
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e)
{
    return (Mono) {.p = *p, .exp = e};
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p)
{
    return p->l == NULL;
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
static inline bool PolyIsZero(const Poly *p)
{
    return PolyIsCoeff(p) && p->c == 0;
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m)
{
    PolyDestroy(&(m->p));
}

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m)
{
    return (Mono) {.p = PolyClone(&(m->p)), .exp = m->exp};
}

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru).
 * Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianem
 * i zmniejszane są indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Usuwa tablicę wielomianów z pamięci.
 * @param[in] count : liczba wielomianów
 * @param[in] x : tablica wielomianów
 */
void PolyArrayDestroy(unsigned count, Poly x[]);

/**
 * Zwraca wielomian `p`, w którym pod zmienną @f$x_i@f$ podstawiamy wielomian `x[i]`
 * @param[in] p : wielomian
 * @param[in] count : liczba wielomianów
 * @param[in] x : tablica wielomianów
 * @return @f$p(x[0], x[1], \ldots, x[count - 1], 0, 0, \ldots, 0)@f$
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

/**
 * Tworzy pustą listę jednomianów.
 * @return pusta lista jednomianów
 */
List ListCreate();

/**
 * Sprawdza, czy lista jest pusta.
 * @param[in] l : lista jednomianów
 * @return wynik w postaci boolowej 
 */
bool ListIsEmpty(const List l);

/**
 * Usuwa listę jednomianów.
 * @param[in] l : lista jednomianów
 */
void ListDestroy(List l);

/**
 * Tworzy nowy element listy.
 * @param[in] m : jednomian
 * @param[in] next : następnik
 * @return element listy
 */
Node* NodeCreate(Mono *m, Node *next);

/**
 * Klonuję element listy.
 * @param[in] n : element listy
 * @return sklonowany element listy
 */
Node* NodeClone(Node* n);

/**
 * Klonuję listę jednomianów.
 * @param[in] l : lista jednomianów
 * @return sklonowana lista jednomianów
 */
List ListClone(const List l);

/**
 * Daje dlugość listy.
 * @param[in] l : lista jednomianów
 * @return dlugość listy
 */
unsigned ListLen(const List l);

/**
 * Dodaje jednomian na listę.
 * @param[in] l : lista jednomianów
 * @param[in] m : jednomian
 * @return lista jednomianów po ewentualnemu dodaniu
 */
List ListAddMonos(List l, Mono *m);

/**
 * Scaluję dwie listy.
 * @param[in] p : lista jednomianów
 * @param[in] q : lista jednomianów
 * @return scalowana lista
 */
List ListMerge(const List p, const List q);

/**
 * Przerabia liste jednomianów na tablice jednomianów, przejmuje na wlasność.
 * @param[in] l : lista jednomianów
 * @param[in,out] size : rozmiar tablicy
 * @return tablica jednomianów
 */
Mono* ListToArray(List l, unsigned *size);

#endif /* __POLY_H__ */