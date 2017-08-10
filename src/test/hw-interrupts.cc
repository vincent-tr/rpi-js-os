#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/utils/panic.hh"
#include "kernel/mm/protection.hh"
#include "kernel/mm/region.hh"
#include "kernel/platform.hh"
#include "test.hh"

#ifdef TEST_ENABLED

namespace test {

  void hw_interrupts() {
    DEBUG("test begin");

    // get an address and release the region
    const uint32_t &page_size = kernel::platform::get().page_size();
    auto *reg = kernel::mm::region::create(page_size, kernel::mm::protection{1,1}, "temp");
    uint32_t *addr = reinterpret_cast<uint32_t *>(reg->address());
    kernel::mm::region::release(reg);

    DEBUG("addr=" << addr);
    DEBUG("addr value=" << *addr);

    DEBUG("test end");
  }

}

#endif // TEST_ENABLED
