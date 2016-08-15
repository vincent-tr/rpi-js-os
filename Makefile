TOOL_PATH = /Users/vincent/workspace/sandbox/rpi-js-os/cross-toolchain/build/bin
CC = $(TOOL_PATH)/arm-none-eabi-gcc
CXX = $(TOOL_PATH)/arm-none-eabi-gcc
LINK = $(TOOL_PATH)/arm-none-eabi-gcc
OBJCOPY = $(TOOL_PATH)/arm-none-eabi-objcopy
MKDIR = mkdir
RM = rm

KERNEL_BIN = rpi-js-os

ASFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding
CXXFLAGS = -mcpu=arm1176jzf-s -DQEMU -fpic -ffreestanding -std=gnu++14 -O2 -Wall -Wextra -pedantic -fno-rtti -fno-exceptions
LDFLAGS = -ffreestanding -O2 -nostdlib

all: kernel

clean:
	$(RM) -rf build $(KERNEL_BIN) $(KERNEL_BIN).qemu

build/rpi:
	$(MKDIR) -p build/rpi

build/qemu:
	$(MKDIR) -p build/qemu

build/qemu/$(KERNEL_BIN).elf: build/qemu
	$(CC) -DQEMU $(ASFLAGS) -c src/boot.S -o build/qemu/boot.o
	$(CXX) -DQEMU $(CXXFLAGS) -c src/kernel/main.cc -o build/qemu/main.o
	$(LINK) $(LDFLAGS) -T linker-qemu.ld -o build/qemu/$(KERNEL_BIN).elf build/qemu/boot.o build/qemu/main.o

build/rpi/$(KERNEL_BIN).elf: build/rpi
	$(CC) $(ASFLAGS) -c src/boot.S -o build/rpi/boot.o
	$(CXX) $(CXXFLAGS) -c src/kernel/main.cc -o build/rpi/main.o
	$(LINK) $(LDFLAGS) -T linker-rpi.ld -o build/rpi/$(KERNEL_BIN).elf build/rpi/boot.o build/rpi/main.o

$(KERNEL_BIN).qemu: build/qemu/$(KERNEL_BIN).elf
	$(OBJCOPY) build/qemu/$(KERNEL_BIN).elf -O binary $(KERNEL_BIN).qemu

$(KERNEL_BIN): build/rpi/$(KERNEL_BIN).elf
	$(OBJCOPY) build/rpi/$(KERNEL_BIN).elf -O binary $(KERNEL_BIN)

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