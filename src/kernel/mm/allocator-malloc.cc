#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "allocator.hh"
#include "support/newlib/malloc_def.hh"

namespace kernel {
  namespace mm {

    static void *s_malloc(size_t size) {
      if(!size) { size = 1; }
      return kernel::mm::allocator::allocate(size);
    }

    static void s_free(void *ptr) {
      if(!ptr) { return; }
      kernel::mm::allocator::deallocate(ptr);
    }

    static void *s_realloc(void *ptr, size_t size) {
      if(!ptr) {
        return s_malloc(size);
      }
      if(!size) {
        s_free(ptr);
        return nullptr;
      }
      size_t ptr_size = kernel::mm::allocator::get_size(ptr);
      if(ptr_size >= size) {
        return ptr;
      }
      void *new_ptr = s_malloc(size);
      if(!new_ptr) {
        return nullptr;
      }

      memcpy(new_ptr, ptr, ptr_size);
      s_free(ptr);
      return new_ptr;
    }

    static void *s_calloc(size_t nmemb, size_t size) {
      size *= nmemb;
      void *ptr = s_malloc(size);
      if(!ptr) { return ptr; }
      memset(ptr, 0, size);
      return ptr;
    }

    // connect to support/malloc_def
    struct connecter {
      connecter() {
        support::malloc_impl = s_malloc;
        support::free_impl = s_free;
        support::realloc_impl = s_realloc;
        support::calloc_impl = s_calloc;
      }
    };

    static connecter instance;

  }
}