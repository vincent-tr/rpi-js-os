#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/hw/interrupts.hh"

namespace kernel {
  namespace utils {

    void panic(const char *msg) {
      hw::interrupts::disable();
      DEBUG("PANIC: " << msg);
      while(true);
    }
  }
}
