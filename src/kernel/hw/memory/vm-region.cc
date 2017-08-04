#include <stddef.h>
#include <stdint.h>

#include "phys-page.hh"
#include "vm-protection.hh"
#include "vm-page.hh"
#include "vm-region.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      extern "C" uint32_t __text_start;
      extern "C" uint32_t __text_end;
      extern "C" uint32_t __rodata_start;
      extern "C" uint32_t __rodata_end;
      extern "C" uint32_t __data_start;
      extern "C" uint32_t __data_end;
      extern "C" uint32_t __bss_start;
      extern "C" uint32_t __bss_end;

      struct region_info : public utils::list<region_info>::node, vm_region {
        explicit inline region_info(const uint32_t &vaddr, const uint32_t &plen, const vm_protection &pprot, const char *pname)
         : vm_region(vaddr, plen, pprot, pname) {
        }
      };

      static utils::list<region_info> regions;

      // 4x kernel mapping
      // phys mem desc array
      // TODO: add cells for is_internal
      static constexpr uint32_t builtin_regions_size = 5;
      static char builtin_region_buffer[ builtin_regions_size * sizeof(region_info) ];
      static region_info *builtin_regions = reinterpret_cast<region_info *>(builtin_region_buffer);
      static uint32_t internal_count = 0;

      static region_info *new_internal(const uint32_t &address, const uint32_t &len, const vm_protection &prot, const char *name);
      static void map_region(region_info *ri);
      static void create_internal_region(const uint32_t &begin, const uint32_t &end, const bool &can_write, const char *name);

      void vm_region::init() {
        const auto &platform = kernel::platform::get();
        create_internal_region(__text_start,                 __text_end,                 false, "kernel:text");
        create_internal_region(__rodata_start,               __rodata_end,               false, "kernel:rodata");
        create_internal_region(__data_start,                 __data_end,                 true,  "kernel:data");
        create_internal_region(__bss_start,                  __bss_end,                  true,  "kernel:bss");
        create_internal_region(platform.hw_mem_desc_begin(), platform.hw_mem_desc_end(), true,  "kernel:hw_mem_desc");
      }

      void create_internal_region(const uint32_t &begin, const uint32_t &end, const bool &can_write, const char *name) {
        region_info *ri = new_internal(begin, end - begin, can_write ? vm_protection{1, 1} : vm_protection{1, 0}, name);
        regions.add(ri);
      }

      vm_region *vm_region::find(const uint32_t &address) {
        for(region_info* region : regions) {
          if(region->address() <= address && address < region->address_end()) {
            return region;
          }
        }

        return nullptr;
      }

      vm_region *vm_region::create(const uint32_t &len, const vm_protection &prot, const char *name, const bool &is_internal) {
        const uint32_t &page_size = kernel::platform::get().page_size();

        uint32_t address = page_size; // forbid allocation at address 0

        // TODO: find suitable place

/*
TODO: permit allocation
        region_info *ri = is_internal ?
          new_internal(address, len, prot, name) :
          new region_info(address, len, prot, name);
*/
        ASSERT(is_internal);
        region_info *ri = new_internal(address, len, prot, name);

        map_region(ri);

        regions.add(ri);

        return ri;
      }

      region_info *new_internal(const uint32_t &address, const uint32_t &len, const vm_protection &prot, const char *name) {
        ASSERT(internal_count < builtin_regions_size);
        region_info *ri = builtin_regions + (internal_count++);
        return new (ri) region_info(address, len, prot, name);
      }

      void map_region(region_info *ri) {
        const uint32_t &page_size = kernel::platform::get().page_size();
        for(uint32_t addr=ri->address(); addr < ri->address_end(); addr += page_size) {
          vm_page page(addr);
          page.map(ri->protection(), phys_page::alloc());
        }
      }

      void vm_region::release(vm_region *region) {
        region_info *ri = static_cast<region_info *>(region);

        const uint32_t &page_size = kernel::platform::get().page_size();
        for(uint32_t addr=ri->address(); addr < ri->address_end(); addr += page_size) {
          vm_page page(addr);
          page.phys().free();
          page.unmap();
        }

        regions.remove(ri);

/*
TODO: permit allocation
        delete ri;
*/
      }
    }
  }
}
