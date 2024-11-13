/*
 *  PCode.c
 * 
 *  Created by Janin on 12/11/21.
 *  Copyright 2021 ENSEIRB. All rights reserved.
 *
 */


#include "PCode.h"
#include <stdio.h>

stack_value stack[SIZE];

int sp = 0; // première case libre
int bp = 0;
int fp = 0;


// test pile vide, pile pleine et pile valide (pour le debug ?)

int empty_stack () {
  return sp <= 0;  // sp == 0 when valid stack
}


int full_stack () {
  return sp >= SIZE;  // sp == SIZE when valid stack
}

int valid_stack (){
  return sp > 1 && sp <= SIZE;
}


// imprime la pile

void print_stack() {
  printf("\nBEGIN STACK : bp = %i, fp = %i\n",
	 bp, fp);
  int p = 0;
  while (p < sp) {
    printf ("(%i)  %i\n",p, stack[p].int_value);
    p++;
  }
  printf("END STACK\n");
  
}

void print_top_stack_int()
{printf("Stack[%i] = %i (INT)\n",sp-1,stack[sp-1].int_value);}

void print_top_stack_float()
{printf("Stack[%i] = %f (FLOAT)\n",sp-1,stack[sp-1].float_value);}

void print_top_stack_index()
{printf("Stack[%i] = %i (INDEX)\n",sp-1,stack[sp-1].index_value);}


void print_top_stack_float();
// Imprime le sommet de pile comme un flottant


void test();

void test() {
  // testing macros' syntax (not to be ran)
  
  LOADI(22)
  LOADF(4.5)
  DROP(4)
  MINUSI
  MINUSF  
  I2F1
    
  ADDI
  MULTI
  SUBI
  DIVI
    
  ADDF
  MULTF
  SUBF
  DIVF
    
  LTI
  GTI
    //  LEQI
    //  GEQI
    
  EQI
  LTF
  GTF
    // LEQF
    // GEQF
  EQF
    
  IFT(toto)
  IFN(toto)
  GOTO(toto)

 toto: NOP 
  LOADP
  STOREP
  SAVEBP
    
  RESTOREBP
  
  CALL(test)
    
  ENDCALL(3)
  
}


