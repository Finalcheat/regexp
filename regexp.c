/**
 * Regular Expression Matching Can Be Simple And Fast
 * https://swtch.com/~rsc/regexp/regexp1.html
 **/


#include <stdio.h>
#include <stdlib.h>
#include "regexp.h"


static Ptrlist *list1(State **outp)
{
    Ptrlist* pl = (Ptrlist*)outp;
    pl->next = NULL;
    return pl;
}


static Ptrlist *append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist* l = l1;
    while (l->next)
    {
        l = l->next;
    }
    l->next = l2;
    return l1;
}


static void patch(Ptrlist *l, State *s)
{
    while (l)
    {
        Ptrlist* next = l->next;
        l->s = s;
        l = next;
    }
}


static State* state(int c, State* out, State* out1)
{
    State* s = (State*)malloc(sizeof(*s));

    s->lastlist = 0;
    s->c = c;
    s->out = out;
    s->out1 = out1;
    return s;
}


static Frag frag(State* start, Ptrlist* out)
{
    Frag f = { start, out };
    return f;
}


State* post2nfa(char* postfix)
{
    char *p;
    struct Frag stack[1000], *stackp, e1, e2, e;
    struct State *s;

#define push(x) *stackp++ = x
#define pop()   *--stackp

    stackp = stack;
    for (p = postfix; *p; ++p)
    {
        switch (*p)
        {
            /* compilation cases, described below */
        case '.':
            e2 = pop();
            e1 = pop();
            patch(e1.out, e2.start);
            push(frag(e1.start, e2.out));
            break;
        case '|':
            e2 = pop();
            e1 = pop();
            s = state(Split, e1.start, e2.start);
            push(frag(s, append(e1.out, e2.out)));
            break;
        case '?':
            e = pop();
            s = state(Split, e.start, NULL);
            push(frag(s, append(e.out, list1(&s->out1))));
            break;
        case '*':
            e = pop();
            s = state(Split, e.start, NULL);
            patch(e.out, s);
            push(frag(s, list1(&s->out1)));
            break;
        case '+':
            e = pop();
            s = state(Split, e.start, NULL);
            patch(e.out, s);
            push(frag(e.start, list1(&s->out1)));
            break;
        default:
            s = state(*p, NULL, NULL);
            push(frag(s, list1(&s->out)));
            break;
        }
    }
    e = pop();
    struct State* matchstate = state(Match, NULL, NULL);
    patch(e.out, matchstate);
    return e.start;

#undef pop
#undef push
}


typedef struct List
{
    struct State* s[1000];
    int n;
} List;

static List l1, l2;
static int listid = 0;


static void addstate(List* l, State* s)
{
    if (s == NULL || s->lastlist == listid)
    {
        return;
    }
    s->lastlist = listid;
    if (s->c == Split)
    {
        /* follow unlabeled arrows */
        addstate(l, s->out);
        addstate(l, s->out1);
        return;
    }
    l->s[l->n++] = s;
}


static List* startlist(State* s, List* l)
{
    ++listid;
    l->n = 0;
    addstate(l, s);
    return l;
}


static void step(List* clist, int c, List* nlist)
{
    ++listid;
    nlist->n = 0;
    for (int i = 0; i < clist->n; ++i)
    {
        struct State* s = clist->s[i];
        if (s->c == c)
        {
            addstate(nlist, s->out);
        }
    }
}


static int ismatch(List* l)
{
    for (int i = 0; i < l->n; ++i)
    {
        if(l->s[i]->c == Match)
        {
            return 1;
        }
    }
    return 0;
}


int match(State* start, char* s)
{
    List *clist, *nlist, *t;

    /* l1 and l2 are preallocated globals */
    clist = startlist(start, &l1);
    nlist = &l2;
    for(; *s; ++s)
    {
        step(clist, *s, nlist);
        t = clist; clist = nlist; nlist = t;	/* swap clist, nlist */
    }
    return ismatch(clist);
}
