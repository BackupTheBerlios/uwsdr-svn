/* thunk.c */

#ifndef _thunk_h
#define _thunk_h

typedef int (*Thunk)(int n, char **p);

typedef struct _cmd_tbl_entry {
  char *key;
  Thunk thk;
} CTE;

extern Thunk Thunk_lookup(const CTE* ctb, const char *key);

#endif
