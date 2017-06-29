/** @file
    Implementacja interfejsu stosu wielomianów

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-05-27
*/

#include <stdlib.h>
#include <assert.h>

#include "stack.h"
#include "utils.h"

Stack StackInit()
{
    return NULL;
}

bool StackIsEmpty(const Stack s)
{
    return s == StackInit();
}

void StackPush(Stack *s, Poly p)
{
    StackNode *tmp = (Stack) malloc(sizeof(StackNode));
    assert(tmp != NULL);
    tmp->next = *s;
    tmp->p = p;
    *s = tmp;
}

Poly StackPop(Stack *s)
{
    StackNode *tmp = *s;
    Poly res = (*s)->p;
    *s = (*s)->next;
    free(tmp);
    return res;
}

void StackDestroy(Stack *s)
{
    while (!StackIsEmpty(*s))
    {
        Poly tmp = StackPop(s);
        PolyDestroy(&tmp);
    }
}

bool StackIsSmallerThan(const Stack s, size_t size)
{
    return size == 0 ? false : (StackIsEmpty(s) ? true : StackIsSmallerThan(s->next, size - 1));
}

Poly* StackPopArray(Stack *s, unsigned count)
{
    Poly* res = (Poly*)calloc(count, sizeof(Poly));
    assert(res != NULL);

    for (unsigned i = 0; i < count; i++)
        res[i] = StackPop(s);

    return res;
}
