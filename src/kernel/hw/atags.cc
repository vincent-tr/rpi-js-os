#include <stddef.h>
#include <stdint.h>

#include <string.h>

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

#ifdef DEBUG_ENABLED

      static void dump() {
        for(reader r; r; ++r) {
          switch(r->tag) {
            case type::CORE: {
              auto tag = static_cast<const core *>(*r);
              DEBUG("atag core: flags=" << tag->flags << ", pagesize=" << tag->pagesize << ", rootdevice=" << tag->rootdevice);
              break;
            }

            case type::MEM: {
              auto tag = static_cast<const mem *>(*r);
              DEBUG("atag mem: size=" << tag->size << ", address=" << tag->address);
              break;
            }

            case type::VIDEOTEXT: {
              DEBUG("atag videotext");
              break;
            }

            case type::RAMDISK: {
              auto tag = static_cast<const ramdisk *>(*r);
              DEBUG("atag ramdisk: flags=" << tag->flags << ", size=" << tag->size << ", start=" << tag->start);
              break;
            }

            case type::INITRD2: {
              auto tag = static_cast<const initrd2 *>(*r);
              DEBUG("atag initrd: address=" << tag->address << ", size=" << tag->size);
              break;
            }

            case type::SERIAL: {
              auto tag = static_cast<const serial *>(*r);
              DEBUG("atag serial: low=" << tag->low << ", high=" << tag->high);
              break;
            }

            case type::REVISION: {
              auto tag = static_cast<const revision *>(*r);
              DEBUG("atag revision: revision=" << tag->revision);
              break;
            }

            case type::VIDEOLFB: {
              auto tag = static_cast<const videolfb *>(*r);
              DEBUG("atag videolfb: width=" << tag->width <<
                ", height=" << tag->height <<
                ", depth=" << tag->depth <<
                ", linelength=" << tag->linelength <<
                ", address=" << tag->address <<
                ", size=" << tag->size <<
                ", redsize=" << tag->redsize <<
                ", redpos=" << tag->redpos <<
                ", greensize=" << tag->greensize <<
                ", greenpos=" << tag->greenpos <<
                ", bluesize=" << tag->bluesize <<
                ", bluepos=" << tag->bluepos <<
                ", reservedsize=" << tag->reservedsize <<
                ", reservedpos=" << tag->reservedpos);
              break;
            }

            case type::CMDLINE: {
              auto tag = static_cast<const cmdline *>(*r);
              DEBUG("atag cmdline: cmdline=" << tag->commandline);
              break;
            }

            default: {
              DEBUG("atag <unknown>");
              break;
            }
          }
        }
      }

#endif


      void init(const void *address) {
        tags = reinterpret_cast<const header *>(address);

#ifdef DEBUG_ENABLED
        dump();
#endif
      }
    }
  }
}

