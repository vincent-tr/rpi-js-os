#include <stddef.h>
#include <stdint.h>

#include <string.h>

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

    static constexpr uint32_t stack_start = 0;
    static const uint32_t stack_end = reinterpret_cast<uint32_t>(&__text_start);

    struct region_info;

    static void create_internal_region(const uint32_t &begin, const uint32_t &end, const bool &can_write, const char *name);
    static void create_internal_region(const void *begin, const void *end, const bool &can_write, const char *name);
    static void create_and_map_region(region_info *ri, region_info **oprev, const uint32_t &len, const protection &prot, const char *name);
    static void insert_region(region_info *ri, region_info *prev);
    static void remove_region(region_info *ri);

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

    class region_slab : public utils::list<region_slab>::node {
      uint32_t free_count;
      utils::list<region_info> free_list;
      region_info *_this_region;

    public:
      static uint32_t obj_count() {
        const uint32_t &page_size = kernel::platform::get().page_size();
        return (page_size - sizeof(region_slab)) / sizeof(region_info);
      }

      explicit inline region_slab(region_info *reg)
       : free_count(0) {
        ASSERT(this == reinterpret_cast<void*>(reg->address()));
        for(uint32_t addr = reg->address() + sizeof(region_slab); addr + sizeof(region_info) <= reg->address_end(); addr += sizeof(region_info)) {
          region_info *obj = reinterpret_cast<region_info *>(addr);
          free_list.add(obj);
          ++free_count;
        }

        // move current region to first object
        _this_region = allocate();
        memcpy(_this_region, reg, sizeof(region_info));
      }

      region_info *allocate() {
        ASSERT(free_count);
        region_info *obj = free_list.head();
        free_list.remove(obj);
        --free_count;
        return obj;
      }

      void deallocate(region_info *obj) {
        free_list.add(obj);
        ++free_count;
      }

      bool full() const {
        return !free_count;
      }

      bool should_release() const {
        return free_count + 1 == obj_count();
      }

      region_info *this_region() {
        return _this_region;
      }
    };

    class region_cache {

      // stack
      // 4x kernel mapping
      // phys mem desc array
      // uart0 mapping
      // TODO: move stack + setup stack guard
      static constexpr uint32_t builtin_regions_count = 7;
      char builtin_regions_buffer[ builtin_regions_count * sizeof(region_info) ];
      region_info *builtin_regions;
      uint32_t builtin_use_count;

      utils::list<region_slab> slabs;
      region_slab *usable_slab;

    public:
      explicit inline region_cache()
       : builtin_regions(reinterpret_cast<region_info *>(builtin_regions_buffer))
       , builtin_use_count(0)
       , usable_slab(nullptr) {
      }

    private:

      region_slab *find_slab(region_info *ptr) {
        // every page begin
        uint32_t addr = reinterpret_cast<uint32_t>(ptr);
        const uint32_t &page_size = kernel::platform::get().page_size();
        addr = addr / page_size * page_size;
        return reinterpret_cast<region_slab *>(addr);
      }

      void find_usable() {
        if(usable_slab) {
          return;
        }

        for(auto *s : slabs) {
          if(!s->full()) {
            usable_slab = s;
            return;
          }
        }

        // allocate new region/slab
        char local_region_buffer[ sizeof(region_info) ];
        region_info *ri = reinterpret_cast<region_info *>(local_region_buffer);
        region_info *prev;

        create_and_map_region(ri, &prev, kernel::platform::get().page_size(), protection{1,1}, "kernel:region:slab");

        usable_slab = reinterpret_cast<region_slab *>(ri->address());
        new (usable_slab) region_slab(ri);
        // region copied from stack to slab
        ri = usable_slab->this_region();

        insert_region(ri, prev);
        slabs.add(usable_slab);
      }

    public:

      region_info *allocate_builtin() {
        ASSERT(builtin_use_count < builtin_regions_count);
        return builtin_regions + (builtin_use_count++);
      }

      region_info *allocate() {
        find_usable();

        region_info *ret = usable_slab->allocate();

        if(usable_slab->full()) {
          usable_slab = nullptr;
        }

        return ret;
      }

      void deallocate(region_info *ptr) {
        region_slab *s = find_slab(ptr);
        ASSERT(s);

        s->deallocate(ptr);

        if(!s->should_release()) {
          return;
        }

        if(usable_slab == s) { usable_slab = nullptr; }
        slabs.remove(s);
        remove_region(s->this_region());

        //copy region to local before unmap it because its address is unside the mapping
        char local_region_buffer[ sizeof(region_info) ];
        region_info *ri = reinterpret_cast<region_info *>(local_region_buffer);
        memcpy(ri, s->this_region(), sizeof(region_info));
        ri->unmap();
      }
    };

    class region_list {
      utils::list<region_info> regions;

    public:

      void insert(region_info *ri, region_info* prev = nullptr) {
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

      void remove(region_info *ri) {
        regions.remove(ri);
      }

      region_info *find(const uint32_t &address) {
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

      // return prev
      region_info *find_place(const uint32_t &len) {

        for(region_info* rit : regions) {

          if(rit == regions.tail()) {
            return rit;
          }

          if(rit->next_node()->address() - rit->address_end() >= len) {
            return rit;
          }
        }

        return nullptr;
      }

      const region_info *get_first() const {
        return regions.head();
      }

      const region_info *get_next(const region_info *ri) const {
        auto next = ri->next_node();
        if(next == regions.head()) {
          return nullptr;
        }
        return next;
      }
    };

    static region_list list;
    static region_cache cache;

    void region::init() {
      const auto &platform = kernel::platform::get();
      create_internal_region(stack_start,                  stack_end,                  true, "kernel:stack");
      create_internal_region(&__text_start,                &__text_end,                false, "kernel:text");
      create_internal_region(&__rodata_start,              &__rodata_end,              false, "kernel:rodata");
      create_internal_region(&__data_start,                &__data_end,                true,  "kernel:data");
      create_internal_region(&__bss_start,                 &__bss_end,                 true,  "kernel:bss");
      create_internal_region(platform.hw_mem_desc_begin(), platform.hw_mem_desc_end(), true,  "kernel:hw_mem_desc");
      create_internal_region(devices_start,                devices_end,                true,  "kernel:device_map");
    }

    void create_internal_region(const uint32_t &begin, const uint32_t &end, const bool &can_write, const char *name) {
      region_info *ri = cache.allocate_builtin();
      new (ri) region_info(begin, end - begin, can_write ? protection{1, 1} : protection{1, 0}, name);
      ri->map_identity();
      list.insert(ri);
    }

    void create_internal_region(const void *begin, const void *end, const bool &can_write, const char *name) {
      create_internal_region(reinterpret_cast<uint32_t>(begin), reinterpret_cast<uint32_t>(end), can_write, name);
    }

    void create_and_map_region(region_info *ri, region_info **oprev, const uint32_t &len, const protection &prot, const char *name) {

      region_info *prev = list.find_place(len);
      *oprev = prev;
      ASSERT(prev);
      const uint32_t address = prev->address_end();

      new (ri) region_info(address, len, prot, name);
      ri->map();
    }

    void insert_region(region_info *ri, region_info *prev) {
      list.insert(ri, prev);
    }

    void remove_region(region_info *ri) {
      list.remove(ri);
    }

    region *region::find(const uint32_t &address) {
      return list.find(address);
    }

    region *region::create(const uint32_t &len, const protection &prot, const char *name) {

      region_info *ri = cache.allocate();
      region_info *prev;
      create_and_map_region(ri, &prev, len, prot, name);
      insert_region(ri, prev);

      return ri;
    }

    void region::release(region *region) {
      region_info *ri = static_cast<region_info *>(region);

      ri->unmap();
      remove_region(ri);
      cache.deallocate(ri);
    }

    const region *region::get_first() {
      return list.get_first();
    }

    const region *region::get_next(const region *region) {
      return list.get_next(static_cast<const region_info *>(region));
    }
  }
}
