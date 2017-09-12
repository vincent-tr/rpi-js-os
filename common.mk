TOOLCHAIN_PREFIX = arm-none-eabi-

CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)gcc
LINK = $(TOOLCHAIN_PREFIX)gcc
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

COMMON_FLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding $(DEFINES)
COMMON_ASFLAGS = $(COMMON_FLAGS)
COMMON_CFLAGS = -std=gnu99 -O2 -Wall -Wextra $(COMMON_FLAGS)
COMMON_CXXFLAGS = -std=gnu++14 -O2 -Wall -Wextra -pedantic -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics $(COMMON_FLAGS)

NEWLIB_VERSION = 2.5.0

MKDIR = mkdir
FIND = find
RM = rm
MAKE = make
COPY = cp

KERNEL_BIN = rpi-js-os