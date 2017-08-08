#ifndef KERNEL_UTILS_ALLOCATOR_NEW_HH
#define KERNEL_UTILS_ALLOCATOR_NEW_HH

void *operator new(size_t)              throw();
void *operator new[](size_t)            throw();
void  operator delete  (void *)         throw();
void  operator delete[](void *)         throw();
void  operator delete  (void *, size_t) throw();
void  operator delete[](void *, size_t) throw();

#endif // KERNEL_UTILS_ALLOCATOR_NEW_HH
