TOOL_PATH = /Users/vincent/workspace/sandbox/rpi-js-os/cross-toolchain/build/bin
CC = $(TOOL_PATH)/arm-none-eabi-gcc
CXX = $(TOOL_PATH)/arm-none-eabi-gcc
LINK = $(TOOL_PATH)/arm-none-eabi-gcc
OBJCOPY = $(TOOL_PATH)/arm-none-eabi-objcopy
RM = rm

KERNEL_BIN = rpi-js-os

ASFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding
CXXFLAGS = -mcpu=arm1176jzf-s -DQEMU -fpic -ffreestanding -std=gnu++14 -O2 -Wall -Wextra -pedantic -fno-rtti -fno-exceptions
LDFLAGS = -ffreestanding -O2 -nostdlib

all: kernel

clean:
	$(RM) -f *.o *.elf $(KERNEL_BIN).qemu $(KERNEL_BIN)

$(KERNEL_BIN).qemu.elf:
	$(CC) -DQEMU $(ASFLAGS) -c boot.S -o boot.o
	$(CXX) -DQEMU $(CXXFLAGS) -c kernel.cc -o kernel.o
	$(LINK) $(LDFLAGS) -T linker-qemu.ld -o $(KERNEL_BIN).qemu.elf boot.o kernel.o

$(KERNEL_BIN).elf:
	$(CC) $(ASFLAGS) -c boot.S -o boot.o
	$(CXX) $(CXXFLAGS) -c kernel.c -o kernel.o
	$(LINK) $(LDFLAGS) -T linker-rpi.ld -o $(KERNEL_BIN).elf boot.o kernel.o

$(KERNEL_BIN).qemu: $(KERNEL_BIN).qemu.elf
	$(OBJCOPY) $(KERNEL_BIN).qemu.elf -O binary $(KERNEL_BIN).qemu

$(KERNEL_BIN): $(KERNEL_BIN).elf
	$(OBJCOPY) $(KERNEL_BIN).elf -O binary $(KERNEL_BIN)

kernel: $(KERNEL_BIN).qemu $(KERNEL_BIN)

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

.PHONY: qemu kernel clean