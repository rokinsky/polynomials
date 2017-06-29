/** @file
    Implementacja kalkulatora działającego na wielomianach i
    stosującego odwrotną notację polską

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-06-03
 */

#include "parse.h"
#include "stack.h"
#include "utils.h"

/** Nieskończona pętla */
#define FOREVER while (1)

/**
 * Funkcja główna.
 * Program zakończy swoje działanie, gdy wczyta EOF.
 * @return
 */
int main(void)
{
    Poly p, q, r, s = PolyZero();
    Command command;
    Stack stack = StackInit();
    unsigned row = 0, col = 0;
    do
    {
        row++;
        switch (ParseLineRead(&s, &command, &col))
        {
            case POLY:
                StackPush(&stack, s);
                s = PolyZero();
                break;
            case WRONGPOLY:
                ErrorWrongPoly(row, col);
                s = PolyZero();
                break;
            case WRONGCOMMAND:
                ErrorWrongCommand(row);
                s = PolyZero();
                break;
            case WRONGVALUE:
                ErrorWrongValue(row);
                s = PolyZero();
                break;
            case WRONGVARIABLE:
                ErrorWrongVariable(row);
                s = PolyZero();
                break;
            case WRONGCOUNT:
                ErrorWrongCount(row);
                s = PolyZero();
                break;
            case COMMAND:
                if (StackIsSmallerThan(stack, ParseNeededArguments(&s, command)))
                {
                    ErrorStackUnderflow(row);
                    break;
                }

                switch (command)
                {
                    case ZERO:
                        StackPush(&stack, PolyZero());
                        break;
                    case IS_COEFF:
                        p = StackPop(&stack);
                        printf("%d\n", PolyIsCoeff(&p));
                        StackPush(&stack, p);
                        break;
                    case IS_ZERO:
                        p = StackPop(&stack);
                        printf("%d\n", PolyIsZero(&p));
                        StackPush(&stack, p);
                        break;
                    case CLONE:
                        p = StackPop(&stack);
                        q = PolyClone(&p);
                        StackPush(&stack, p);
                        StackPush(&stack, q);
                        break;
                    case ADD:
                        p = StackPop(&stack);
                        q = StackPop(&stack);
                        r = PolyAdd(&p, &q);
                        StackPush(&stack, r);
                        PolyDestroy(&p);
                        PolyDestroy(&q);
                        break;
                    case MUL:
                        p = StackPop(&stack);
                        q = StackPop(&stack);
                        r = PolyMul(&p, &q);
                        StackPush(&stack, r);
                        PolyDestroy(&p);
                        PolyDestroy(&q);
                        break;
                    case NEG:
                        p = StackPop(&stack);
                        q = PolyNeg(&p);
                        StackPush(&stack, q);
                        PolyDestroy(&p);
                        break;
                    case SUB:
                        p = StackPop(&stack);
                        q = StackPop(&stack);
                        r = PolySub(&p, &q);
                        StackPush(&stack, r);
                        PolyDestroy(&p);
                        PolyDestroy(&q);
                        break;
                    case IS_EQ:
                        p = StackPop(&stack);
                        q = StackPop(&stack);
                        printf("%d\n", PolyIsEq(&p, &q));
                        StackPush(&stack, q);
                        StackPush(&stack, p);
                        break;
                    case DEG:
                        p = StackPop(&stack);
                        printf("%d\n", PolyDeg(&p));
                        StackPush(&stack, p);
                        break;
                    case DEG_BY:
                        p = StackPop(&stack);
                        printf("%d\n", PolyDegBy(&p, (unsigned)s.c));
                        StackPush(&stack, p);
                        break;
                    case AT:
                        p = StackPop(&stack);
                        q = PolyAt(&p, s.c);
                        PolyDestroy(&p);
                        StackPush(&stack, q);
                        break;
                    case PRINT:
                        p = StackPop(&stack);
                        PolyPrint(&p);
                        StackPush(&stack, p);
                        break;
                    case POP:
                        p = StackPop(&stack);
                        PolyDestroy(&p);
                        break;
                    case COMPOSE:
                        p = StackPop(&stack);
                        Poly *x = StackPopArray(&stack, (unsigned)s.c);
                        StackPush(&stack, PolyCompose(&p, (unsigned)s.c, x));
                        PolyArrayDestroy((unsigned)s.c, x);
                        PolyDestroy(&p);
                        break;
                }
                break;
            case END:
                StackDestroy(&stack);
                return 0;
        }
    }
    FOREVER;
}
