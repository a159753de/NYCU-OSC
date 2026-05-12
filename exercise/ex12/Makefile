RISCV_GNU ?= riscv64-unknown-elf
CC = $(RISCV_GNU)-gcc
LD = $(RISCV_GNU)-ld
OBJCOPY = $(RISCV_GNU)-objcopy
CFLAGS = -mcmodel=medany -ffreestanding -nostdlib -g -Wall
QEMU ?= qemu-system-riscv64
TARGET = kernel

# 預設平台為 OPI，可以透過 "make PLATFORM=QEMU" 切換
PLATFORM ?= OPI

# 根據平台選擇不同的 Linker Script 與定義
ifeq ($(PLATFORM), QEMU)
    LDSCRIPT = linker_qemu.ld
    CFLAGS += -DUSE_QEMU
else
    LDSCRIPT = linker_opi.ld
    # 實體板通常不需要特別的 QEMU_OPTS，但保留架構一致性
endif

# 告訴 Makefile 這4個是指令
.PHONY: all build run clean

# 當在 terminal 執行 "make" 時，預設會執行 "all"，而 "all" 會呼叫 "build"
all: build

build: 
	make clean
	$(CC) $(CFLAGS) -c *.S *.c                         
	$(LD) -T $(LDSCRIPT) -o $(TARGET).elf *.o          
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	mkimage -f $(TARGET).its $(TARGET).fit

run: 
	$(MAKE) build PLATFORM=QEMU $(TARGET) 
	$(QEMU) -M virt -m 128M -kernel $(TARGET) -display none -serial stdio

clean:
	rm -f $(TARGET) $(TARGET).elf *.o
	rm -f $(TARGET).bin
	rm -f $(TARGET).fit
