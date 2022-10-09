%{
#ifdef DEBUG

	main ()
	{
		char *p ;

		while (p = (char *) yylex ())
			printf ("%-10.10s is \"%s\"\n", p, yytext) ;
	}

	s_lookup () { }

#	define	token(x)	(int) "x"

#else	// ! DEBUG

#	include "y.tab.h"
#	define	token(x)	x
#	include	"symb.h"

	struct symbtbl stable [MAXSYMB] ;
	extern int yylval ;

	s_lookup (tok)
	int tok ;
	{
		int i, val ;

		if (tok == CONST) val = atoi (yytext) ;

		i = 0 ;
		while (stable [i].tok)
		{
			if (stable [i].tok == tok)
			{
				if (((tok == CONST)&&(stable [i].num == val))
				    || (strcmp (yytext, stable[i].name)==0))
				{
					yylval = i ;
					return ;
				}
			}
			i++ ;
		}
		if (i == MAXSYMB - 1)
		{
			fprintf (stderr, "not enough space for symbol table\n");
			exit (1) ;
		}
		stable [i].tok = tok ;
		if (tok == CONST) stable [i].num = val ;
		else	strcpy (stable [i].name, yytext) ;
		yylval = i ;
		return ;
	}

#endif	// DEBUG

#define	END(v)	(v-1 + sizeof v / sizeof v [0])
static int screen () ;
%}

letter			[a-zA-Z_]
digit			[0-9]
letter_or_digit		[a-zA-Z_0-9]
white_space		[ \t\n]
blank			[ \t]
other			.
exposant		([eE][-+]?[0-9]+)

%%

"/*""/"*([^*/]|[^*]"/"|"*"[^/])*"*"*"*/"	;

"=="			return token (EQ) ;
"!="			return token (NE) ;
"<="			return token (LE) ;
">="			return token (GE) ;
"++"			return token (PP) ;
"--"			return token (MM) ;
"+="			return token (PE) ;
"-="			return token (ME) ;

{letter}{letter_or_digit}*	return screen () ;

{digit}+		{
				s_lookup (token (CONST)) ;
				return token (CONST) ;
			}
{white_space}+		;
{other}			return token (yytext [0]) ;

%%

static struct rwtable
{
	char *rw_name ;
	int rw_yylex ;
} rwtable [] = {	
			"for",		token (FOR),
			"function",	token (FUNCTION),
			"if",		token (IF),
			"return",	token (RETURN) ,
			"variable",	token (VARIABLE),
			"while",	token (WHILE),
		} ;

static int screen ()
{
	struct rwtable	*low = rwtable,
			*high = END(rwtable),
			*mid ;
	int c ;

	while (low <= high)
	{
		mid = low + (high - low)/2 ;
		if ((c = strcmp (mid->rw_name, yytext)) == 0)
			return mid->rw_yylex ;
		else if (c < 0)	low = mid + 1 ;
		else high = mid - 1 ;
	}
	s_lookup (token (IDENT)) ;
	return token (IDENT) ;
}
