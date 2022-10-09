CFLAGS = -Wall -Wextra -Werror

fnc: lex.yy.o y.tab.o util.o axem.o
	$(CC) $(CFLAGS) -o fnc util.o axem.o y.tab.o lex.yy.o -ll

axem.o: axem.c global.h symb.h y.tab.h

util.o: util.c global.h

lex.yy.c: fnc.l y.tab.h
	lex fnc.l

y.tab.c y.tab.h: fnc.y
	yacc -d fnc.y

test:
	fnc <test.fn
	aas a
	acp lex

clean:
	rm -f lex.yy* y.tab* *.o
	rm -f fnc
	rm -f *.ao *.as lex
	rm -f a.as
