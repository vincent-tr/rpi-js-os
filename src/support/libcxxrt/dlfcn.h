#ifndef SUPPORT_LIBCXXRT_DLFCN_H
#define SUPPORT_LIBCXXRT_DLFCN_H

typedef struct {
   const char *dli_fname;
   void       *dli_fbase;
   const char *dli_sname;
   void       *dli_saddr;
} Dl_info;

int dladdr(void*, Dl_info*) {
  return 0;
}

#endif