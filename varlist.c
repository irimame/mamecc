#include "mamecc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Varlist *init_varlist() {
  Varlist *varlist = malloc(sizeof(Varlist));
  varlist->ident = "";
  varlist->offset = 0;
  varlist->next = NULL;
  return varlist;
}

Varlist *new_var_ident(Varlist *vl, char *ident) {
  Varlist *tail = tail_of_varlist(vl);
  Varlist *newvarlist = malloc(sizeof(Varlist));
  newvarlist->ident = ident;
  newvarlist->offset = tail->offset + 8;
  newvarlist->next = NULL;
  tail->next = newvarlist;
  return newvarlist;
}

Varlist *is_registered(Varlist *vl, char *ident) {
  while (vl != NULL) {
    if (strncmp(vl->ident, ident, strlen(ident)) == 0) return vl;
    vl = vl->next;
  }
  return NULL;
}

int get_offset(Varlist *vl, char *ident) {
  while (vl != NULL) {
    if (strncmp(vl->ident, ident, strlen(ident)) == 0) return vl->offset;
    vl = vl->next;
  }

  fprintf(stderr, "Error: Variable '%s' is not registered\n", ident);
  exit(1);
}

Varlist *tail_of_varlist(Varlist *vl) {
  Varlist *cur = vl;
  while (cur->next != NULL) cur = cur->next;
  return cur;
}

