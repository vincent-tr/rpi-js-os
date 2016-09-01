#include <stddef.h>
#include <stdint.h>

#include "memory.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      class page_info : public utils::list<page_info>::node {
        uint32_t _address;
        bool _used;

      public:
        explicit inline page_info(const uint32_t &paddress)
         : _address(paddress) {
        }

        inline const uint32_t &address() const {
          return _address;
        }

        inline const bool &used() const {
          return _used;
        }

        void alloc();
        void free();
        static inline page_info *find(const uint32_t &address);
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

      inline page_info *page_info::find(const uint32_t &address) {
        const uint32_t &page_size = kernel::platform::get().page_size();
        return pages + (address / page_size);
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

      void init(uint32_t &pages_begin, uint32_t &page_end) {
        const uint32_t &page_size = kernel::platform::get().page_size();
        const uint32_t &ram_size = kernel::platform::get().ram_size();
        //ASSERT(page_size == 4096);

        page_info *current=pages;
        for(uint32_t addr = 0; addr<ram_size; ++current, addr += page_size) {
          new (current) page_info(addr);
          free_pages.add(current);
        }

        pages_begin = reinterpret_cast<uint32_t>(pages);
        page_end = reinterpret_cast<uint32_t>(current);

        while((uint32_t)page::alloc() + page_size < page_end);
      }
    }
  }
}