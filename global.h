#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// lex.yy.c
extern int yylex (void);

// axem.c
extern void generer (char *label, char *op, char *mod) ;
extern void declarer (int symbole, int type) ;
extern void generer_comm (char *com) ;
extern void generer_pop (char *reg) ;
extern void generer_push (char *reg) ;
extern void init_output (void) ;
extern void generer_getadr (int v) ;
extern void x_entete (void) ;
extern void x_nomfonction (int fn) ;
extern void x_declarer_par (int par) ;
extern void x_declarer_var (int var) ;
extern void x_endfn (void) ;
extern void x_return (void) ;
extern void x_affectation (int var) ;
extern void x_mul (void) ;
extern void x_div (void) ;
extern void x_mod (void) ;
extern void x_chs (void) ;
extern void x_ident (int nom) ;
extern void x_const (int cst) ;
extern void x_beginwhile (void) ;
extern void x_midwhile (void) ;
extern void x_endwhile (void) ;
extern void x_beginfor (void) ;
extern void x_mid1for (void) ;
extern void x_mid2for (void) ;
extern void x_endfor (void) ;
extern void x_beginif (void) ;
extern void x_endif (void) ;
extern void mklbl (char *name, int l) ;
extern void pushlbl (int l) ;
extern int poplbl (void) ;
extern void pushcall (int fn) ;
extern int popcall (void) ;
extern void x_preinc (int var) ;
extern void x_predec (int var) ;
extern void x_postinc (int var) ;
extern void x_postdec (int var) ;
extern void x_op (char *op) ;
extern void x_cmp (char *op) ;
extern void x_affadd (int var) ;
extern void x_affsub (int var) ;
extern void x_xor (void) ;
extern void x_appel1 (int fn) ;
extern void x_appel2 (void) ;
extern void x_appel3 (void) ;
extern void x_actualiser_var (void) ;

// util.c
extern void yyerror (char *s) ;
extern void yywhere (void) ;

// lex.yy.c
extern void s_lookup (int tok) ;

// y.tab.c
extern int yyparse (void) ;
