#include "mamecc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LocalVarList *init_varlist(void) {
  LocalVarList *varlist = malloc(sizeof(LocalVarList));
  varlist->ident = "";
  varlist->offset = 0;
  varlist->next = NULL;
  return varlist;
}

LocalVarList *new_var_ident(LocalVarList *vl, char *ident) {
  LocalVarList *tail = tail_of_varlist(vl);
  LocalVarList *newvarlist = malloc(sizeof(LocalVarList));
  newvarlist->ident = ident;
  newvarlist->offset = tail->offset + 8;
  newvarlist->next = NULL;
  tail->next = newvarlist;
  return newvarlist;
}

LocalVarList *is_registered(LocalVarList *vl, char *ident) {
  while (vl != NULL) {
    if (strncmp(vl->ident, ident, strlen(ident)) == 0) return vl;
    vl = vl->next;
  }
  return NULL;
}

size_t get_offset(LocalVarList *vl, char *ident) {
  while (vl != NULL) {
    if (strncmp(vl->ident, ident, strlen(ident)) == 0) return vl->offset;
    vl = vl->next;
  }

  fprintf(stderr, "Error: Local variable '%s' is not registered\n", ident);
  exit(1);
}

LocalVarList *tail_of_varlist(LocalVarList *vl) {
  LocalVarList *cur = vl;
  while (cur->next != NULL) cur = cur->next;
  return cur;
}

size_t get_num_vars(LocalVarList *vl) {
  size_t count = 0;
  while (vl->next != NULL) {
    ++count;
    vl = vl->next;
  }
  return count;
}