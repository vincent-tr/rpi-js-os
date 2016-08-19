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
      class none : public header {
        // No further data in this ATAG
      };

      // ATAG_CORE begins the list of ATAGs
      class core : public header {
        // Optional entries below
        uint32_t flags;      // Bit 0 - read only. Others unused
        uint32_t pagesize;   // Page size
        uint32_t rootdevice; // Root device number
      };

      // ATAG_MEM defines a physical memory region
      class mem : public header {
        uint32_t size;    // Size of region
        uint32_t address; // Address of start of region
      };

      // ATAG_VIDEOTEXT defines a VGA text screen. Not relevant to a Raspberry Pi

      // ATAG_RAMDISK defines an initial ramdisk - floppy images only?
      class ramdisk : public header {
        uint32_t flags; // Bit 0 = load, bit 1 = prompt
        uint32_t size;  // Decompressed size in KB
        uint32_t start; // Start block of ram disk
      };

      // ATAG_INITRD2 - define physical location of ramdisk image
      class initrd2 : public header {
        uint32_t address; // Address of ramdisk image
        uint32_t size;    // Size of compressed(?) image
      };

      // ATAG_SERIAL has the 64-bit serial number
      class serial : public header {
        uint32_t low;
        uint32_t high;
      };

      // ATAG_REVISION - board revision number
      class revision : public header {
        uint32_t revision;
      };

      // ATAG_VIDEOLFB - describes a framebuffer
      class videolfb : public header {
        unsigned short int width;      // Width of buffer
        unsigned short int height;     // Height
        unsigned short int depth;      // Bits/pixel
        unsigned short int linelength; // ?
        uint32_t address;              // Base address of buffer
        uint32_t size;                 // Size of buffer
        unsigned char redsize;         // Number of red bits in each pixel
        unsigned char redpos;          // Position of red bits in pixel
        unsigned char greensize;
        unsigned char greenpos;
        unsigned char bluesize;
        unsigned char bluepos;
        unsigned char reservedsize;    // Number of reserved bits/pixel
        unsigned char reservedpos;     // Position of reserved bits
      };

      // ATAG_CMDLINE - kernel command line
      class cmdline : public header {
        char commandline; // Multiple characters from here
      };

      class reader {
        const header *current;

      public:
        explicit reader();

        reader& operator++();
        operator bool() const;
        const header *operator*() const;
      };

      void init(const void *address);
    }
  }
}

#endif // KERNEL_HW_ATAGS_HH
