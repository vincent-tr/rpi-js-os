#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>

#include "kernel/hw/uart.hh"
#include "kernel/utils/debug.hh"

#ifdef DEBUG_ENABLED

namespace kernel {
  namespace utils {
    namespace details {

      debug::debug(const char *file, const int &line) {
        *this << "<" << file << ":" << line << "> ";
      }

      debug::~debug() {
        *this << "\n";
      }

      debug &debug::operator << (const char *str) {
        const auto size = strlen(str);
        for(size_t i=0; i<size; ++i) {
          kernel::hw::uart::uart0().write(str[i]);
        }
        return *this;
      }

      debug &debug::operator << (const int &val) {
        char str[16];
        itoa(val, str, 10);
        return (*this) << str;
      }
    }
  }
}

#endif // DEBUG_ENABLED
