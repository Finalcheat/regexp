/**
 * Regular Expression Matching Can Be Simple And Fast
 * https://swtch.com/~rsc/regexp/regexp1.html
 **/

#include <stdio.h>
#include "regexp.h"


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage: nfa pattern string\n\nExamples:\n  nfa 'ab.c.d.' 'abcd'\n  nfa 'ab.a.b.ab.b.b.|' 'abbb'\n");
        return 1;
    }

    struct State* start = post2nfa(argv[1]);
    if (start == NULL)
    {
        fprintf(stderr, "bad regexp %s\n", argv[1]);
        return 2;
    }

    if (match(start, argv[2]))
    {
        printf("%s -> %s\n", argv[1], argv[2]);
    }

    return 0;
}
