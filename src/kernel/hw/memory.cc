#include <stddef.h>
#include <stdint.h>

#include "memory.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      class page_info : public utils::list<page_info>::node {
        void *_address;
        bool _used;

      public:
        explicit inline page_info(void *paddress)
         : _address(paddress) {
        }

        inline void *address() const {
          return _address;
        }

        inline bool used() const {
          return _used;
        }

        void alloc();
        void free();
        static inline page_info *find(const void *address);
      };

      extern "C" page_info __end;

      page_info *pages = &__end;
      static utils::list<page_info> used_pages;
      static utils::list<page_info> free_pages;


      inline void page_info::alloc() {
        ASSERT(!_used);
        free_pages.remove(this);
        used_pages.add(this);
      }

      inline void page_info::free() {
        ASSERT(_used);
        used_pages.remove(this);
        free_pages.add(this);
      }

      inline page_info *page_info::find(const void *address) {
        // TODO
      }

      page page::alloc() {
        ASSERT(!free_pages.empty());
        page_info *pi = free_pages.head();
        pi->alloc();
        return page(pi->address());
      }

      void page::free() {
        page_info *pi = page_info::find(address);
        pi->free();
      }

      void init(const uint32_t &page_size, const uint32_t &ram_size) {
        // TODO
      }
    }
  }
}