RISCV_GNU ?= riscv64-unknown-elf
CC = $(RISCV_GNU)-gcc
LD = $(RISCV_GNU)-ld
OBJCOPY = $(RISCV_GNU)-objcopy
CFLAGS = -mcmodel=medany -ffreestanding -nostdlib -g -Wall
QEMU ?= qemu-system-riscv64
TARGET = kernel
PAYLOAD = new_kernel

PLATFORM ?= OPI

ifeq ($(PLATFORM), QEMU)
    LDSCRIPT = linker_qemu.ld
    PAYLOAD_LDSCRIPT = linker_qemu_new_kernel.ld
    CFLAGS += -DUSE_QEMU
else
    LDSCRIPT = linker_opi.ld
    PAYLOAD_LDSCRIPT = linker_opi_new_kernel.ld
endif

.PHONY: all build build-payload run clean

all: build

build:
	make clean
	$(CC) $(CFLAGS) -c *.S *.c
	$(LD) -T $(LDSCRIPT) -o $(TARGET).elf *.o
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	mkimage -f $(TARGET).its $(TARGET).fit

build-payload: build
	$(LD) -T $(PAYLOAD_LDSCRIPT) -o $(PAYLOAD).elf *.o
	$(OBJCOPY) -O binary $(PAYLOAD).elf $(PAYLOAD)

run:
	$(MAKE) build PLATFORM=QEMU
# 	$(QEMU) -M virt -m 128M -kernel $(TARGET) -display none -serial pty
	$(QEMU) -M virt -m 128M -kernel $(TARGET) -display none -serial stdio

clean:
	rm -f $(TARGET) $(TARGET).elf *.o
	rm -f $(TARGET).bin $(TARGET).fit
	rm -f $(PAYLOAD) $(PAYLOAD).elf
