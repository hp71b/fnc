#include <stdio.h>
#include "symb.h"
#include "y.tab.h"

FILE *fout ;
int nb_par ;
int retflag ;
int label = 0;
int varcnt = 0;
int mainfn = 0;

#define MAXLBL 100
int lblstack [MAXLBL] ;
int lblsp ;
#define MAXCALL 100
int callstack [MAXCALL] ;
int callsp ;
int adresse ;
int code_never_reached ;

generer (label, op, mod)
char *label, *op, *mod ;
{
	if (*label)	fprintf (fout, label) ;
	if (*op)
	{
		putc ('\t', fout) ;
		fprintf (fout, op) ;
		if (*mod)
		{
			putc ('\t', fout) ;
			fprintf (fout, mod) ;
		}
	}
	putc ('\n', fout) ;
}

declarer (symbole, type)
int symbole, type ;
{
	char tmp [100], tmp1 [100], tmp2 [100] ;

	if (stable [symbole].param)
	{
		sprintf (tmp, "%s already defined", stable [symbole].name) ;
		yyerror (tmp) ;
	}
	stable [symbole].param = type ;
	sprintf (tmp1, "var%03d", varcnt) ;
	sprintf (tmp2, "%d", adresse) ;
	stable [symbole].adr = varcnt++ ;
	generer (tmp1, "EQU", tmp2) ;
	adresse += 5 ;
}

generer_comm (com)
char *com ;
{
	generer (com, "", "") ;
}

generer_pop (reg)
char *reg ;
{
	char tmp [100] ;

	sprintf (tmp, "%s=DAT1", reg) ;
	generer ("", tmp, "A") ;
	generer ("", "D1=D1+", "5") ;
}

generer_push (reg)
char *reg ;
{
	char tmp [100] ;

	generer ("", "D1=D1-", "5") ;
	sprintf (tmp, "DAT1=%s", reg) ;
	generer ("", tmp, "A") ;
}

init_output ()
{
	if ((fout = fopen ("a.as", "w")) == NULL)
		yyerror ("cannot open source file") ;
}

generer_getadr (v)
int v ;
{
	char tmp [100] ;

	switch (stable [v].param)
	{
		case 0 :
			sprintf(tmp,"variable %s not declared",stable [v].name);
			yyerror (tmp) ;
			break ;
		case IS_FN :
			sprintf(tmp,"%s is the function name",stable [v].name);
			yyerror (tmp) ;
			break ;
	}

	sprintf (tmp, "* adresse de la variable %s", stable [v].name) ;
	generer (tmp, "", "") ;
	generer ("", "D0=(5)", "=VARADR") ;
	generer ("", "A=DAT0", "A") ;
	sprintf (tmp, "var%03d", stable [v].adr) ;
	generer ("", "LC(5)", tmp) ;
	generer ("", "A=A-C", "A") ;
	generer ("", "D0=A", "") ;
}

x_entete ()
{
}

x_nomfonction (fn)
int fn ;
{
	char tmp [100] ;
	int i ;

	stable [fn].param = IS_FN ;
	mainfn = ! strcmp (stable [fn].name, "main") ;
	adresse = 0 ;
	nb_par = 0 ;
	lblsp = 0 ;
	callsp = 0 ;
	code_never_reached = 0 ;
	sprintf (tmp, "=_%s", stable [fn].name) ;
	generer (tmp, "", "") ;
	generer ("", "A=0", "A") ;	/* pour l'init. des var. locales */
}

x_declarer_par (par)
int par ;
{
	declarer (par, IS_PARAM) ;
	nb_par++ ;
}

x_declarer_var (var)
int var ;
{
	declarer (var, IS_VAR) ;
	generer_push ("A") ;
}

x_endfn ()
{
	char tmp [100] ;
	int i ;

	if (!code_never_reached)
	{
		generer ("", "D1=D1-", "5") ;
		x_return () ;
	}
	i = 0 ;
	while (stable [i].tok)
	{
		switch (stable [i].param)
		{
			case 0 :
			case IS_FN :
				break ;
			case IS_VAR :
			case IS_PARAM :
				stable [i].param = 0 ;
				break ;
		}
		i++ ;
	}
	if (mainfn)
	{
		for (i=0; i<nb_par; i++)
			tmp [i] = '8' ;
		sprintf (tmp + nb_par, "%X%X", nb_par, nb_par) ;
		generer ("", "NIBHEX", tmp) ;
		generer ("=entree", "GOTO", "=prologue") ;
	}
}

x_return ()
{
	char tmp [100] ;

	generer_comm ("* sortie de la fonction") ;
	generer ("", "D0=(5)", "=VARADR") ;
	generer ("", "C=DAT0", "A") ;
	generer ("", "D0=C", "") ;
	generer ("", "D0=D0+", "10") ;		/* D0 := ^ adresse de retour */
	generer ("", "C=DAT0", "A") ;
	generer ("", "RSTK=C", "") ;
	generer ("", "RTN", "") ;
	code_never_reached = (lblsp == 0) ;
}

x_affectation (var)
int var ;
{
	char tmp [100] ;

	switch (stable [var].param)
	{
		case 0 :
			sprintf (tmp, "var. %s not declared",stable [var].name);
			yyerror (tmp) ;
			return ;
		case IS_FN :
			sprintf (tmp,"var. %s is a function",stable [var].name);
			yyerror (tmp) ;
			return ;
	}
	generer_getadr (var) ;
	generer ("", "A=DAT1", "A") ;
	generer ("", "DAT0=A", "A") ;
}

x_mul ()
{	generer_pop ("A") ;
	generer_pop ("C") ;
	generer ("", "GOSBVL", "=A-MULT") ;
	generer_push ("A") ;
}

x_div ()
{
	char tmp [100] ;

	generer_pop ("C") ;		/* diviseur */
	generer_pop ("A") ;		/* dividende */
	generer ("", "?C#0", "A") ;
	mklbl (tmp, label++) ;
	generer ("", "GOYES", tmp) ;
	generer ("", "LC(2)", "=eZRO/0") ;
	generer ("", "GOVLNG", "=MFERR") ;
	generer (tmp, "GOSBVL", "=IDIVA") ;
	generer ("", "P=", "0") ;
	generer_push ("A") ;
}

x_mod ()
{
	char tmp [100] ;

	generer_pop ("C") ;		/* diviseur */
	generer_pop ("A") ;		/* dividende */
	generer ("", "?C#0", "A") ;
	mklbl (tmp, label++) ;
	generer ("", "GOYES", tmp) ;
	generer ("", "LC(2)", "=eZRO/0") ;
	generer ("", "GOVLNG", "=MFERR") ;
	generer (tmp, "GOSBVL", "=IDIVA") ;
	generer ("", "P=", "0") ;
	generer_push ("C") ;
}

x_chs ()
{	generer_pop ("A") ;
	generer ("", "A=-A", "A") ;
	generer_push ("A") ;
}

x_ident (nom)
int nom ;
{
	generer_getadr (nom) ;
	generer ("", "A=DAT0", "A") ;
	generer_push ("A") ;
}

x_const (const)
int const ;
{
	char tmp [100] ;

	sprintf (tmp, "%d", stable [const].num) ;
	generer ("", "LC(5)", tmp) ;
	generer_push ("C") ;
}

x_beginwhile ()
{
	int l ;
	char tmp [100] ;

	mklbl (tmp, l = label++) ;
	pushlbl (l) ;
	generer (tmp, "", "") ;
}

x_midwhile ()
{
	int l1, l2 ;
	char tmp1 [100], tmp2 [100] ;

	mklbl (tmp1, l1 = label++) ;
	mklbl (tmp2, l2 = label++) ;
	generer_pop ("A") ;
	generer ("", "?A#0", "A") ;
	generer ("", "GOYES", tmp1) ;
	generer ("", "GOTO", tmp2) ;
	generer (tmp1, "", "") ;
	pushlbl (l2) ;
}

x_endwhile ()
{
	int l1, l2 ;
	char tmp1 [100], tmp2 [100] ;

	l2 = poplbl () ;
	l1 = poplbl () ;
	mklbl (tmp1, l1) ;
	mklbl (tmp2, l2) ;
	generer ("", "GOTO", tmp1) ;
	generer (tmp2, "", "") ;
}

x_beginfor ()
{
	int l ;
	char tmp [100] ;

	mklbl (tmp, l = label++) ;
	pushlbl (l) ;
	generer ("", "D1=D1+", "5") ;
	generer (tmp, "", "") ;
}

x_mid1for ()
{
	int l, l1, l2, l3 ;
	char tmp [100], tmp1 [100], tmp2 [100], tmp3[100] ;

	mklbl (tmp, l  = label++) ;
	mklbl (tmp1, l1 = label++) ;
	mklbl (tmp2, l2 = label++) ;
	mklbl (tmp3, l3 = label++) ;
	generer_pop ("A") ;
	generer ("", "?A#0", "A") ;
	generer ("", "GOYES", tmp) ;
	generer ("", "GOTO", tmp1) ;
	generer (tmp, "GOTO", tmp2) ;
	generer (tmp3, "", "") ;
	pushlbl (l1) ;
	pushlbl (l2) ;
	pushlbl (l3) ;
}

x_mid2for ()
{
	int l1, l2, l3, l4 ;
	char tmp2 [100], tmp4 [100] ;

	l3 = poplbl () ;
	l2 = poplbl () ;
	l1 = poplbl () ;
	l4 = poplbl () ;
	mklbl (tmp2, l2) ;
	mklbl (tmp4, l4) ;
	generer ("", "D1=D1+", "5") ;
	generer ("", "GOTO", tmp4) ;
	generer (tmp2, "", "") ;
	pushlbl (l1) ;
	pushlbl (l3) ;
}

x_endfor ()
{
	int l1, l3 ;
	char tmp1 [100], tmp3 [100] ;

	l3 = poplbl () ;
	l1 = poplbl () ;
	mklbl (tmp1, l1) ;
	mklbl (tmp3, l3) ;
	generer ("", "GOTO", tmp3) ;
	generer (tmp1, "", "") ;
}

x_beginif ()
{
	int l ;
	char tmp1 [100], tmp2 [100] ;

	mklbl (tmp1, label++) ;
	mklbl (tmp2, l = label++) ;
	pushlbl (l) ;

	generer_pop ("A") ;
	generer ("", "?A#0", "A") ;
	generer ("", "GOYES", tmp1) ;
	generer ("", "GOTO", tmp2) ;
	generer (tmp1, "", "") ;
}

x_endif ()
{
	int l ;
	char tmp [100] ;

	mklbl (tmp, poplbl ()) ;
	generer (tmp, "", "") ;
}

mklbl (name, l)
char *name ;
int l ;
{
	sprintf (name, "lbl%03d", l) ;
}
pushlbl (l)
int l ;
{
	if (lblsp < MAXLBL) lblstack [lblsp++] = l ;
	else yyerror ("label stack overflow") ;
}

int poplbl ()
{
	if (lblsp) return lblstack [--lblsp] ;
	yyerror ("mismatched structure") ;
}

pushcall (fn)
int fn ;
{
	if (callsp < MAXCALL) callstack [callsp++] = fn ;
	else yyerror ("call stack overflow") ;
}

int popcall ()
{
	if (callsp) return callstack [--callsp] ;
	yyerror ("mismatched function calls") ;
}

x_preinc (var)
int var ;
{
	generer_getadr (var) ;
	generer ("", "A=DAT0", "A") ;
	generer ("", "A=A+1", "A") ;
	generer ("", "DAT0=A", "A") ;
	generer_push ("A") ;
}

x_predec (var)
int var ;
{
	generer_getadr (var) ;
	generer ("", "A=DAT0", "A") ;
	generer ("", "A=A-1", "A") ;
	generer ("", "DAT0=A", "A") ;
	generer_push ("A") ;
}

x_postinc (var)
int var ;
{
	generer_getadr (var) ;
	generer ("", "A=DAT0", "A") ;
	generer_push ("A") ;
	generer ("", "A=A+1", "A") ;
	generer ("", "DAT0=A", "A") ;
}

x_postdec (var)
int var ;
{
	generer_getadr (var) ;
	generer ("", "A=DAT0", "A") ;
	generer_push ("A") ;
	generer ("", "A=A-1", "A") ;
	generer ("", "DAT0=A", "A") ;
}

x_op (op)
char *op ;
{
	char tmp [100] ;

	generer_pop ("C") ;
	generer_pop ("A") ;
	sprintf (tmp, "A=A%sC", op) ;
	generer ("", tmp, "A") ;
	generer_push ("A") ;
}

x_cmp (op)
char *op ;
{
	char tmp [100] ;

	generer_pop ("C") ;
	generer_pop ("A") ;
	generer ("", "D=0", "A") ;
	sprintf (tmp, "?C%sA", op) ;
	generer ("", tmp, "A") ;
	mklbl (tmp, label++) ;
	generer ("", "GOYES", tmp) ;
	generer ("", "D=D+1", "A") ;
	generer (tmp, "C=D", "A") ;
	generer_push ("C") ;
}

x_affadd (var)
int var ;
{
}

x_affsub (var)
int var ;
{
}

x_xor ()
{
}

x_appel1 (fn)
int fn ;
{
	char tmp [100], tmp1 [100], tmp2 [100] ;
	int l1, l2 ;

	switch (stable [fn].param)
	{
		case 0 :
			stable [fn].param = IS_FN ;
			break ;
		case IS_FN :
			break ;
		default :
			sprintf (tmp, "%s is not a function", stable [fn].name);
			yyerror (tmp) ;
			break ;
	}
	mklbl (tmp1, l1 = label++) ;
	mklbl (tmp2, l2 = label++) ;	/* adresse de retour */
	sprintf (tmp, "* appel de la fonction %s", stable [fn].name) ;
	generer_comm (tmp) ;
	generer_comm ("*   calcul de l'adresse de retour") ;
	generer ("", "GOSUB", tmp1) ;
	generer (tmp1, "C=RSTK", "") ;
	generer ("", "A=C", "A") ;
	sprintf (tmp, "(%s)-(%s)", tmp2, tmp1) ;
	generer ("", "LC(5)", tmp) ;
	generer ("", "A=A+C", "A") ;

	generer_comm ("*   ecriture du bloc de controle") ;
	generer_push ("A") ;		/* l'adresse de retour */
	generer ("", "D0=(5)", "=VARADR") ;
	generer ("", "C=DAT0", "A") ;
	generer_push ("C") ;

	generer_comm ("*   nouvelle adresse des variables") ;
	generer ("", "D1=D1-", "5") ;
	generer ("", "CD1EX", "") ;
	generer ("", "D0=(2)", "=SVVADR") ;
	generer ("", "DAT0=C", "A") ;
	generer ("", "D1=C", "") ;
	generer ("", "D1=D1+", "5") ;
	generer_comm ("*   passage des parametres") ;
	pushlbl (l2) ;
	pushcall (fn) ;
}

x_appel2 ()
{
	/* rien */
}

x_appel3 ()
{
	int l, fn ;
	char tmp [100] ;

	l = poplbl () ;
	fn = popcall () ;
	generer_comm ("*   mise en place de la nouvelle zone des variables") ;
	generer ("", "D0=(5)", "=SVVADR") ;	/* sauvegarde de VARADR */
	generer ("", "A=DAT0", "A") ;
	generer ("", "D0=(2)", "=VARADR") ;
	generer ("", "DAT0=A", "A") ;

	generer_comm ("*   branchement dans l'appelee") ;
	sprintf (tmp, "=_%s", stable [fn].name) ;
	generer ("", "GOTO", tmp) ;
	sprintf (tmp, "*   retour de la fonction %s", stable [fn].name) ;
	generer_comm (tmp) ;
	mklbl (tmp, l) ;
	generer (tmp, "", "") ;

/* code de retour de l'appelee */
	generer_comm ("*   le resultat") ;
	generer_pop ("A") ;			/* le resultat de la fn */
	generer_comm ("*   changement de pile") ;
	generer ("", "D0=(5)", "=VARADR") ;
	generer ("", "C=DAT0", "A") ;
	generer ("", "D1=C", "") ;
	generer ("", "D1=D1+", "5") ;
	generer_pop ("C") ;			/* VARADR */
	generer ("", "DAT0=C", "A") ;
	generer ("", "D1=D1+", "5") ;
	generer_comm ("*   mettre le resultat sur la pile de l'appelante") ;
	generer_push ("A") ;			/* le resultat */
	generer ("*   la fonction reprend", "", "") ;
}

x_actualiser_var ()
{
}
