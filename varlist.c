#include "mamecc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LocalIdentList *init_varlist(void) {
  LocalIdentList *varlist = malloc(sizeof(LocalIdentList));
  varlist->ident = "";
  varlist->offset = 0;
  varlist->kind = LCL_NULL;
  varlist->next = NULL;
  return varlist;
}

LocalIdentList *new_var_ident(LocalIdentList **vl, char *ident, LocalIdentKind kind) {
  LocalIdentList *newvarlist = malloc(sizeof(LocalIdentList));
  newvarlist->ident = ident;
  newvarlist->offset = (*vl)->offset + 8;
  newvarlist->kind = kind;
  newvarlist->next = (*vl);
  (*vl) = newvarlist;
  return newvarlist;
}

LocalIdentList *is_registered(LocalIdentList *vl, char *ident) {
  while (vl) {
    if (strncmp(vl->ident, ident, strlen(ident)) == 0) return vl;
    vl = vl->next;
  }
  return NULL;
}

size_t get_offset(LocalIdentList *vl, char *ident) {
  while (vl) {
    if (strncmp(vl->ident, ident, strlen(ident)) == 0) return vl->offset;
    vl = vl->next;
  }

  fprintf(stderr, "Error: Local variable '%s' is not registered\n", ident);
  exit(1);
}

size_t get_num_idents(LocalIdentList *vl, LocalIdentKind kind) {
  size_t count = 0;
  while (vl) {
    if (vl->kind == kind) ++count;
    vl = vl->next;
  }
  return count;
}