%{
#include "global.h"
%}

%token	'+' 
%token	'='
%token	'{' 
%token	',' 
%token	CONST
%token	'/' 
%token	'}'
%token	FUNCTION
%token	IDENT
%token	'('
%token	'%' 
%token	'*' 
%token	')'
%token	';'
%token	'-' 
%token	VARIABLE 
%token	RETURN
%token	IF
%token	FOR
%token	WHILE
%token	'<'
%token	'>'
%token	GE
%token	LE
%token	EQ
%token	NE
%token	'&'
%token	'|'
%token	'^'
%token	PE
%token	ME
%token	PP
%token	MM

%right '=' PE ME
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' '>' GE LE
%left '+' '-'
%left '*' '/' '%'
%right PP MM

%%

programme
	: fonction
	| programme fonction

fonction
	: entete corps
		{	x_endfn () ;		}

entete
	: FUNCTION IDENT
		{	x_nomfonction ($2) ;	}
	  '(' liste_parametres ')'
		{	x_entete () ;		}

liste_parametres
	: /* vide */
	| liste_par

liste_par
	: IDENT
		{	x_declarer_par ($1) ;	}
	| liste_par ',' IDENT
		{	x_declarer_par ($3) ;	}

corps
	: '{' liste_instructions '}'
	| '{' declarations liste_instructions '}'

declarations
	: VARIABLE liste_var ';'
		{	x_actualiser_var () ;	}

liste_var
	: IDENT
		{	x_declarer_var ($1) ;	}
	| liste_var ',' IDENT
		{	x_declarer_var ($3) ;	}

liste_instructions
	: instruction
	| instruction liste_instructions

instruction
	: RETURN expression ';'
		{	x_return () ;		}
	| boucle_for
	| boucle_while
	| test
	| '{' liste_instructions '}'
	| ';'
	| expression ';'
		{	generer_pop ("A") ;	}

boucle_for
	: FOR '(' expression ';'
		{	x_beginfor () ;		}
	  expression ';'
		{	x_mid1for () ;		}
	  expression ')'
		{	x_mid2for () ;		}
	  instruction
		{	x_endfor () ;		}


boucle_while
	: WHILE	{	x_beginwhile () ;	}
		'(' expression ')'
		{	x_midwhile () ;		}
	  instruction
		{	x_endwhile () ;		}

test
	: IF '(' expression ')'
		{	x_beginif () ;		}
	  instruction
		{	x_endif () ;		}

expression
	: binaire
	| expression ',' binaire

binaire
	: IDENT
		{	x_ident ($1) ;		}
	| CONST
		{	x_const ($1) ;		}
	| IDENT 
		{	x_appel1 ($1) ;		}
	  '(' liste_arg_opt_vide ')'
		{	x_appel3 () ;		}
	| '(' expression ')'
	| PP IDENT
		{	x_preinc ($2) ;		}
	| MM IDENT
		{	x_predec ($2) ;		}
	| binaire '+' binaire
		{	x_op ("+") ;		}
	| binaire '-' binaire
		{	x_op ("-") ;		}
	| binaire '*' binaire
		{	x_mul () ;		}
	| binaire '/' binaire
		{	x_div () ;		}
	| binaire '%' binaire
		{	x_mod () ;		}
	| binaire '<' binaire
		{	x_cmp ("<") ;		}
	| binaire '>' binaire
		{	x_cmp (">") ;		}
	| binaire GE binaire
		{	x_cmp (">=") ;		}
	| binaire LE binaire
		{	x_cmp ("<=") ;		}
	| binaire EQ binaire
		{	x_cmp ("#") ;		}
	| binaire NE binaire
		{	x_cmp ("=") ;		}
	| binaire '&' binaire
		{	x_op ("&") ;		}
	| binaire '^' binaire
		{	x_xor () ;		}
	| binaire '|' binaire
		{	x_op ("!") ;		}
	| IDENT '=' binaire
		{	x_affectation ($1) ;	}
	| IDENT PE binaire
		{	x_affadd ($1) ;		}
	| IDENT ME binaire
		{	x_affsub ($1) ;		}
	| IDENT PP
		{	x_postinc ($2) ;	}
	| IDENT MM
		{	x_postdec ($2) ;	}

liste_arg_opt_vide
	:	/* vide */
	| liste_arg

liste_arg
	: binaire
		{	x_appel2 () ;		}
	| liste_arg ',' binaire
		{	x_appel2 () ;		}
