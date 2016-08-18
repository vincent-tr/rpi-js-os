#include <stddef.h>
#include <stdint.h>

#include "kernel/hw/uart.hh"
#include "kernel/utils/debug.hh"

extern "C" void _init();

extern "C" void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
  _init();

  (void) r0;
  (void) r1;
  (void) atags;

  kernel::hw::uart::init();

  DEBUG("Hello, kernel World!");
}