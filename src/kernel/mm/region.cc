#include <stddef.h>
#include <stdint.h>

#include "kernel/hw/memory/phys-page.hh"
#include "protection.hh"
#include "kernel/hw/memory/vm-page.hh"
#include "region.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace mm {

    extern "C" uint32_t __text_start;
    extern "C" uint32_t __text_end;
    extern "C" uint32_t __rodata_start;
    extern "C" uint32_t __rodata_end;
    extern "C" uint32_t __data_start;
    extern "C" uint32_t __data_end;
    extern "C" uint32_t __bss_start;
    extern "C" uint32_t __bss_end;

    static constexpr uint32_t devices_start =
#ifdef QEMU
      0x10000000
#else
      0x20000000
#endif
    ;

    static constexpr uint32_t devices_end =
#ifdef QEMU
      0x20000000
#else
      0x30000000
#endif
    ;

    struct region_info : public utils::list<region_info>::node, region {
      explicit inline region_info(const uint32_t &vaddr, const uint32_t &plen, const protection &pprot, const char *pname)
       : region(vaddr, plen, pprot, pname) {
      }

      void map() {
        const uint32_t &page_size = kernel::platform::get().page_size();
        for(uint32_t addr=address(); addr < address_end(); addr += page_size) {
          hw::memory::vm_page page(addr);
          page.map(protection(), hw::memory::phys_page::alloc());
        }
      }

      void map_identity() {
        const uint32_t &page_size = kernel::platform::get().page_size();
        for(uint32_t addr=address(); addr < address_end(); addr += page_size) {
          hw::memory::vm_page page(addr);
          page.map(protection(), hw::memory::phys_page(addr));
        }
      }

      void unmap() {
        const uint32_t &page_size = kernel::platform::get().page_size();
        for(uint32_t addr=address(); addr < address_end(); addr += page_size) {
          hw::memory::vm_page page(addr);
          page.phys().free();
          page.unmap();
        }
      }
    };

    static utils::list<region_info> regions;

    // reserved
    // 4x kernel mapping
    // phys mem desc array
    // uart0 mapping
    // TODO: add cells for is_internal
    // TODO: stack
    static constexpr uint32_t builtin_regions_size = 7;
    static char builtin_region_buffer[ builtin_regions_size * sizeof(region_info) ];
    static region_info *builtin_regions = reinterpret_cast<region_info *>(builtin_region_buffer);
    static uint32_t internal_count = 0;

    static region_info *new_internal(const uint32_t &address, const uint32_t &len, const protection &prot, const char *name);
    static void create_internal_region(const uint32_t &begin, const uint32_t &end, const bool &can_write, const char *name);
    static void create_internal_region(const void *begin, const void *end, const bool &can_write, const char *name);
    static void insert_region(region_info *ri, region_info* prev = nullptr);

    void region::init() {
      const auto &platform = kernel::platform::get();
      create_internal_region(nullptr,                      &__text_start,              false, "kernel:reserved");
      create_internal_region(&__text_start,                &__text_end,                false, "kernel:text");
      create_internal_region(&__rodata_start,              &__rodata_end,              false, "kernel:rodata");
      create_internal_region(&__data_start,                &__data_end,                true,  "kernel:data");
      create_internal_region(&__bss_start,                 &__bss_end,                 true,  "kernel:bss");
      create_internal_region(platform.hw_mem_desc_begin(), platform.hw_mem_desc_end(), true,  "kernel:hw_mem_desc");
      create_internal_region(devices_start,                devices_end,                true,  "kernel:device_map");
    }

    void create_internal_region(const uint32_t &begin, const uint32_t &end, const bool &can_write, const char *name) {
      region_info *ri = new_internal(begin, end - begin, can_write ? protection{1, 1} : protection{1, 0}, name);
      ri->map_identity();
      insert_region(ri);
    }

    void create_internal_region(const void *begin, const void *end, const bool &can_write, const char *name) {
      create_internal_region(reinterpret_cast<uint32_t>(begin), reinterpret_cast<uint32_t>(end), can_write, name);
    }

    void insert_region(region_info *ri, region_info* prev) {
      if(regions.empty()) {
        regions.add(ri);
        return;
      }

      if(prev) {
        regions.insert_after(prev, ri);
        return;
      }

      for(region_info* rit : regions) {
        if(rit->address() > ri->address()) {
          regions.insert_before(rit, ri);
          return;
        }
      }

      regions.insert_tail(ri);
    }

    region *region::find(const uint32_t &address) {
      for(region_info* ri : regions) {
        if(ri->address() <= address && address < ri->address_end()) {
          return ri;
        }
        if(ri->address() > address) {
          break; // sorted list
        }
      }

      return nullptr;
    }

    region *region::create(const uint32_t &len, const protection &prot, const char *name, const bool &is_internal) {

      // find suitable place
      region_info *prev = nullptr;
      // we do not lookup before kernel mappings
      for(region_info* rit : regions) {
        if(rit == regions.tail()) {
          prev = rit;
          break;
        }
        if(rit->next_node()->address() - rit->address_end() <= len) {
          prev = rit;
          break;
        }
      }

      ASSERT(prev);
      const uint32_t address = prev->address_end();
/*
TODO: permit allocation
      region_info *ri = is_internal ?
        new_internal(address, len, prot, name) :
        new region_info(address, len, prot, name);
*/
      ASSERT(is_internal);
      region_info *ri = new_internal(address, len, prot, name);

      ri->map();
      insert_region(ri, prev);

      return ri;
    }

    region_info *new_internal(const uint32_t &address, const uint32_t &len, const protection &prot, const char *name) {
      ASSERT(internal_count < builtin_regions_size);
      region_info *ri = builtin_regions + (internal_count++);
      return new (ri) region_info(address, len, prot, name);
    }

    void region::release(region *region) {
      region_info *ri = static_cast<region_info *>(region);

      ri->unmap();
      regions.remove(ri);

/*
TODO: permit allocation
      delete ri;
*/
    }

    region *region::get_first() {
      return regions.head();
    }

    region *region::get_next(region *region) {
      region_info *ri = static_cast<region_info *>(region);
      auto next = ri->next_node();
      if(next == regions.head()) {
        return nullptr;
      }
      return next;
    }
  }
}
