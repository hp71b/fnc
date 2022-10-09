#include <stdio.h>

extern int yynerrs ;
extern char yytext [] ;
extern int yyleng ;
extern int yylineno ;

main ()
{
	init_output () ;
	yynerrs = 0 ;
	yyparse () ;
}

yyerror (s)
register char *s ;
{
	extern int yynerrs ;

	fprintf (stderr, "[error %d] ", yynerrs+1) ;
	yywhere () ;
	fputs (s, stderr) ;
	putc ('\n', stderr) ;
}

yywhere ()
{
	char colon = 0 ;

	if (yylineno > 0)
	{
		fprintf (stderr, "line %d", yylineno -
					    (*yytext == '\n' || ! *yytext)) ;
		colon = 1 ;
	}
	if (*yytext)
	{
		register int i ;

		for (i=0; i<20; ++i)
			if (!yytext [i] || yytext [i] == '\n')
				break ;
		if (i)
		{
			if (colon) putc (' ', stderr) ;
			fprintf (stderr, "near \"%.*s\"", i, yytext) ;
			colon = 1 ;
		}
	}
	if (colon) fputs (": ", stderr) ;
}
