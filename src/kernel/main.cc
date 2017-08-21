#include <stddef.h>
#include <stdint.h>

#include "kernel/platform.hh"

namespace kernel {

  extern "C" void _init();
  extern "C" uint32_t __bss_start;
  extern "C" uint32_t __bss_end;

  static void init_bss() {
    uint32_t* bss = &__bss_start;
    uint32_t* bss_end = &__bss_end;

    while(bss < bss_end) {
      *bss++ = 0;
    }
  }

  extern "C" void main(uint32_t boot_device, uint32_t machine_type, const void *atags) {
    init_bss();
    _init();

    kernel::platform::get().run(boot_device, machine_type, atags);

    for(;;);
  }
}
