#include <stddef.h>
#include <stdint.h>

#include "kernel/hw/uart.hh"
#include "kernel/utils/debug.hh"

#ifdef DEBUG_ENABLED

// TODO: libc
size_t strlen(const char* str)
{
  size_t ret = 0;
  while ( str[ret] != 0 )
    ret++;
  return ret;
}

void uart_write(const unsigned char* buffer, size_t size)
{
  for ( size_t i = 0; i < size; i++ )
    kernel::hw::uart::uart0().write(buffer[i]);
}

void uart_puts(const char* str)
{
  uart_write((const unsigned char*) str, strlen(str));
}
// ---

namespace kernel {
  namespace utils {
    namespace details {

      static void debug_write(const char *text) {
        uart_puts(text);
      }

      debug::debug(const char *file, const int &line) {
        debug_write("<");
        debug_write(file);
        debug_write("> ");
      }

      debug::~debug() {
        debug_write("\n");
      }

      debug &debug::operator << (const char *str) {
        debug_write(str);
        return *this;
      }
    }
  }
}

#endif // DEBUG_ENABLED
