
#ifndef _REGEXP_H_
#define _REGEXP_H_


enum
{
    Match = 256,
    Split = 257
};


typedef struct State
{
    int c;
    struct State* out;
    struct State* out1;
    int lastlist;
} State;


typedef union Ptrlist Ptrlist;


union Ptrlist
{
    Ptrlist* next;
    struct State* s;
};


typedef struct Frag
{
    struct State* start;
    Ptrlist* out;
} Frag;


State* post2nfa(char* postfix);

int match(State* start, char* s);



#endif
