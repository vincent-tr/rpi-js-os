#include <stddef.h>
#include <stdint.h>

#include "allocator.hh"

void *operator new(size_t s)     throw() {
  return kernel::mm::allocator::allocate(s);
}

void *operator new[](size_t s)   throw() {
  return kernel::mm::allocator::allocate(s);
}

void  operator delete(void *p) throw() {
  kernel::mm::allocator::deallocate(p);
}

void  operator delete(void *p, size_t) throw() {
  kernel::mm::allocator::deallocate(p);
}

void  operator delete[](void *p) throw() {
  kernel::mm::allocator::deallocate(p);
}

void  operator delete[](void *p, size_t) throw() {
  kernel::mm::allocator::deallocate(p);
}
