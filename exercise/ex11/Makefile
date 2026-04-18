RISCV_GNU ?= riscv64-unknown-elf
CC = $(RISCV_GNU)-gcc
LD = $(RISCV_GNU)-ld
OBJCOPY = $(RISCV_GNU)-objcopy
CFLAGS = -mcmodel=medany -ffreestanding -nostdlib -g -Wall
QEMU ?= qemu-system-riscv64
TARGET = kernel

build: 
	make clean
	$(CC) $(CFLAGS) -c *.S *.c
	$(LD) -T link.ld -o $(TARGET).elf *.o
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	mkimage -f $(TARGET).its $(TARGET).fit

run: build $(TARGET)
	$(QEMU) -M virt -m 128M -kernel $(TARGET) -display none -serial stdio

clean:
	rm -f $(TARGET) $(TARGET).elf *.o
	rm -f $(TARGET).bin
	rm -f $(TARGET).fit
