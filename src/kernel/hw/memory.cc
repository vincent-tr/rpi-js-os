#include <stddef.h>
#include <stdint.h>

#include "memory.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      class page_info : public utils::list<page_info>::node {

      };

      static utils::list<page_info> used_pages;
      static utils::list<page_info> free_pages;

      page page::alloc() {
        // TODO
      }

      void page::free() {
        // TODO
      }

      void init(const uint32_t &page_size, const uint32_t &ram_size) {
        // TODO
      }
    }
  }
}