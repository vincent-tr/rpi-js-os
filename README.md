vm :
====
 - arch linux
 - pacman -S screen gcc make arm-none-eabi-gcc qemu qemu-arch-extra gdb arm-none-eabi-gdb

newlib tuning :
===============
 - newlib/-xxx/newlib/configure.host : newlib_cflags="-DMALLOC_PROVIDED"