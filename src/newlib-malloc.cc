extern "C" {

#include <stdlib.h>

struct _reent;

void *_malloc_r(struct _reent *, size_t size) {
  return malloc(size);
}

void _free_r(struct _reent *, void *ptr) {
  free(ptr);
}

void *_realloc_r(struct _reent *, void *ptr, size_t size) {
  return realloc(ptr, size);
}

void *_calloc_r(struct _reent *, size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

}