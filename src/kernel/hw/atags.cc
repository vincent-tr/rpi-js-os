#include <stddef.h>
#include <stdint.h>

#include "atags.hh"
#include "kernel/utils/debug.hh"

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

      const header *reader::operator->() const {
        return current;
      }

      void init(const void *address) {
        tags = reinterpret_cast<const header *>(address);

#ifdef DEBUG_ENABLED

        for(reader r; r; ++r) {
          switch(r->tag) {
            case type::CORE      : DEBUG("atag core:"); break;
            case type::MEM       : DEBUG("atag mem:"); break;
            case type::VIDEOTEXT : DEBUG("atag videotext:"); break;
            case type::RAMDISK   : DEBUG("atag ramdisk:"); break;
            case type::INITRD2   : DEBUG("atag initrd:"); break;
            case type::SERIAL    : DEBUG("atag serial:"); break;
            case type::REVISION  : DEBUG("atag revision:"); break;
            case type::VIDEOLFB  : DEBUG("atag videolfb:"); break;
            case type::CMDLINE   : DEBUG("atag cmdline:"); break;
            default: DEBUG("atag <unknown>"); break;
          }
        }

#endif
      }
    }
  }
}

