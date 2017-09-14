extern "C" {
  #include <stdlib.h>
}

#include "malloc_def.hh"

namespace support {
  void *(*malloc_impl)(size_t size);
  void (*free_impl)(void *ptr);
  void *(*realloc_impl)(void *ptr, size_t size);
  void *(*calloc_impl)(size_t nmemb, size_t size);
}

extern "C" {
  struct _reent;

  void *_malloc_r(struct _reent *, size_t size) {
    return support::malloc_impl(size);
  }

  void *malloc(size_t size) {
    return support::malloc_impl(size);
  }

  void _free_r(struct _reent *, void *ptr) {
    support::free_impl(ptr);
  }

  void free(void *ptr) {
    support::free_impl(ptr);
  }

  void *_realloc_r(struct _reent *, void *ptr, size_t size) {
    return support::realloc_impl(ptr, size);
  }

  void *realloc(void *ptr, size_t size) {
    return support::realloc_impl(ptr, size);
  }

  void *_calloc_r(struct _reent *, size_t nmemb, size_t size) {
    return support::calloc_impl(nmemb, size);
  }

  void *calloc(size_t nmemb, size_t size) {
    return support::calloc_impl(nmemb, size);
  }
}