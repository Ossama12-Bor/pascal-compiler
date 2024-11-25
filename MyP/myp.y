%{

#include "Table_des_symboles.h"


#include <stdio.h>
#include <stdlib.h>
  
extern int yylex();
extern int yyparse();

void yyerror (char* s) {
  printf ("Erreur: %s\n",s);
  exit(0);
  }
		
int depth=0; // block depth
int offset =0 ;//var global offset 

int makeLabel ()
{
  static int n = 0;
  return n++;
}



  char * start_main=  
"int main() {\n\
/* Starting (main) program target PCode */\n";
    
  char * end_main=  
"\n\
/* Stoping (main) program target PCode */\n\
return stack[sp-1].int_value;\n\
}\n";  



 
%}

%union { 
  struct ATTRIBUTE * symbol_value;
  char * string_value;
  int int_value;
  float float_value;
  int type_value;
  int label_value;
  int offset_value;
}

%token <int_value> NUM
%token <float_value> DEC

%token PRG FUN VAR BIN BOUT AFF CLN DOT

%token INT FLOAT VOID

%token <string_value> ID
%token PO PF PV VIR
%token <label_value> IF ELSE WHILE

%token <label_value> AND OR NOT DIFF EQUAL SUP INF
%token PLUS MOINS STAR DIV
%token ARR

%nonassoc IFX
%left OR                       // higher priority on ||
%left AND                      // higher priority on &&
%left DIFF EQUAL SUP INF       // higher priority on comparison
%left PLUS MOINS               // higher priority on + - 
%left STAR DIV                 // higher priority on * /
%left DOT ARR                  // higher priority on . and -> 
%nonassoc UNA                  // highest priority on unary operator
%nonassoc ELSE


%{
char * type2string (int c) {
  switch (c)
    {
    case INT:
      return("int");
    case FLOAT:
      return("float");
    case VOID:
      return("void");
    default:
      return("type error");
    }  
};

  
  %}


%start prog  

// liste de tous les non terminaux dont vous voulez manipuler l'attribut
%type <type_value> type exp  typename
%type <string_value> fun_head ida
%type <label_value> if while

 /* Attention, la rêgle de calcul par défaut $$=$1 
    peut créer des demandes/erreurs de type d'attribut */

%%

 // I. Programme (top level)

prog : prog_head prog_body;

prog_head : PRG ID PV
;

prog_body : opt_funs
            {printf("%s\n",start_main);}
            opt_vars
	    BIN inst_list BOUT DOT
	    {printf("%s\n",end_main);}
            
;


// II. Declarations de variables

opt_vars : var decl_list
|
;

var: VAR // ici on entre dans des declarations de variables
;

decl_list : decl_list decl   {} 
| decl                       {}
;

decl: ID CLN type PV             {attribute valeur;
  if( $3 == INT ){
  valeur = set_symbol_value(string_to_sid($1), makeSymbol($3,offset++, depth));
  printf("LOADI(0)\n");}
  else if ( $3==FLOAT ){
    valeur = set_symbol_value(string_to_sid($1), makeSymbol($3,offset++, depth));
    printf("LOADF(0.0)\n");}
}
;

type
: typename                     {}
;

typename
: INT                          {$$=INT;}
| FLOAT                        {$$=FLOAT;}
| VOID                         {$$=VOID;}
;


// III. Déclarations de functions

opt_funs : opt_funs function
| {printf("/* (Optionnal) functions' target PCode */\n\n");}
;

function : fun_head fun_body {}
;

fun_head : fun fun_name opt_arg CLN type {}
;

fun:FUN

fun_name : ID
;

opt_arg : PO arg_list PF
| ;

arg_list : arg vir arg_list
	   // récursion droite pour numéroter les paramètres du dernier au premier
| arg

arg : ID CLN type              {}
;

vir : VIR                      {}
;

fun_body : opt_vars BIN inst_list BOUT PV    {}
;

// IV. Instructions


inst_list: inst_list inst PV   {} 
| inst PV                      {}
;

     // chaque instruction se termine par ";" contrairement au langage C

inst:
 {depth++;printf("SAVEBP\n");} 
 opt_vars  block_begin inst_list  block_end   {{printf("RESTOREBP\n");
                                              remove_local_variables(depth); depth--;}}
| aff                         {}
| ret                         {}
| cond                        {}
| loop                        {}
;

// Entrée et sortie explicite d'un bloc

block_begin : BIN 
;
// entrée dans un sous-bloc

block_end : BOUT 
;
// sortie d'un sous-bloc


// IV.1 Affectations

aff : ida AFF exp               {sid id = string_to_sid($1);
                                attribute valeur = get_symbol_value(id);
                                if(valeur->type != $3) {fprintf(stderr,"Erreur: Type error in affectation\n"); exit(-1);}
                                printf("STOREP\n");}
;
ida : ID                        {$$ = $1;
                                sid id = string_to_sid($1);
                                attribute valeur = get_symbol_value(id);
                                printf("LOADBP\n");
                                for(int i=0;i<depth-valeur->depth;i++){printf("LOADP\n");}
                                printf("SHIFT(%d)\n",valeur->offset);}
;

// IV.2. Conditionelles
//           N.B. ces rêgles génèrent un conflit déclage reduction
//           qui est résolu comme on le souhaite par un décalage (shift)
//           avec ELSE en entrée (voir y.output)

cond :
if bool_cond inst  elsop       {printf("End_%d:\n",$1);}
;

elsop : else inst              {}
|                  %prec IFX   {} // juste un "truc" pour éviter le message de conflit shift / reduce
;

bool_cond : PO exp PF         { printf("IFN(False_%d)\n",$<label_value>0);}
;

if : IF                       {$$ = makeLabel();}
;

else : ELSE                   {printf("GOTO(End_%d)\nFalse_%d:\n",$<label_value>-2,$<label_value>-2);}
;

// IV.3. Iterations

loop : while while_cond inst  {printf("GOTO(StartLoop_%d)\nEndLoop_%d:\n",$1,$1);}
;

while_cond : PO exp PF        {printf("IFN(EndLoop_%d)\n",$<label_value>0);}

while : WHILE                 {$$ = makeLabel();printf("StartLoop_%d:\n",$$);}
;

// IV.4. Valeur

ret : exp
;



// V. Expressions

exp
// V.1 Exp. arithmetiques
: MOINS exp %prec UNA         {$$ = $2;
                                if ($2 == INT) {printf("MINUSI\n");} 
                                else if ($2 == FLOAT) {printf("MINUSF\n");}}
         // -x + y lue comme (- x) + y  et pas - (x + y)
| exp PLUS exp                {if($1 == FLOAT && $3 == FLOAT){$$ = FLOAT;printf("ADDF\n");}
                              else if($1== FLOAT){ $$ = FLOAT ;printf("I2F2\nADDF\n");}
                              else if($3 == FLOAT){$$ = FLOAT; printf("I2F1\nADDF\n");}
                              else {$$ = INT;printf("ADDI\n");} }

| exp MOINS exp               {if($1 == FLOAT && $3 == FLOAT){$$ = FLOAT;printf("SUBF\n");}
                              else if($1== FLOAT){ $$ = FLOAT ;printf("I2F2\nSUBF\n");}
                              else if($3 == FLOAT){$$ = FLOAT; printf("I2F1\nSUBF\n");}
                              else {$$ = INT;printf("SUBI\n");} }

| exp STAR exp                {if($1 == FLOAT && $3 == FLOAT){$$ = FLOAT;printf("MULTF\n");}
                              else if($1== FLOAT){ $$ = FLOAT ;printf("I2F2\nMULTF\n");}
                              else if($3 == FLOAT){$$ = FLOAT; printf("I2F1\nMULTF\n");}
                              else {$$ = INT;printf("MULTI\n");} }

| exp DIV exp                 {if($1 == FLOAT && $3 == FLOAT){$$ = FLOAT;printf("DIVF\n");}
                              else if($1== FLOAT){ $$ = FLOAT ;printf("I2F2\nDIVF\n");}
                              else if($3 == FLOAT){$$ = FLOAT; printf("I2F1\nDIVF\n");}
                              else {$$ = INT;printf("DIVI\n");} }
| PO exp PF                   {$$ = $2;}
| ID                          { attribute valeur = get_symbol_value(string_to_sid($1));
                              $$ = valeur->type;
                              printf("LOADBP\n");
                              for(int i=0;i<valeur->depth-depth;i++){printf("LOADP\n");}
                              printf("SHIFT(%i)\n",get_symbol_value(string_to_sid($1))->offset); printf("LOADP\n");
                              }
| app                         {}
| NUM                         {$$ = INT;printf("LOADI(%i)\n",$1);}
| DEC                         {$$ = FLOAT;printf("LOADF(%f)\n",$1);}


// V.2. Booléens

| NOT exp %prec UNA           {printf("NOP\n");}
| exp INF exp                 {if($1 == $3){if($1 == INT) printf("LTI\n"); else printf("LTF\n");} 
                              else {fprintf(stderr,"Erreur: Type error in comparaison\n"); exit(-1);}}
| exp SUP exp                 {if($1 == $3){if($1 == INT) printf("GTI\n"); else printf("GTF\n");} 
                              else {fprintf(stderr,"Erreur: Type error in comparaison\n"); exit(-1);}}
| exp EQUAL exp               {if($1 == $3){if($1 == INT) printf("EQI\n"); else printf("EQF\n");} 
                              else {fprintf(stderr,"Erreur: Type error in comparaison\n"); exit(-1);}}
| exp DIFF exp                {if($1 == $3){if($1 == INT) printf("NEQI\n"); else printf("NEQF\n");} 
                              else {fprintf(stderr,"Erreur: Type error in comparaison\n"); exit(-1);}}
| exp AND exp                 {printf("AND\n");}
| exp OR exp                  {printf("OR\n");}

;

// V.3 Applications de fonctions


app : fid PO args PF          {}
;

fid : ID                      {}

args :  arglist               {}
|                             {}
;

arglist : arglist VIR exp     {} // récursion gauche pour empiler les arguements de la fonction de gauche à droite
| exp                         {}
;



%% 
int main () {

  /* Ici on peut ouvrir le fichier source, avec les messages 
     d'erreur usuel si besoin, et rediriger l'entrée standard 
     sur ce fichier pour lancer dessus la compilation.
  */
  
  
printf("#include \"PCode.h\"\n\n");
    	 
return yyparse ();

} 

