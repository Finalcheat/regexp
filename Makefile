OBJECTS = nfa.o regexp.o
NFA = nfa

${NFA} : ${OBJECTS}
	cc -o ${NFA} ${OBJECTS}

.PHONY : clean
clean : 
	-rm -f ${NFA} ${OBJECTS}
