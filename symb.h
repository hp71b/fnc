#define MAXSYMB 100

struct symbtbl
{
	int tok ;
	int num ;
	char name [13] ;
	int param ;
	int adr ;
} ;

/* valeurs de stable [i].param : */

#define IS_FN		1
#define IS_PARAM	2
#define IS_VAR		3

extern struct symbtbl stable [] ;

/*
 * La table des symboles est tres primitive. Elle contient 2 sortes
 * d'objets :
 *
 *								initialise par
 * Les constantes
 *	stable.tok = CONST						lex
 *	stable.num = la valeur numerique de la constante		lex
 *
 * Les identificateurs
 *	stable.tok = IDENT						lex
 *	stable.name = le nom de l'identificateur			lex
 *	stable.param = 1 si la fct, 2 si parametre, 3 si variable	yacc
 *	stable.adr = l'adresse allouee par l'allocateur			yacc
 */
