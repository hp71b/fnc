CFLAGS = -Wall -Wextra -Werror

fnc: lex.yy.o y.tab.o util.o axem.o
	$(CC) $(CFLAGS) -o fnc util.o axem.o y.tab.o lex.yy.o -ll
	beep

axem.o: axem.c symb.h y.tab.h

util.o: util.c

lex.yy.o: fnc.l y.tab.h
	lex fnc.l
	$(CC) $(CFLAGS) -c lex.yy.c

y.tab.o y.tab.h: fnc.y y.tab.h
	yacc fnc.y
	$(CC) $(CFLAGS) -c y.tab.c

test:
	fnc <test.fn
	aas a
	acp lex

clean:
	rm -f lex.yy* y.tab* *.o
	rm -f fnc
	rm -f *.ao *.as lex
