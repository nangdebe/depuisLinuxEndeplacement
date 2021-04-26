/* Reverse Polish Notation calculator. */
%{
 #include <stdio.h>
 #include <math.h>
 int yylex (void);
 void yyerror (char const *);
%}
%define api.value.type {double}
%token NUM
%token ADD SUB MUL DIV POW NEG
%token EOL
%% /* Grammar rules and actions follow. */
input:
 %empty
| input line
;
line:
 EOL
| exp EOL { printf ("%.10g\n", $1); } ;
exp:
 NUM
| exp exp ADD { $$ = $1 + $2; }
| exp exp SUB { $$ = $1 - $2; }
| exp exp MUL { $$ = $1 * $2; }
| exp exp DIV { $$ = $1 / $2; }
| exp exp POW { $$ = pow ($1, $2); } /* Exponentiation */
| exp NEG { $$ = -$1; } /* Unary minus */
;
%%
#include <ctype.h>
/* Called by yyparse on error. */
void
yyerror (char const *s){
 fprintf (stderr, "%s\n", s);
}
int main (void){
 return yyparse ();
}
