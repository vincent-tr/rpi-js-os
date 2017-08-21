#ifndef KERNEL_MM_REGION_HH
#define KERNEL_MM_REGION_HH

namespace kernel {
  namespace mm {

    class region {
      uint32_t addr;
      uint32_t len;
      protection _prot;
      const char *_name;

    public:
      explicit inline region(const uint32_t &vaddr, const uint32_t &plen, const protection &pprot, const char *pname)
       : addr(vaddr)
       , len(plen)
       , _prot(pprot)
       , _name(pname) {
      }

      inline uint32_t address() const {
        return addr;
      }

      inline uint32_t address_end() const {
        return addr + len;
      }

      inline uint32_t length() const {
        return len;
      }

      inline protection prot() const {
        return _prot;
      }

      inline const char * name() const {
        return _name;
      }

      static void init();
      static void clean_reserved();

      static region *find(const uint32_t &address);

      static region *create(const uint32_t &len, const protection &prot, const char *name);
      static region *create(const uint32_t &address, const uint32_t &len, const protection &prot, const char *name);
      static void release(region *region);

      static const region *get_first();
      static const region *get_next(const region *region);

      static const region *get_stack();
      static const region *get_exc_stack();
    };
  }
}

#endif // KERNEL_MM_REGION_HH
