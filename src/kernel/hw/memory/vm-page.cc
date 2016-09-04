#include <stddef.h>
#include <stdint.h>

#include "phys-page.hh"
#include "vm-page.hh"
#include "kernel/utils/placement-new.hh"
#include "kernel/utils/debug.hh"
#include "kernel/utils/list.hh"
#include "kernel/platform.hh"

namespace kernel {
  namespace hw {
    namespace memory {

      phys_page vm_page::phys() const {
        // TODO
      }

      const vm_protection &vm_page::protection() const {
        // TODO
      }

      void vm_page::map(const vm_protection &prot, const phys_page &phys) {
        // TODO
      }

      void vm_page::unmap() {
        // TODO
      }

      void vm_page::init(uint32_t &desc_end) {
        // TODO
      }
    }
  }
}
