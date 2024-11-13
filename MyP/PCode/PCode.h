/*
 *  PCode.h
 * 
 *  Created by Janin on 11/23.
 *  Copyright 2023 ENSEIRB. All rights reserved.
 *
 *  Le PCode présenté ici mime, via des macros C
 *  un langage d'assembleur avec pile.
 *
 *  Les concessions faites au C:
 *    - gestion du CO (conteur ordinal) via les appels de fonctions de C sans arguements
 *    - gestion des types, alignements mémoires, conversion sur la pile via le type union en C
 *      (avec un vrai assembleur il faudrait le faire "à la main"...
 *
 *  La pile de PCode est donc codée comme un tableau C de valeurs typées selon 
 *  un type union qui rassemble les types des valeurs qu'on veut pouvoir stocker 
 *  sur la pile.
 * 
 *  Les "adresses" de PCode sont alors simplement codées comme des indices
 *  dans ce tableau. Autrement dit, on évite, pour la beauté du geste,
 *  d'utiliser explicitement les pointeurs C.
 *
 *  Bien sur stack[k] c'est la même chose que *(stack+k) il y a donc bien des pointeurs C
 *  derrière notre codage du PCode en C.
 *
 *  WARNING : Codage du PCode en l'état, peut-être avec des erreurs (?) 
 *  et susceptible d'être étendu si besoin.
 *  Dans ce cas, il faudra détailler les extensions ou corrections proposées dans le rapport.
 *
 */


#ifndef PCODE_H
#define PCODE_H

#define SIZE 100


typedef union {
  int int_value;
  float float_value;
  int index_value;
}  stack_value;
  

extern stack_value stack[SIZE];
   

extern int sp;
     // Stack pointeur (ou index) qui pointe toujours sur la première
     // case LIBRE.
     // On empile en incrémentant sp !
     // On dépile en décrémentant sp !


extern int bp;
     // Block pointeur : un pointeur (ou index) qui marque une position fixe,
     // sur la pile, à partir de laquelle on peut retrouver les données
     // (paramètres, variables locales) propres au bloc courant.

/*

extern int fp;
     // Frame pointer. Peut être utilisé à divers usage :-)
     // Simplifie la gestion des appels / retours de fonctions
     // Mais c'est non necessaire.
     
*/

/*************************************************/
// Instructions P-CODE, définit comme des macros C

/********** I. Empiler / depiler une valeur ***************/

// empiler
#define LOADI(X)  stack[sp ++].int_value = X;

#define LOADF(F)  stack[sp ++].float_value = F;

// depiler n valeurs du sommet de pile
#define DROP(n)   sp = sp-n;

/********** II. Opérations arithmetiques binaires *********/

/* OPERATION UNAIRE */

#define MINUSI stack[sp-1].int_value = - stack[sp-1].int_value;
#define MINUSF stack[sp-1].float_value = - stack[sp-1].float_value;


/* CONVERSION */
#define I2F1 stack[sp-1].float_value = (float) stack[sp-1].int_value;
                   /* Conversion sommet de pile */
#define I2F2 stack[sp-2].float_value = (float) stack[sp-2].int_value;
                   /* Conversion sous sommet de pile */


/* OPERATIONS BINAIRES */

/* On dépile, on effectue le calcul
   on rempile le résultat.

   On peut ajouter des fonctions binaires de PCode si besoin.
   Ex : ==
*/

  /* on integer */
#define ADDI stack[sp - 2].int_value = stack[sp - 2].int_value + stack[sp - 1].int_value; sp--;
#define MULTI stack[sp - 2].int_value = stack[sp - 2].int_value * stack[sp - 1].int_value; sp--;
#define SUBI stack[sp - 2].int_value = stack[sp - 2].int_value - stack[sp - 1].int_value; sp--;
#define DIVI stack[sp - 2].int_value = stack[sp - 2].int_value / stack[sp - 1].int_value; sp--;

  /* on float */
#define ADDF stack[sp - 2].float_value = stack[sp - 2].float_value + stack[sp - 1].float_value; sp--;
#define MULTF stack[sp - 2].float_value = stack[sp - 2].float_value * stack[sp - 1].float_value; sp--;
#define SUBF stack[sp - 2].float_value = stack[sp - 2].float_value - stack[sp - 1].float_value; sp--;
#define DIVF stack[sp - 2].float_value = stack[sp - 2].float_value / stack[sp - 1].float_value; sp--;

/********** III. Opérations comparaison binaires *********/

  /* on integer */
#define LTI stack[sp - 2].int_value = stack[sp - 2].int_value < stack[sp - 1].int_value; sp--;
#define GTI stack[sp - 2].int_value = stack[sp - 2].int_value > stack[sp - 1].int_value; sp--;
//#define LEQI stack[sp - 2].int_value = stack[sp - 2].int_value <= stack[sp - 1].int_value; sp--;
//#define GEQI stack[sp - 2].int_value = stack[sp - 2].int_value >= stack[sp - 1].int_value; sp--;
#define EQI stack[sp - 2].int_value = stack[sp - 2].int_value == stack[sp - 1].int_value; sp--;
#define NEQI stack[sp - 2].int_value = stack[sp - 2].int_value != stack[sp - 1].int_value; sp--;
 
  /* on float */
#define LTF stack[sp - 2].int_value = (int) stack[sp - 2].float_value < stack[sp - 1].float_value; sp--;
#define GTF stack[sp - 2].int_value = (int) stack[sp - 2].float_value > stack[sp - 1].float_value; sp--;
//#define LEQF stack[sp - 2].int_value = (int) stack[sp - 2].float_value <= stack[sp - 1].float_value; sp--;
//#define GEQF stack[sp - 2].int_value = (int) stack[sp - 2].float_value >= stack[sp - 1].float_value; sp--;
#define EQF stack[sp - 2].int_value = (int) stack[sp - 2].float_value == stack[sp - 1].float_value; sp--;
#define NEQF stack[sp - 2].int_value = (int) stack[sp - 2].float_value != stack[sp - 1].float_value; sp--;



/********** IV. Branchements ******************/
// La condition est en sommet de pile.
// Elle est toujours dépilée.

      // branchement conditionel
#define IFT(L) if (stack[--sp].int_value) goto L;

      // branchement conditionel avec negation
#define IFN(L) if (!(stack[--sp].int_value)) goto L;

      // branchement inconditionel
#define GOTO(L) goto L;

/**********  V. NOP ************************************/

#define NOP 


/********** V. Opérations booléennes *********/
/* codé par évaluation paresseuse ? */

/*
  // boolean coded as integer
#define OR stack[sp - 2].int_value = stack[sp - 2].int_value || stack[sp - 1].int_value; sp--;
#define AND stack[sp - 2].int_value = stack[sp - 2].int_value && stack[sp - 1].int_value; sp--;
#define NOT stack[sp - 1].int_value = ! stack[sp - 1].int_value;

*/

/********** VI. Arithmétique des pointeurs *********/

#define LOADBP stack[sp++].index_value = bp;

#define SHIFT (N) stack[sp].index_value = stack[sp].index_value + N;
/* L'offset N popurra aussi être négatif */

#define LOADP stack[sp-1] = stack[stack[sp-1].index_value];
    /* Remplace l'index de sommet de pile par la valeur stocké à cet index */

#define STOREP stack[stack[sp-2].index_value] = stack[sp-1]; sp=sp-2;
    /* Stocke à l'index sous le sommet de pile la valeur de sommet de pile,
       et dépile d'autant */

/* REMARQUE : dans ces deux operation, on copie des valeurs de type union.
   La vérification de type devra être faites à la compilation !!! */


/********** VII. Gestion des blocs et des variables locales ***/

/*  Etats de la pile souhaité pendant l'execution d'un gestion d'un bloc
    avec m variables locales  Varloc1, ....,  Varlocm

       ---------
bp+m  | VarLocm | 
...   | ....... |
...   | ....... |
bp+1  | Varloc1 |
bp    | OldBp   |  // to be restored upon exiting the block with oldSP = bp
       ---------

Autrement dit, l'adresse dans le code cible des variables locales
au block est donné, dans l'ordre, par bp+1, bp+2, ..., bp+n
Le i de la variable Varloci est appelé l'offset de cette variable.
Il devra être stocké, à la compilation, dans la table des symboles.

*/


/* ***** Entrée dans un bloc ***** 
   Sauvegarde de bp en sommet de pile et positione bp à cet endroit. 
*/
#define SAVEBP stack[sp].index_value = bp; bp = sp++;
// Remarque : il faudra après ça reserver les emplacements dans la pile
// ce qui se fera dans le traitement des déclarations locales à ce block.

/* ***** Sortir d'un bloc ***** */
#define RESTOREBP stack[sp].index_value = bp;\
                  bp = stack[bp].index_value;\
                  stack[stack[sp].index_value]=stack[sp-1];\
		  sp = stack[sp].index_value+1;


/* Ce qui fait, dans l'ordre:
   1) sauvergarde de bp,
   2) restauration de (l'ancien) bp
   3) copie de la valeur de sommet de pile (valeur de retour du block) à l'index bp sauvergardé
   4) dépilement de toutes les variables locales (sp pointe au dessus de la valeur de retour)
   
   Autrement dit, l'exécution du code cible (hors effet de bord due aux affectations de variables 
   externe aux block) reviendra in fine à empiler la "valeur du bloc". 
*/

/* REMARQUE : recopie d'une valeur de type union.
   La vérification de type devra être faites à la compilation !!! */

/********* VII. Gestion des appels de fonctions ***************/

/* 
  Remarque:
  lors d'un appel type toto(e1,...,en) la compilation des n arguments, dans l'ordre,
  produit un code cible qui, à l'execution, revient à empiler les valeurs 
  de e1, e2, ..., en.

  On appel alors sur le bloc du corp de la fonction toto compilée 
  en une fonction void pcode_toto()

*/

#define CALL(L)  L(); // L est le symbole pcode_toto dans notre exemple

/* au début de l'execution du bloc de la fonction la pile est donc 
   dans l'état

bp    |         | (positionnée par SAVEBP au début bloc de la fonction)
       ---------
bp-1  |  expn   | 
...   | ....... |
...   | ....... |
bp-n  |  exp1   |
       ---------
      |  Pile0  |

On constate que les arguements de la fonction sont donc, dans l'ordre, aux index
bp-n pour exp1, bp-(n-1) pour exp2, ..., bp-1 pour expn.

A la fin de l'execution du bloc, on a 
sp      |         |
         ---------
sp-1    | ret_val | (positionnée par RESTOREBP à la fin de l'execution du bloc de la fonction)
         ---------
sp-2    |  expn   | 
...     | ....... |
...     | ....... |
sp-n-1  |  exp1   |
         ---------
        |  Pile0  |
  
*/

#define ENDCALL(n)  stack[sp-n-1] = stack[sp-1];\
                    sp = sp-n;

/* 

Ce qui fait, dans l'ordre:
    1) recopie de la valeur de retour de la fonction à la place de son premier arguement
    2) positionnement de sp juste au dessus de cette valeur.
Remarque, on vérifie que si n=0 on ne change finalement rien sur la pile.

sp      |         |
         ---------
sp-1    | ret_val | (positionnée par RESTOREBP à la fin de l'execution du bloc de la fonction)
         ---------
        |  Pile0  |

*/

/*

Bref, l'appel de (la compilation de) l'appel de fonction toto(e1,...,en) a eut, in fine, 
d'empiler s valeur de retour CQFD.

Pour résumé, dans le corps d'une fonction, 
1) la valeur de la ième variable locale est stockée dans 
           stack[bp+i]
2) la valeur de la ième variable locale du bloc immédiatement englobant est stockée dans
           stack[stack[bp].index_value+i]
3) la valeur de la ième variable locale du bloc immédiatement englobant le bloc
immédiatement englobant est stockée dans
           stack[stack[stack[bp].index_value].index_value+i]
etc...

Et on peut faire un calcul analogue pour les paramètres des fonctions appelé récursivement les unes les autres:
1) la valeur du ième paramètre de la fonction courante:
           stack[bp-(n0+1-i)] avec n0 le nb de paramètre de la fonction courante
2) la valeur du ième paramètre de la fonction appelant la fonction courante:
           stack[stack[bp].index_value-(n1+1-i)] avec n1 le nb de paramètres de cette 
                              fonction appelant la fonction courante etc...

profondeur d'appel calculable à la compilation est "lisible" (comment ?) dans la table des symboles... 

*/

/***************************************************/
/* Autres fonctions de "PCODE" pour le debug.
   Vous pouvez ajouter les fonctions que vous voulez !
 */

int empty_stack ();

int full_stack ();

int valid_stack ();

void print_stack();
// Attention, n'imprime que les élements de pile vue comme des entiers !!!


void print_top_stack_int();
// Imprime le sommet de pile comme un entier

void print_top_stack_float();
// Imprime le sommet de pile comme un flottant

void print_top_stack_index();
// Imprime le sommet de pile comme un index (de pile)

#endif

