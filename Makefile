include common.mk

all: $(KERNEL_BIN) $(KERNEL_BIN).qemu

clean: clean-kernel
	$(RM) -rf build

clean-kernel:
	$(RM) -rf build/rpi build/qemu $(KERNEL_BIN) $(KERNEL_BIN).qemu

# run separatly because too long
newlib:
	$(MAKE) -f Makefile.newlib

libcxxrt:
	$(MAKE) -f Makefile.libcxxrt

libcxx:
	$(MAKE) -f Makefile.libcxx

$(KERNEL_BIN):
	$(MAKE) CONFIG=rpi DEFINES= -f Makefile.kernel
	$(COPY) build/rpi/$(KERNEL_BIN) $@

$(KERNEL_BIN).qemu:
	$(MAKE) CONFIG=qemu DEFINES=-DQEMU -f Makefile.kernel
	$(COPY) build/qemu/$(KERNEL_BIN) $@

qemu: $(KERNEL_BIN).qemu
	echo "press 'ctrl+a' 'x' to exit"
	qemu-system-arm \
		-kernel $(KERNEL_BIN).qemu \
		-cpu arm1176 \
		-m 256 \
		-M versatilepb \
		-no-reboot \
		-nographic \
		-append "kernel cmd line test" \
		-d guest_errors
#    -serial stdio \
#    -hda 2014-12-24-wheezy-raspbian.img

qemu-debug: $(KERNEL_BIN).qemu
	echo "press 'ctrl+a' 'x' to exit"
	qemu-system-arm \
		-kernel $(KERNEL_BIN).qemu \
		-cpu arm1176 \
		-m 256 \
		-M versatilepb \
		-no-reboot \
		-nographic \
		-append "kernel cmd line test" \
		-d guest_errors \
		 -s -S

start-gdb:
	arm-none-eabi-gdb -x gdb-script

.PHONY: qemu qemu-debug start-gdb kernel all clean clean-kernel $(KERNEL_BIN) $(KERNEL_BIN).qemu newlib
