#include <stddef.h>
#include <stdint.h>

#include "kernel/kernel-main.hh"

extern "C" void _init();

extern "C" void main(uint32_t boot_device, uint32_t machine_type, const void *atags) {
  _init();

  kernel::kernel_main inst;
  inst.run(boot_device, machine_type, atags);
}
