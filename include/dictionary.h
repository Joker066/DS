#ifndef DS_DICTIONARY_H
#define DS_DICTIONARY_H

/* Public interface for a string->string hash dictionary (open addressing).
   All operations return explicit status codes; no diagnostic printing is performed. */

#include <stddef.h>   /* size_t */
#include <stdbool.h>  /* bool */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define DICT_OK            0
#define DICT_ERR_OOM      -2
#define DICT_ERR_NOTFOUND -4

/* Opaque handle */
typedef struct dictionary dict;

/* ---- Lifecycle ---------------------------------------------------------- */
dict   *new_dict(void);                 /* Returns NULL on allocation failure */
void    dict_dispose(dict *self);

/* ---- Queries ------------------------------------------------------------ */
size_t  dict_size(const dict *self);    /* number of stored key/value pairs */
bool    dict_contains(const dict *self, const char *key);

/* ---- Lookups ------------------------------------------------------------ */
/* On success, writes a pointer to the stored value into *out_value and returns DICT_OK.
   The returned pointer remains owned by the dictionary; do not free it. */
int     dict_get(const dict *self, const char *key, const char **out_value);

/* ---- Mutations ---------------------------------------------------------- */
/* Insert or replace (upsert). On success returns DICT_OK. */
int     dict_insert(dict *self, const char *key, const char *value);

/* Deletes key if present. Returns DICT_OK on success, DICT_ERR_NOTFOUND otherwise. */
int     dict_del(dict *self, const char *key);

/* ---- Debug -------------------------------------------------------------- */
/* Optional helper to print basic stats or dump contents (implementation-defined). */
void    dict_debug_print(const dict *self);

#ifdef __cplusplus
}
#endif
#endif /* DS_DICTIONARY_H */
