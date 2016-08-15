RM = rm
MAKE = make
COPY = cp

KERNEL_BIN = rpi-js-os

all: $(KERNEL_BIN) $(KERNEL_BIN).qemu

clean:
	$(RM) -rf build $(KERNEL_BIN) $(KERNEL_BIN).qemu

$(KERNEL_BIN):
	$(MAKE) CONFIG=rpi DEFINES= LINKER_SCRIPT=linker-rpi.ld -f Makefile.kernel
	$(COPY) build/rpi/$(KERNEL_BIN) $@

$(KERNEL_BIN).qemu:
	$(MAKE) CONFIG=qemu DEFINES=-DQEMU LINKER_SCRIPT=linker-qemu.ld -f Makefile.kernel
	$(COPY) build/qemu/$(KERNEL_BIN) $@

qemu: $(KERNEL_BIN).qemu
	qemu-system-arm \
		-kernel $(KERNEL_BIN).qemu \
		-cpu arm1176 \
		-m 256 \
		-M versatilepb \
		-no-reboot \
		-nographic
#    -serial stdio \
#    -append "root=/dev/sda2 panic=1 rootfstype=ext4 rw" \
#    -hda 2014-12-24-wheezy-raspbian.img

.PHONY: qemu kernel all clean $(KERNEL_BIN) $(KERNEL_BIN).qemu
