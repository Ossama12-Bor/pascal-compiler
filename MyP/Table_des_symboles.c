/*
 *  Table des symboles.c
 *
 *  Created by Janin on 12/10/10.
 *  Copyright 2010 LaBRI. All rights reserved.
 *
 */


#include <stdlib.h>
#include <stdio.h>

#include "Table_des_symboles.h"

/* bit  type handling */

  


/* Attribute types */

attribute new_attribute () {
  attribute r;
  r  = malloc (sizeof (struct ATTRIBUTE));
  if (r==NULL) {printf("Failed Malloc\n"); exit(-1);}
  return r;
};

attribute makeSymbol(int type, int offset, int depth)
{
  attribute r = new_attribute();
  r -> type = type;
  r -> offset = offset;
  r -> depth = depth;
  return r;
}



/* The storage structure is implemented as a linked chain */

/* linked element def */

typedef struct elem {
	sid symbol_name;
	attribute symbol_value;
	struct elem * next;
} elem;

/* linked chain initial element */
static elem * storage=NULL;

/* get the symbol value of symb_id from the symbol table */
attribute get_symbol_value(sid symb_id) {
	elem * tracker=storage;

	/* look into the linked list for the symbol value */
	while (tracker) {
		if (tracker -> symbol_name == symb_id) return tracker -> symbol_value; 
		tracker = tracker -> next;
	}
    
	/* if not found does cause an error */
	fprintf(stderr,"Error : symbol %s is not a valid defined symbol\n",(char *) symb_id);
	exit(-1);
};

/* add the symbol symb_id with given value */
attribute set_symbol_value(sid symb_id,attribute value) {

	elem * tracker;	
	tracker = malloc(sizeof(elem));
	tracker -> symbol_name = symb_id;
	tracker -> symbol_value = value;
	tracker -> next = storage;
	storage = tracker;
	return storage -> symbol_value;
}

