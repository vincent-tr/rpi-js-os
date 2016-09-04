#include <stddef.h>
#include <stdint.h>

#include "phys-page.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      class phys_page_info : public utils::list<phys_page_info>::node {
        uint32_t _address;
        bool _used;

      public:
        explicit inline phys_page_info(const uint32_t &paddress)
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
        static inline phys_page_info *find(const uint32_t &address);
      };

      extern "C" phys_page_info __end;

      static phys_page_info *pages = &__end;
      static utils::list<phys_page_info> used_pages;
      static utils::list<phys_page_info> free_pages;


      inline void phys_page_info::alloc() {
        ASSERT(!_used);
        free_pages.remove(this);
        used_pages.add(this);
        _used = true;
      }

      inline void phys_page_info::free() {
        ASSERT(_used);
        used_pages.remove(this);
        free_pages.add(this);
        _used = false;
      }

      inline phys_page_info *phys_page_info::find(const uint32_t &address) {
        const uint32_t &page_size = kernel::platform::get().page_size();
        return pages + (address / page_size);
      }

      phys_page phys_page::alloc() {
        ASSERT(!free_pages.empty());
        phys_page_info *pi = free_pages.head();
        pi->alloc();
        return phys_page(pi->address());
      }

      void phys_page::free() {
        phys_page_info *pi = phys_page_info::find(address);
        pi->free();
      }

      void phys_page::init(uint32_t &pages_begin, uint32_t &page_end) {
        const uint32_t &page_size = kernel::platform::get().page_size();
        const uint32_t &ram_size = kernel::platform::get().ram_size();
        //ASSERT(page_size == 4096);

        phys_page_info *current=pages;
        for(uint32_t addr = 0; addr<ram_size; ++current, addr += page_size) {
          new (current) phys_page_info(addr);
          free_pages.add(current);
        }

        pages_begin = reinterpret_cast<uint32_t>(pages);
        page_end = reinterpret_cast<uint32_t>(current);

        while((uint32_t)phys_page::alloc() + page_size < page_end);
      }
    }
  }
}