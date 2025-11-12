CROSS ?= aarch64-elf-
CC := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

SRC_DIR := src
INC_DIR := include
ASM_DIR := asm

CFLAGS := -O2 -Wall -Wextra -ffreestanding -fno-builtin -nostdlib -nostartfiles -mcpu=cortex-a53 -fno-stack-protector -fno-pic -I$(INC_DIR)
LDFLAGS := -T linker.ld -nostdlib

SRCS_C := $(wildcard $(SRC_DIR)/*.c)
SRCS_S := $(wildcard $(ASM_DIR)/*.S)
OBJS := $(SRCS_C:.c=.o) $(SRCS_S:.S=.o)

all: kernel8.img

kernel8.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) -Wl,$(LDFLAGS) -o $@ $(OBJS)

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_DIR)/%.o: $(ASM_DIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(ASM_DIR)/*.o *.elf *.img

.PHONY: all clean

