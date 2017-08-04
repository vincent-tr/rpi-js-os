#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>

#include "kernel/hw/uart.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/panic.hh"

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

      void debug::fatal() {
        *this << "\n";
        panic("assertion failed!");
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

      debug &debug::operator << (const int8_t &val) {
        return (*this) << static_cast<int>(val);
      }

      debug &debug::operator << (const int16_t &val) {
        return (*this) << static_cast<int>(val);
      }

      debug &debug::operator << (const int32_t &val) {
        return (*this) << static_cast<int>(val);
      }

      debug &debug::operator << (const unsigned int &val) {
        char str[16];
        utoa(val, str, 10);
        return (*this) << str;
      }

      debug &debug::operator << (const uint8_t &val) {
        return (*this) << static_cast<unsigned int>(val);
      }

      debug &debug::operator << (const uint16_t &val) {
        return (*this) << static_cast<unsigned int>(val);
      }

      debug &debug::operator << (const uint32_t &val) {
        return (*this) << static_cast<unsigned int>(val);
      }

      debug &debug::operator << (const void *ptr) {
        char buf[16];
        char str[16];

        itoa(reinterpret_cast<uint32_t>(ptr), buf, 16);
        const int len = strlen(buf);

        str[0] = '0';
        str[1] = 'x';
        for(int i=2; i<10 - len; ++i) {
          str[i] = '0';
        }
        memcpy(str + 10 - len, buf, len + 1);
        return (*this) << str;
      }
    }
  }
}

#endif // DEBUG_ENABLED
