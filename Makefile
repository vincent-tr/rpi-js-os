TOOL_PATH = /Users/vincent/workspace/sandbox/rpi-js-os/cross-toolchain/build/bin
CC = $(TOOL_PATH)/arm-none-eabi-gcc
CXX = $(TOOL_PATH)/arm-none-eabi-gcc
LINK = $(TOOL_PATH)/arm-none-eabi-gcc
OBJCOPY = $(TOOL_PATH)/arm-none-eabi-objcopy

KERNEL_BIN = rpi-js-os

all: kernel

$(KERNEL_BIN).elf:
	$(CC) -mcpu=arm1176jzf-s -fpic -ffreestanding -c boot.S -o boot.o
	$(CC) -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c kernel.c -o kernel.o -O2 -Wall -Wextra
	$(LINK) -T linker-qemu.ld -o $(KERNEL_BIN).elf -ffreestanding -O2 -nostdlib boot.o kernel.o

$(KERNEL_BIN): $(KERNEL_BIN).elf
	$(OBJCOPY) $(KERNEL_BIN).elf -O binary $(KERNEL_BIN)

kernel: $(KERNEL_BIN)

qemu:
	qemu-system-arm \
		-kernel $(KERNEL_BIN) \
		-cpu arm1176 \
		-m 256 \
		-M versatilepb \
		-no-reboot \
		-nographic
#    -serial stdio \
#    -append "root=/dev/sda2 panic=1 rootfstype=ext4 rw" \
#    -hda 2014-12-24-wheezy-raspbian.img

.PHONY: qemu kernel