# Exercise 1-1

## 編譯
```bash
make build
riscv64-unknown-elf-objcopy -O binary kernel.elf kernel.bin
mkimage -f kernel.its kernel.fit
```
再將 SD 卡中的 kernel.its 替換成新的 kernel.its 
