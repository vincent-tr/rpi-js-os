#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/mm/protection.hh"
#include "kernel/mm/region.hh"
#include "kernel/platform.hh"
#include "kernel/utils/list.hh"
#include "kernel/utils/allocator-new.hh"
#include "test.hh"

#ifdef TEST_ENABLED

namespace test {

  static void print_regions() {
    for(auto *region = kernel::mm::region::get_first(); region; region = kernel::mm::region::get_next(region)) {
      DEBUG(
        "region: " <<
        reinterpret_cast<void*>(region->address()) << " -> " << reinterpret_cast<void*>(region->address_end()) <<
        " (" << region->length() << ") : " << region->name());
    }
  }

  static void mm_region() {
    DEBUG("region test begin");

    print_regions();

    constexpr uint32_t regions_count = 2;
    kernel::mm::region *regions[regions_count];
    const uint32_t &page_size = kernel::platform::get().page_size();

    uint32_t total_size = 0;
    for(uint32_t i=0; i<regions_count; ++i) {
      uint32_t size = (i+1) * page_size;
      kernel::mm::region *r = kernel::mm::region::create(size, kernel::mm::protection{1,1}, "test");
      total_size += size;
      regions[i] = r;
      for(uint32_t addr = r->address(); addr < r->address_end(); addr += sizeof(addr)) {
        *reinterpret_cast<uint32_t *>(addr) = r->address();
      }
    }

    DEBUG(regions_count << " regions created (total_size=" << total_size << ")");
    print_regions();

    for(uint32_t i=0; i<regions_count; ++i) {
      kernel::mm::region *r = regions[i];
      for(uint32_t addr = r->address(); addr < r->address_end(); addr += sizeof(addr)) {
        ASSERT(*reinterpret_cast<uint32_t *>(addr) == r->address());
      }
      kernel::mm::region::release(r);
    }

    DEBUG(regions_count << " regions released");
    print_regions();

    DEBUG("region test end");
  }

  void mm() {
    DEBUG("test begin");

    mm_region();
/*

    static constexpr uint32_t ptr_len = 10;
    static char *ptr[ptr_len];

    for(uint32_t i=0; i<ptr_len; ++i) {
      DEBUG("before alloc " << i);
      ptr[i] = new char[i+1];
      DEBUG("after alloc " << i);
    }

    DEBUG("allocated " << ptr_len << " times");

    for(auto *region = kernel::mm::region::get_first(); region; region = kernel::mm::region::get_next(region)) {
      DEBUG(
        "region: " <<
        reinterpret_cast<void*>(region->address()) << " -> " << reinterpret_cast<void*>(region->address_end()) <<
        " (" << region->length() << ") : " << region->name());
    }

    for(uint32_t i=0; i<ptr_len; ++i) {
      delete ptr[i];
    }

    DEBUG("deallocated " << ptr_len << " times");

    for(auto *region = kernel::mm::region::get_first(); region; region = kernel::mm::region::get_next(region)) {
      DEBUG(
        "region: " <<
        reinterpret_cast<void*>(region->address()) << " -> " << reinterpret_cast<void*>(region->address_end()) <<
        " (" << region->length() << ") : " << region->name());
    }
*/
    DEBUG("test end");
  }

}

#endif // TEST_ENABLED
