#include <stddef.h>
#include <stdint.h>

#include "kernel/utils/debug.hh"
#include "kernel/hw/exceptions.hh"

#include "support/newlib/syscalls_def.hh"

namespace kernel {
  namespace utils {

    void panic(const char *msg) {

      hw::exceptions::disable();

      (void)msg;
      DEBUG("PANIC: " << msg);

      while(true);
    }

    static void s_exit(int status) {
      DEBUG("exit called with status=" << status);
      panic("exit");
    }

    // connect to support/syscall_def
    struct connecter {
      connecter() {
        support::exit_impl = s_exit;
      }
    };

    static connecter instance;
  }
}
