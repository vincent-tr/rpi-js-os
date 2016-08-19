#include <stddef.h>
#include <stdint.h>

#include "kernel/hw/uart.hh"
#include "kernel/utils/debug.hh"

extern "C" void _init();

extern "C" void kernel_main(uint32_t boot_device, uint32_t machine_type, uint32_t atags) {
  _init();

  (void) boot_device;
  (void) machine_type;
  (void) atags;

  kernel::hw::uart::init();

  DEBUG("Hello, kernel World!");
}