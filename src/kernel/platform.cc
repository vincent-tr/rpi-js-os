#include <stddef.h>
#include <stdint.h>

#include "platform.hh"

#include "kernel/hw/uart.hh"
#include "kernel/hw/atags.hh"
#include "kernel/hw/interrupts.hh"
#include "kernel/hw/memory.hh"
#include "kernel/utils/debug.hh"

namespace kernel {
  static platform instance;

  platform &platform::get() {
    return instance;
  }

  void platform::run(const uint32_t &pboot_device, const uint32_t &pmachine_type, const void *patags) {
    boot_device = pboot_device;
    machine_type = pmachine_type;

    kernel::hw::uart::init();
    kernel::hw::atags::init(patags);
    kernel::hw::interrupts::init();
    // TODO: read atags
    kernel::hw::memory::init(page_size, ram_size);

    DEBUG("Hello, kernel World!");
  }

}
