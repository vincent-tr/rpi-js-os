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

    constexpr uint32_t regions_count = 200;
    kernel::mm::region *regions[regions_count];
    const uint32_t &page_size = kernel::platform::get().page_size();

    for(uint32_t i=0; i<4; ++i) {

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

      for(uint32_t i=0; i<regions_count; ++i) {
        kernel::mm::region *r = regions[i];
        for(uint32_t addr = r->address(); addr < r->address_end(); addr += sizeof(addr)) {
          ASSERT(*reinterpret_cast<uint32_t *>(addr) == r->address());
        }
        kernel::mm::region::release(r);
      }

      DEBUG(regions_count << " regions released");
    }

    print_regions();

    DEBUG("region test end");
  }

  static constexpr uint32_t ptr_len = 10000;
  static uint32_t *ptr[ptr_len];

  static void mm_allocator() {
    DEBUG("allocator test begin");

    for(uint32_t count=0; count<4; ++count) {

      uint32_t total_size = 0;

      for(uint32_t idx=0; idx<ptr_len; ++idx) {
        uint32_t *current = new uint32_t[idx+1];
        total_size += (idx+1) * sizeof(uint32_t);
        ptr[idx] = current;
        for(uint32_t i=0; i<idx; ++i) {
          current[i] = reinterpret_cast<uint32_t>(current);
        }
      }

      DEBUG("allocated " << ptr_len << " times (total_size=" << total_size << ")");

      for(uint32_t idx=0; idx<ptr_len; ++idx) {
        uint32_t *current = ptr[idx];
        for(uint32_t i=0; i<idx; ++i) {
          ASSERT(current[i] == reinterpret_cast<uint32_t>(current));
        }
        delete ptr[idx];
      }

      DEBUG("deallocated " << ptr_len << " times");
    }

    DEBUG("allocator test end");
  }

  void mm() {
    DEBUG("test begin");

    mm_region();
    mm_allocator();

    DEBUG("test end");
  }

}

#endif // TEST_ENABLED
