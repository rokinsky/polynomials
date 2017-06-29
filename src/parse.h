/** @file
    Interfejs parsowania

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-06-03
*/

#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdio.h>

#include "poly.h"
#include "utils.h"

/** Rezultat wczytywania linii */
typedef enum {
    POLY,
    COMMAND,
    END,
    WRONGPOLY,
    WRONGCOMMAND,
    WRONGVARIABLE,
    WRONGVALUE,
    WRONGCOUNT
} ParseResult;

/** Wszystkie dostępne komendy */
typedef enum {
    ZERO = 0,
    IS_COEFF,
    IS_ZERO,
    CLONE,
    ADD,
    MUL,
    NEG,
    SUB,
    IS_EQ,
    DEG,
    DEG_BY,
    AT,
    PRINT,
    POP,
    COMPOSE
} Command;

/**
 * Sprawdza ile jest potrzebne argumentów do komendy
 * @param[in] p : wielomian
 * @param[in] command : komenda
 * @return liczba
 */
static inline size_t ParseNeededArguments(const Poly *p, Command command)
{
    switch (command)
    {
        case ZERO:
            return 0;
        case IS_COEFF:
        case IS_ZERO:
        case CLONE:
        case DEG:
        case DEG_BY:
        case AT:
        case PRINT:
        case POP:
        case NEG:
            return 1;
        case IS_EQ:
        case ADD:
        case MUL:
        case SUB:
            return 2;
        case COMPOSE:
            return (size_t)p->c + 1;
    }
    return 0;
}

/**
 * Wypisuje wielomian.
 * @param[in] p : wielomian
 */
void PolyPrint(const Poly *p);

/**
 * Próbuję wczytać liniję z wejścia
 * @param[in,out] p : wielomian
 * @param[in,out] command : komenda
 * @param[in,out] c : kolumna
 * @return rezultat wczytywania linii
 */
ParseResult ParseLineRead(Poly *p, Command *command, unsigned *c);

/**
 * Wypisuje komunikat o błędzie stosu.
 * @param[in] r : wiersz
 */
static inline void ErrorStackUnderflow(int r)
{
    fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", r);
}

/**
 * Wypisuje komunikat o błędzie wczytywania wielomianu.
 * @param[in] r : wiersz
 * @param[in] c : kolumna
 */
static inline void ErrorWrongPoly(int r, int c)
{
    fprintf(stderr, "ERROR %d %d\n", r, c);
}

/**
 * Wypisuje komunikat o błędzie komendy.
 * @param[in] r : wiersz
 */
static inline void ErrorWrongCommand(int r)
{
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", r);
}

/**
 * Wypisuje komunikat o błędzie AT.
 * @param[in] r : wiersz
 */
static inline void ErrorWrongValue(int r)
{
    fprintf(stderr, "ERROR %d WRONG VALUE\n", r);
}

/**
 * Wypisuje komunikat o błędzie DEG_BY.
 * @param[in] r : wiersz
 */
static inline void ErrorWrongVariable(int r)
{
    fprintf(stderr, "ERROR %d WRONG VARIABLE\n", r);
}

/**
 * Wypisuje komunikat o błędzie COMPOSE.
 * @param[in] r : wiersz
 */
static inline void ErrorWrongCount(int r)
{
    fprintf(stderr, "ERROR %d WRONG COUNT\n", r);
}

#endif /* __PARSE_H__ */