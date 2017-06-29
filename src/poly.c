/** @file
    Implementacja interfejsu klasy wielomianów

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-06-03
 */

#include <stdlib.h>
#include <assert.h>

#include "poly.h"
#include "utils.h"

/**
 * Daje większą z dwóch liczb.
 * @param[in] a : liczba
 * @param[in] b : liczba
 * @return max(a, b)
 */
static inline poly_exp_t Max(poly_exp_t a, poly_exp_t b)
{
	return a < b ? b : a;
}

/**
 * Podnosi bazę do potęgi.
 * @param[in] x : baza
 * @param[in] exp : potęga
 * @return x^exp
 */
static poly_coeff_t Power(poly_coeff_t x, poly_exp_t exp)
{
    poly_coeff_t res = 1;

    while (exp != 0)
    {
        if (exp & 1)
            res *= x;
        exp >>= 1;
        x *= x;
    }

    return res;
}

List ListCreate()
{
    return NULL;
}

bool ListIsEmpty(const List l)
{
    return l == ListCreate();
}

void ListDestroy(List l)
{
    if (!ListIsEmpty(l))
    {
        ListDestroy(l->next);
        MonoDestroy(&(l->m));
        free(l);
    }
}

Node* NodeCreate(Mono *m, Node *next)
{
    Node* new = (Node*) malloc(sizeof(Node));

    assert(new != NULL);

    new->m = *m;
    new->next = next;

    return new;
}

Node* NodeClone(Node* n)
{
    Mono m = MonoClone(&(n->m));

    return NodeCreate(&m, NULL);
}

List ListClone(const List l)
{
    if (!ListIsEmpty(l))
    {
        List res = NodeClone(l);
        res->next = ListClone(l->next);
        return res;
    }
    else
    {
        return ListCreate();
    }
}

Mono* ListToArray(List l, unsigned *size)
{
    *size = ListLen(l);
    Mono* res = (Mono*)calloc(*size, sizeof(Mono));
    assert(res != NULL);
    unsigned i = 0;
    List p = l;
    while (p != NULL)
    {
        res[i++] = p->m;
        List tmp = p->next;
        free(p);
        p = tmp;
    }

    return res;
}

unsigned ListLen(const List l)
{
    return ListIsEmpty(l) ? 0 : 1 + ListLen(l->next);
}

List ListMerge(const List p, const List q)
{
    if (ListIsEmpty(p) && ListIsEmpty(q))
    {
        return NULL;
    }
    else if (ListIsEmpty(p))
    {
        if (!PolyIsZero(&q->m.p))
        {
            Mono mono = MonoClone(&(q->m));
            return NodeCreate(&mono, ListMerge(p, q->next));
        }
        else
        {
            return ListMerge(p, q->next);
        }
    }
    else if (ListIsEmpty(q))
    {
        if (!PolyIsZero(&(p->m.p)))
        {
            Mono mono = MonoClone(&(p->m));
            return NodeCreate(&mono, ListMerge(p->next, q));
        }
        else
        {
            return ListMerge(p->next, q);
        }
    }
    if (p->m.exp == q->m.exp)
    {
        Poly tmp = PolyAdd(&(p->m.p), &(q->m.p));
        if (!PolyIsZero(&tmp))
        {
            Mono m = MonoFromPoly(&tmp, p->m.exp);
            return NodeCreate(&m, ListMerge(p->next, q->next));
        }
        else
        {
            PolyDestroy(&tmp);
            return ListMerge(p->next, q->next);
        }
    }
    else
    {
        if (p->m.exp < q->m.exp)
        {
            Mono m = MonoClone(&(p->m));
            return NodeCreate(&m, ListMerge(p->next, q));
        }
        else
        {
            Mono m = MonoClone(&(q->m));
            return NodeCreate(&m, ListMerge(p, q->next));
        }
    }
}

List ListAddMonos(List l, Mono *m)
{
    if (!ListIsEmpty(l))
    {
        Node *prev, *ptr;
        for (ptr = l, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next)
        {
            if (ptr->m.exp == m->exp)
            {
                Poly tmp = PolyAdd(&(ptr->m.p), &(m->p));

                if (PolyIsZero(&tmp))
                {
                    if (prev == NULL)
                        l = ptr->next;
                    else
                        prev->next = ptr->next;

                    Node *tmp_ptr = ptr;
                    ptr = ptr->next;
                    MonoDestroy(&(tmp_ptr->m));
                    free(tmp_ptr);
                    PolyDestroy(&tmp);
                }
                else
                {
                    PolyDestroy(&(ptr->m.p));
                    ptr->m.p = tmp;
                }
                MonoDestroy(m);
                break;
            }
            else
            {
                if (ptr->m.exp > m->exp)
                {
                    if (prev == NULL)
                    {
                        l = NodeCreate(m, l);
                    }
                    else
                    {
                        prev->next = NodeCreate(m, ptr);
                    }
                    break;
                }
            }
        }
        if (ptr == NULL && prev != NULL)
            prev->next = NodeCreate(m, NULL);
    }
    else
    {
        l = NodeCreate(m, NULL);
    }

    return l;
}

inline void PolyDestroy(Poly *p)
{
    ListDestroy(p->l);
}

Poly PolyClone(const Poly *p)
{
    return (Poly) {.c = p->c, .l = ListClone(p->l)};
}

/**
 * Dodaje do wielomianu liczbę.
 * @param[in] p : wielomian
 * @param[in] c : liczba
 * @return `p + c`
 */
static Poly PolyAddCoeff(const Poly *p, poly_coeff_t c) {
    assert(!PolyIsCoeff(p));

    Poly res = PolyClone(p);

    if (c == 0)
        return res;

    Poly tmp1 = PolyFromCoeff(c);

    if (res.l->m.exp == 0)
    {
        Poly tmp2 = PolyAdd(&tmp1, &res.l->m.p);
        PolyDestroy(&tmp1);
        PolyDestroy(&res.l->m.p);
        res.l->m.p = tmp2;
        if (PolyIsZero(&res.l->m.p))
        {
            List tmp = res.l->next;
            MonoDestroy(&res.l->m);
            free(res.l);
            res.l = tmp;
        }
    }
    else
    {
        Mono m = MonoFromPoly(&tmp1, 0);
        res.l = NodeCreate(&m, res.l);
    }

    return res;
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return (Poly) {.l = ListCreate(), .c = p->c + q->c};
    else if (PolyIsCoeff(p))
        return PolyAddCoeff(q, p->c);
    else if (PolyIsCoeff(q))
        return PolyAddCoeff(p, q->c);

    List l = ListMerge(p->l, q->l);
    poly_coeff_t x = 0;

    if (l != NULL && l->next == NULL
        && l->m.exp == 0 && PolyIsCoeff(&l->m.p))
    {
        x = l->m.p.c;
        ListDestroy(l);
        l = NULL;
    }

    return (Poly) {.l = l, .c = x};
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{
    Poly res = PolyZero();

    for (int i = (int)count - 1; i >= 0; i--)
    {
        Mono mono = monos[i];
        if (PolyIsZero(&(mono.p)))
        {
            MonoDestroy(&mono);
            continue;
        }
        res.l = ListAddMonos(res.l, &mono);
    }

    if (!ListIsEmpty(res.l) && res.l->next == NULL
        && res.l->m.exp == 0 && PolyIsCoeff(&(res.l->m.p)))
    {
        res.c += res.l->m.p.c;
        ListDestroy(res.l);
        res.l = NULL;
    }

    return res;
}

/**
 * Pomocnicza do funkcji, mnożącej wielomian przez stałą.
 * @param[in,out] p : wielomian
 * @param[in] c : stała
 */
static void PolyMulCoeffHelp(Poly *p, const poly_coeff_t c)
{
    p->c *= c;

    for (Node *ptr = p->l; ptr != NULL; ptr = ptr->next)
        PolyMulCoeffHelp(&(ptr->m.p), c);
}

/**
 * Usuwa zera z wielomianu
 * @param[in,out] p : wielomian
 */
static void PolyRemoveZeros(Poly *p)
{
    Node *ptr = p->l, *prev = NULL;
    while (ptr != NULL)
    {
        PolyRemoveZeros(&ptr->m.p);
        if (PolyIsZero(&ptr->m.p))
        {
            if (prev == NULL)
            {
                Node *tmp = ptr;
                ptr = ptr->next;
                PolyDestroy(&tmp->m.p);
                free(tmp);
                p->l = ptr;
            }
            else
            {
                ptr = ptr->next;
                PolyDestroy(&prev->next->m.p);
                free(prev->next);
                prev->next = ptr;
            }
        }
        else
        {
            prev = ptr;
            ptr = ptr->next;
        }
    }
}

/**
 * Mnoży jednomian przez stalą.
 * @param[in] p : wielomian
 * @param[in] c : stała
 * @return przemnożony wielomian
 */
static Poly PolyMulCoeff(const Poly *p, const poly_coeff_t c)
{
    if (c == 0)
        return PolyZero();

    Poly res = PolyClone(p);

    PolyMulCoeffHelp(&res, c);

    PolyRemoveZeros(&res);

    return res;
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyFromCoeff(p->c * q->c);
    else if (PolyIsCoeff(p))
        return PolyMulCoeff(q, p->c);
    else if (PolyIsCoeff(q))
        return PolyMulCoeff(p, q->c);

    unsigned n = ListLen(p->l) * ListLen(q->l);
    unsigned k = 0;
    Mono *arr = calloc(n, sizeof(struct Mono));
    
    for (Node *z = p->l; z != NULL; z = z->next)
    {
        for (Node *x = q->l; x != NULL; x = x->next)
            arr[k++] = (Mono) {.p = PolyMul(&(z->m.p), &(x->m.p)),
                               .exp = z->m.exp + x->m.exp};
    }

    Poly res = PolyAddMonos(n, arr);

    free(arr);

    return res;
}

Poly PolyNeg(const Poly *p)
{
    return PolyMulCoeff(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly tmp = PolyNeg(q);
    Poly res = PolyAdd(&tmp, p);

    PolyDestroy(&tmp);

    return res;
}

/**
 * Daje maksymalny stopień na liście jednomianów.
 * @param[in] l : lista jednomianów
 * @return max potęga
 */
static poly_exp_t PolyMaxDeg(const List l)
{
    poly_exp_t res = -1;

    for (Node *ptr = l; ptr != NULL; ptr = ptr->next)
        res = Max(res, ptr->m.exp);

    return res;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
    if (var_idx > 0)
    {
        poly_exp_t res = var_idx == POLY_DEG_MAX ? 0 :  -1;

        for (Node *ptr = p->l; ptr != NULL; ptr = ptr->next)
            res = Max(res, PolyDegBy(&(ptr->m.p), var_idx - 1));
        
        return res;
    }
    else
    {
        return PolyIsZero(p) ? -1 : PolyIsCoeff(p) ? 0 : PolyMaxDeg(p->l);
    }
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (!PolyIsCoeff(p))
    {
        poly_exp_t res = -1;

        for (Node *ptr = p->l; ptr != NULL; ptr = ptr->next)
            res = Max(res, ptr->m.exp + PolyDeg(&(ptr->m.p)));

        return res;
    }
    else
    {
        return PolyIsZero(p) ? -1 : 0;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    Poly tmp = PolySub(p, q);

    bool res = PolyIsZero(&tmp);

    PolyDestroy(&tmp);

    return res;
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    Poly res = PolyFromCoeff(p->c);

    for (Node *ptr = p->l; ptr != NULL; ptr = ptr->next)
    {
        Poly tmp1 = PolyMulCoeff(&(ptr->m.p), Power(x, ptr->m.exp));
        Poly tmp2 = PolyAdd(&res, &tmp1);
        PolyDestroy(&res);
        PolyDestroy(&tmp1);
        res = tmp2;
    }

    return res;
}

void PolyArrayDestroy(unsigned count, Poly x[])
{
    for (unsigned i = 0; i < count; i++)
        PolyDestroy(&x[i]);
    free(x);
}

/**
 * Daje wyraz wolny wielomianu.
 * @param[in] p : wielomian
 * @return @f$p(0)@f$
 */
static Poly PolyConstTerm(const Poly *p)
{
    if (!PolyIsCoeff(p))
        return p->l->m.exp == 0 ? PolyConstTerm(&p->l->m.p) : PolyZero();
    else
        return PolyClone(p);
}

/**
 * Podnosi wielomian do potęgi.
 * @param[in] p : wielomian
 * @param[in] exp : wykładnik
 * @return @f$p^exp@f$
 */
static Poly PolyPower(const Poly *p, poly_exp_t exp)
{
    Poly res = PolyFromCoeff(1);
    Poly tmp;
    Poly q = PolyClone(p);

    while (exp != 0)
    {
        if (exp & 1)
        {
            tmp = PolyMul(&res, &q);
            PolyDestroy(&res);
            res = tmp;
        }
        exp >>= 1;
        tmp = PolyMul(&q, &q);
        PolyDestroy(&q);
        q = tmp;
    }

    PolyDestroy(&q);

    return res;
}

Poly PolyCompose(const Poly *p, unsigned count, const Poly x[])
{
    if (count == 0)
    {
        return PolyConstTerm(p);
    }
    else
    {
        if (PolyIsCoeff(p))
        {
            return PolyClone(p);
        }
        else
        {
            Poly tmp1, tmp2, tmp3, tmp4, res = PolyZero();
            for (Node *ptr = p->l; ptr != NULL; ptr = ptr->next)
            {
                tmp1 = PolyCompose(&ptr->m.p, count - 1, x + 1);
                tmp2 = PolyPower(&x[0], ptr->m.exp);
                tmp3 = PolyMul(&tmp1, &tmp2);
                tmp4 = res;
                res = PolyAdd(&tmp3, &tmp4);
                PolyDestroy(&tmp1);
                PolyDestroy(&tmp2);
                PolyDestroy(&tmp3);
                PolyDestroy(&tmp4);
            }
            return res;
        }
    }
}
