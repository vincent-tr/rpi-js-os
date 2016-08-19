#include <stddef.h>
#include <stdint.h>

#include "atags.hh"
#include "kernel/debug.hh"

namespace kernel {
  namespace hw {
    namespace atags {

      static const header *tags;

      reader::reader()
       : current(tags) {
      }

      reader& reader::operator++() {
        current = reinterpret_cast<const header *>(reinterpret_cast<uint32_t>(current) + (current->size * sizeof(uint32_t)));
        return *this;
      }

      reader::operator bool() const {
        return current->tag != type::NONE;
      }

      const header *reader::operator*() const {
        return current;
      }

      void init(const void *address) {
        tags = reinterpret_cast<const header *>(address);

#ifdef DEBUG_ENABLED

#endif
      }
    }
  }
}

