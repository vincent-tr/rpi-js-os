#include <stddef.h>
#include <stdint.h>

#include "kernel/hw/uart.hh"
#include "kernel/hw/atags.hh"
#include "kernel/utils/debug.hh"

extern "C" void _init();

extern "C" void kernel_main(uint32_t boot_device, uint32_t machine_type, const void *atags) {
  _init();

  (void) boot_device;
  (void) machine_type;

  kernel::hw::uart::init();
  kernel::hw::atags::init(atags);

  DEBUG("Hello, kernel World!");
}
