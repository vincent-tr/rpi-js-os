#ifndef SUPPORT_NEWLIB_MALLOC_DEF_H
#define SUPPORT_NEWLIB_MALLOC_DEF_H

namespace support {

  extern void *(*malloc_impl)(size_t size);
  extern void (*free_impl)(void *ptr);
  extern void *(*realloc_impl)(void *ptr, size_t size);
  extern void *(*calloc_impl)(size_t nmemb, size_t size);
}

#endif // SUPPORT_NEWLIB_MALLOC_DEF_H

