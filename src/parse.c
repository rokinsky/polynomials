/** @file
    Implementacja parsowania

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-06-03
*/

#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "utils.h"

/** Maksymalna długość poprawnej komendy */
#define COMMAND_LEN_MAX 16

/** Maksymalna długość wczytywanej liczby */
#define NUMBER_LEN_MAX 21

/** Liczba komend */
#define COMMAND_ALL 15

/** Maksymalna liczba w postaci napisu */
#define NUMBER_MAX_STRING "9223372036854775807"

/** Minimalna liczba w postaci napisu */
#define NUMBER_MIN_STRING "-9223372036854775808"

/** Tablica komend */
const char *t[] =
                {
                    "ZERO", "IS_COEFF",
                    "IS_ZERO", "CLONE",
                    "ADD", "MUL",
                    "NEG", "SUB",
                    "IS_EQ", "DEG",
                    "DEG_BY", "AT",
                    "PRINT", "POP",
                    "COMPOSE"
                };

/**
 * Ignoruje linije do końca.
 * @param[in] c : ostatni wczytany znak przed wywołaniem
 */
static void ParseLineIgnore(int c)
{
    ungetc(c, stdin);
    for (int x = getchar(); x != '\n' && x != EOF; x = getchar());
}

/**
 * Pomocnicza do wczytywania liczby. Porównuje dwa napisy.
 * @param[in] lim : napis
 * @param[in] s : napis
 * @param[in] n : długość napisu
 * @param[in,out] z : na którym znaku jest źle od końca
 * @return czy napisy się zgadzają
 */
static bool ParseNumberLimit(char lim[], char s[], unsigned n, unsigned *z)
{
    for (int i = n; i >= 0; i--)
    {
        (*z)++;
        if (lim[i] != s[i])
            return false;
    }
    return true;
}

/**
 * Próbuje wczytać liczbę ze standartowego wejścia.
 * @param[in,out] n : liczba
 * @param[in,out] c : kolumna
 * @return czy udalo się wczytać
 */
static bool ParseNumber(long long *n, unsigned *c)
{
    char s[NUMBER_LEN_MAX];
    int x = getchar();
    unsigned i = 0;
    (*c)++;
    if (x == '-' || (x >= '0' && x <= '9'))
    {
        s[i++] = (char) x;
        x = getchar();
        while (i < NUMBER_LEN_MAX-1 && x >= '0' && x <= '9')
        {
            s[i++] = (char) x;
            x = getchar();
            (*c)++;
        }
        s[i] = '\0';
        if (x != ')' && x != ',' && x != '\n' && x != EOF) {
            return false;
        }
        else
        {
            ungetc(x, stdin);
            if (i == 1 && s[0] == '-')
            {
                (*c)--;
                return false;
            }
            *n = atoll(s);
            bool res = true;
            unsigned z = 0;
            if (*n == POLY_COEFF_MIN)
                res = ParseNumberLimit(NUMBER_MIN_STRING, s, --i, &z);
            else if (*n == POLY_COEFF_MAX) {
                if (i == NUMBER_LEN_MAX)
                    (*c)--;
                res = ParseNumberLimit(NUMBER_MAX_STRING, s, --i, &z);
            }

            *c = res ? *c : (*c)-z;
            return res;
        }
    }
    else
    {
        (*c)--;
        ungetc(x, stdin);
        return false;
    }
}

/**
 * Próbuje wczytać liczbe dla DEG_BY i AT
 * @param[in,out] n : liczba
 * @return czy udało się wczytać
 */
static bool ParseNumberArgument(long long *n)
{
    unsigned u = 0;

    return ParseNumber(n, &u);
}

/**
 * Parsuje DEG_BY i AT
 * @param[in] command : komenda
 * @param[in,out] c : kolumna
 * @return COMMAND, WRONGVALUE, WRONGVARIABLE, WRONGCOMMAND
 */
static ParseResult ParseArgument(const Command *command, poly_coeff_t *c)
{
    long long n = 0;
    int x = getchar();
    if (x != ' ')
    {
        ParseLineIgnore(x);
        if (*command == AT)
            return WRONGVALUE;
        else if (*command == DEG_BY)
            return WRONGVARIABLE;
        else if (*command == COMPOSE)
            return WRONGCOUNT;
        else
            return WRONGCOMMAND;
    }
    else if (ParseNumberArgument(&n))
    {
        if (*command == AT && (n < POLY_COEFF_MIN || n > POLY_COEFF_MAX))
        {
            if ((x = getchar()) != '\n') ParseLineIgnore(x);
            return WRONGVALUE;
        }
        else if (*command == DEG_BY && (n < 0 || n > POLY_DEG_MAX))
        {
            if ((x = getchar()) != '\n') ParseLineIgnore(x);
            return WRONGVARIABLE;
        }
        else if (*command == COMPOSE && (n < 0 || n > UINT_MAX))
        {
            if ((x = getchar()) != '\n') ParseLineIgnore(x);
            return WRONGCOUNT;
        }
        else
        {
            if (getchar() != '\n')
            {
                ParseLineIgnore('\0');
                return WRONGCOMMAND;
            }
            *c = n;
            return COMMAND;
        }
    }
    else
    {
        ParseLineIgnore(x);
        if (*command == AT)
            return WRONGVALUE;
        else if (*command == DEG_BY)
            return WRONGVARIABLE;
        else if (*command == COMPOSE)
            return WRONGCOUNT;
        else
            return WRONGCOMMAND;
    }
}

/**
 * Próbuje wczytać wielomian.
 * @param[in,out] p : wielomian
 * @param[in,out] c : kolumna
 * @return czy udało się wczytać
 */
static bool ParsePoly(Poly *p, unsigned *c)
{
    int x = getchar();
    ungetc(x, stdin);
    if (x == '(') {
        List monos = ListCreate();

        do
        {
            if ((x = getchar()) != '(')
            {
                ParseLineIgnore(x);
                PolyDestroy(p);
                ListDestroy(monos);
                return false;
            }
            (*c)++;
            Poly l = PolyZero();
            if (ParsePoly(&l, c) == false)
            {
                PolyDestroy(p);
                ListDestroy(monos);
                return false;
            }
            (*c)++;
            if ((x = getchar()) != ',')
            {
                if(x == '\n' || x == ')')
                    (*c)--;
                ParseLineIgnore(x);
                PolyDestroy(p);
                PolyDestroy(&l);
                ListDestroy(monos);
                return false;
            }

            long long n;
            bool res = ParseNumber(&n, c);
            if (!res || n > POLY_EXP_MAX || n < 0)
            {
                if (res)
                    (*c)--;
                ParseLineIgnore('\0');
                PolyDestroy(p);
                PolyDestroy(&l);
                ListDestroy(monos);
                return false;
            }

            poly_exp_t exp = (poly_exp_t ) n;
            Mono m = MonoFromPoly(&l, exp);
            monos = NodeCreate(&m, monos);
            (*c)++;
            if ((x = getchar()) != ')')
            {
                if (x == ',')
                    (*c)--;
                ParseLineIgnore(x);
                PolyDestroy(p);
                ListDestroy(monos);
                return false;
            }
            (*c)++;
            x = getchar();
            if (x != '+' && x != '\n' && x != ',' && x != ')')
            {
                if (x == '(')
                    (*c)--;
                ParseLineIgnore(x);
                PolyDestroy(p);
                ListDestroy(monos);
                return false;
            }
            if (x != '+')
            {
                (*c)--;
                ungetc(x, stdin);
            }
        }
        while (x == '+');

        unsigned size;
        Mono *mm = ListToArray(monos, &size);
        *p = PolyAddMonos(size, mm);
        free(mm);
        return true;
    }
    else
    {
        long long n;
        bool res = ParseNumber(&n, c);
        if (res && n >= POLY_COEFF_MIN && n <= POLY_COEFF_MAX)
        {
            p->c = (poly_coeff_t) n;
            p->l = NULL;
            return true;
        }
        else
        {
            if (res) (*c)--;
            x = getchar();
            ParseLineIgnore(x);
            PolyDestroy(p);
            return false;
        }
    }
}

/**
 * Parsuje komendę.
 * @param[in,out] command : komenda
 * @return czy udało się wczytać
 */
static bool ParseCommand(Command *command)
{
    char s[COMMAND_LEN_MAX];
    int i = 0;
    int x = getchar();

    while (i < COMMAND_LEN_MAX - 1 && x != '\n' && x != ' ' && x != EOF)
    {
        s[i++] = (char)x;
        x = getchar();
    }
    s[i] = '\0';

    *command = -1;
    int j;
    for (j = 0; j < COMMAND_ALL && strcmp(s, t[++(*command)]); j++);
    if (j == COMMAND_ALL)
    {
        ParseLineIgnore(x);
        return false;
    }
    else
    {
        ungetc(x, stdin);
        return true;
    }
}

ParseResult ParseLineRead(Poly *p, Command *command, unsigned *c)
{
    int x = getchar();

    if (x == EOF)
        return END;

    ungetc(x, stdin);
    if ((x >= 'a' &&  x <= 'z') || (x >= 'A' && x <= 'Z'))
    {
        *command = ZERO;
        if (ParseCommand(command))
        {
            if (*command == AT || *command == DEG_BY || *command == COMPOSE)
            {
                return ParseArgument(command, &p->c);
            }

            if ((x = getchar()) != '\n')
            {
                ParseLineIgnore(x);
                return WRONGCOMMAND;
            }
            return COMMAND;
        }
        else
        {
            return WRONGCOMMAND;
        }
    }
    else
    {
        unsigned d = 1;
        if (ParsePoly(p, &d))
        {
            if ((x = getchar()) != '\n')
            {
                *c = d;
                PolyDestroy(p);
                ParseLineIgnore(x);
                return WRONGPOLY;
            }
            return POLY;
        }

        *c = d;
        PolyDestroy(p);
        return WRONGPOLY;
    }
}

static void ListPrint(const List l);

/**
 * Pomocnicza do wypisywania wielomianu.
 * @param[in] p : wielomian
 */
static void PolyPrintHelp(const Poly *p)
{
    if (PolyIsCoeff(p))
    {
        printf("%ld", p->c);
    }
    else
    {
        if (p->c != 0) printf("(%ld,0)+", p->c);
        ListPrint(p->l);
    }
}

/**
 * Wypisuje liste jednomianów.
 * @param[in] l : lista jednomianów
 */
static void ListPrint(const List l)
{
    Node *k = l;
    while(k != NULL)
    {
        printf("(");
        PolyPrintHelp(&(k->m.p));
        printf(",%d)", k->m.exp);
        if (k->next != NULL)
        {
            printf("+");
        }
        k = k->next;
    }
}

void PolyPrint(const Poly *p)
{
    PolyPrintHelp(p);
    printf("\n");
}