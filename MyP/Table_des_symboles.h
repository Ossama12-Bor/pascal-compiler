/*
 *  Table des symboles.h
 *
 *  Associative array encoded as linked list of pair (symbol_name, symbol_value)
 *
 *  To be used only with getter get_symbol_value and setter set_symbol_value.
 *
 *  Type attribute can be customized.
 *
 *  Symbol names must be valid sid from Table des chaines. 
 *
 */

#ifndef TABLE_DES_SYMBOLES_H
#define TABLE_DES_SYMBOLES_H

#include "Table_des_chaines.h"





typedef struct linked {
	int type;
	struct linked * next;
}linked;

typedef struct args {
  linked * head;
  linked * tail;
}args;

/* Déclarations des types d'attributs */

char* type2string (int);
struct ATTRIBUTE {
  int type;
  int offset;
  int depth;
  args *arguments;
  
  /* les autres attributs dont vous pourriez avoir besoin 
     pour les symboles seront déclarés ici */
  
};
   
typedef struct ATTRIBUTE * attribute;


attribute makeSymbol(int type, int offset, int depth);

attribute new_attribute ();
/* returns the pointeur to a newly allocated (but uninitialized) attribute value structure */


/* get the symbol value of symb_id from the symbol table, NULL if it fails */
attribute get_symbol_value(sid symb_id);

/* add the symbol symb_id with given value */
attribute set_symbol_value(sid symb_id,attribute value);

/* removes all symbols that have "depth" depth*/
void remove_local_variables(int depth);

void free_arguments(args * arguments);

void add_argument(args * arguments ,int Type);

#endif
