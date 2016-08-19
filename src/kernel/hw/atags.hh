#ifndef KERNEL_HW_ATAGS_HH
#define KERNEL_HW_ATAGS_HH

namespace kernel {
  namespace hw {
    namespace atags {

      enum type : uint32_t {
        NONE      = 0,
        CORE      = 0x54410001,
        MEM       = 0x54410002,
        VIDEOTEXT = 0x54410003,
        RAMDISK   = 0x54410004,
        INITRD2   = 0x54420005,
        SERIAL    = 0x54410006,
        REVISION  = 0x54410007,
        VIDEOLFB  = 0x54410008,
        CMDLINE   = 0x54410009
      };

      struct header {
        uint32_t size; // Size in words of this tag
        type tag;      // Tag value
      };

      // ATAG_NONE ends the list of ATAGs
      struct none : public header {
        // No further data in this ATAG
      };

      // ATAG_CORE begins the list of ATAGs
      struct core : public header {
        // Optional entries below
        uint32_t flags;      // Bit 0 - read only. Others unused
        uint32_t pagesize;   // Page size
        uint32_t rootdevice; // Root device number
      };

      // ATAG_MEM defines a physical memory region
      struct mem : public header {
        uint32_t size;    // Size of region
        void *address; // Address of start of region
      };

      // ATAG_VIDEOTEXT defines a VGA text screen. Not relevant to a Raspberry Pi

      // ATAG_RAMDISK defines an initial ramdisk - floppy images only?
      struct ramdisk : public header {
        uint32_t flags; // Bit 0 = load, bit 1 = prompt
        uint32_t size;  // Decompressed size in KB
        void *start; // Start block of ram disk
      };

      // ATAG_INITRD2 - define physical location of ramdisk image
      struct initrd2 : public header {
        void *address; // Address of ramdisk image
        uint32_t size;    // Size of compressed(?) image
      };

      // ATAG_SERIAL has the 64-bit serial number
      struct serial : public header {
        uint32_t low;
        uint32_t high;
      };

      // ATAG_REVISION - board revision number
      struct revision : public header {
        uint32_t revision;
      };

      // ATAG_VIDEOLFB - describes a framebuffer
      struct videolfb : public header {
        uint16_t width;       // Width of buffer
        uint16_t height;      // Height
        uint16_t depth;       // Bits/pixel
        uint16_t linelength;  // ?
        void *address;        // Base address of buffer
        uint32_t size;        // Size of buffer
        uint8_t redsize;      // Number of red bits in each pixel
        uint8_t redpos;       // Position of red bits in pixel
        uint8_t greensize;
        uint8_t greenpos;
        uint8_t bluesize;
        uint8_t bluepos;
        uint8_t reservedsize; // Number of reserved bits/pixel
        uint8_t reservedpos;  // Position of reserved bits
      };

      // ATAG_CMDLINE - kernel command line
      struct cmdline : public header {
        char commandline[1]; // Multiple characters from here
      };

      class reader {
        const header *current;

      public:
        explicit reader();

        reader& operator++();
        operator bool() const;
        const header *operator*() const;
        const header *operator->() const;
      };

      void init(const void *address);
    }
  }
}

#endif // KERNEL_HW_ATAGS_HH
