#ifndef DS_SET_H
#define DS_SET_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status codes ------------------------------------------------------- */
#define SET_OK             0
#define SET_ERR_OOM       -2
#define SET_ERR_NOTFOUND  -4
#define SET_ERR_DUPLICATE -7

/* Opaque handle */
typedef struct set set;

/* ---- Lifecycle ---------------------------------------------------------- */
set   *set_new(void);
void   set_dispose(set *self);

/* ---- Queries ------------------------------------------------------------ */
size_t set_size(const set *self);
bool   set_contains(const set *self, const char *key);

/* ---- Mutations ---------------------------------------------------------- */
int    set_insert(set *self, const char *key);
int    set_remove(set *self, const char *key);

/* ---- Debug -------------------------------------------------------------- */
void   set_debug_print(const set *self);

#ifdef __cplusplus
}
#endif
#endif /* DS_SET_H */
