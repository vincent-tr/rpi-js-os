#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/mm/protection.hh"
#include "kernel/mm/region.hh"
#include "kernel/platform.hh"
#include "kernel/utils/list.hh"
#include "test.hh"

#ifdef TEST_ENABLED

namespace test {

  void mm() {
    DEBUG("test begin");

    for(auto *region = kernel::mm::region::get_first(); region; region = kernel::mm::region::get_next(region)) {
      DEBUG(
        "region: " <<
        reinterpret_cast<void*>(region->address()) << " -> " << reinterpret_cast<void*>(region->address_end()) <<
        " (" << region->length() << ") : " << region->name());
    }

    DEBUG("test end");
  }

}

#endif // TEST_ENABLED
