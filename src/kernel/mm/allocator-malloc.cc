#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "allocator.hh"

extern "C" {

  void *malloc(size_t size) {
    if(!size) { size = 1; }
    return kernel::mm::allocator::allocate(size);
  }

  void free(void *ptr) {
    if(!ptr) { return; }
    kernel::mm::allocator::deallocate(ptr);
  }

  void *realloc(void *ptr, size_t size) {
    if(!ptr) {
      return malloc(size);
    }
    if(!size) {
      free(ptr);
      return nullptr;
    }
    size_t ptr_size = kernel::mm::allocator::get_size(ptr);
    if(ptr_size >= size) {
      return ptr;
    }
    void *new_ptr = malloc(size);
    if(!new_ptr) {
      return nullptr;
    }

    memcpy(new_ptr, ptr, ptr_size);
    free(ptr);
    return new_ptr;
  }

  void *calloc(size_t nmemb, size_t size) {
    size *= nmemb;
    void *ptr = malloc(size);
    if(!ptr) { return ptr; }
    memset(ptr, 0, size);
    return ptr;
  }
}