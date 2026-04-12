.section ".text.boot"
.global _start
_start:
    // TODO: Clear the .bss section
    // load the bss start & stop address
    la   t0, __bss_start
    la   t1, __bss_stop

bss_loop:
    bgeu t0, t1, bss_done  // 如果起始位址 >= 結束位址，跳出迴圈
    sd   zero, 0(t0)       // 將 0 寫入當前 t0 指向的位址 (每次 8 bytes)
    addi t0, t0, 8         // 指標往後移動 8 bytes
    j    bss_loop          // 繼續循環

bss_done:
    // TODO: Set up the stack pointer
    la   sp, _end
stop:   tail start_kernel
