#include <stddef.h>
#include <stdint.h>

#include "kernel-main.hh"

#include "kernel/hw/uart.hh"
#include "kernel/hw/atags.hh"
#include "kernel/hw/interrupts.hh"
#include "kernel/hw/memory.hh"
#include "kernel/utils/debug.hh"

namespace kernel {
  static kernel_main instance;

  kernel_main &kernel_main::get() {
    return instance;
  }

  void kernel_main::run(const uint32_t &pboot_device, const uint32_t &pmachine_type, const void *patags) {
    boot_device = pboot_device;
    machine_type = pmachine_type;

    kernel::hw::uart::init();
    kernel::hw::atags::init(patags);
    kernel::hw::interrupts::init();
    kernel::hw::memory::init();

    DEBUG("Hello, kernel World!");
  }
}
