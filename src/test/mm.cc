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

  static constexpr uint32_t ptr_len = 10;
  static char *ptr[ptr_len];

  void mm() {
    DEBUG("test begin");

    for(auto *region = kernel::mm::region::get_first(); region; region = kernel::mm::region::get_next(region)) {
      DEBUG(
        "region: " <<
        reinterpret_cast<void*>(region->address()) << " -> " << reinterpret_cast<void*>(region->address_end()) <<
        " (" << region->length() << ") : " << region->name());
    }
/*
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
