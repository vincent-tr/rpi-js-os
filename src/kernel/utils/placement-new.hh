#ifndef KERNEL_UTILS_PLACEMENT_NEW_HH
#define KERNEL_UTILS_PLACEMENT_NEW_HH

inline void *operator new(size_t, void *p)     throw() { return p; }
inline void *operator new[](size_t, void *p)   throw() { return p; }
inline void  operator delete  (void *, void *) throw() { }
inline void  operator delete[](void *, void *) throw() { }

#endif // KERNEL_UTILS_PLACEMENT_NEW_HH
