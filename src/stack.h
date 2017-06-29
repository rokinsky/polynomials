/** @file
    Interfejs stosu wielomianów

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-05-27
*/

#ifndef __STACK_H__
#define __STACK_H__

#include "poly.h"

/**
 * Struktura przchowująca element stosu wielomianów.
 */
typedef struct StackNode{
    Poly p; ///< wielomian
    struct StackNode *next; ///< następny wielomian
} StackNode;

/** Stos wielomianów */
typedef StackNode *Stack;

/**
 * Sprawdza czy stos jest pusty.
 * @param[in] s: stos wielomianów
 * @return wartość boolean
 */
bool StackIsEmpty(Stack s);

/**
 * Tworzy pusty stos.
 * @return pusty stos wielomianów
 */
Stack StackInit();

/**
 * Wkłada wielomian na stos.
 * @param[in] s: stos wielomianów
 * @param[in] p: wielomian
 */
void StackPush(Stack *s, Poly p);

/**
 * Zdejmuje ze stosu wielomian.
 * @param[in] s: stos wielomianów
 * @return wielomian ze szczytu stosu
 */
Poly StackPop(Stack *s);

/**
 * Wyczyszcza stos.
 * @param[in] s: stos wielomianów
 */
void StackDestroy(Stack *s);

/**
 * Zwraca czy pasuje argument.
 * @param[in] s : stos
 * @param[in] size : rozmiar
 * @return wynik
 */
bool StackIsSmallerThan(const Stack s, size_t size);

/**
 * Zdejmuje ze stosu `count` wielomianów do tablicy
 * @param[in] s : stos
 * @param[in] count : liczba wielomianów do zdjęcia
 * @return tablica wielomianów
 */
Poly* StackPopArray(Stack *s, unsigned count);

#endif /* __STACK_H__ */