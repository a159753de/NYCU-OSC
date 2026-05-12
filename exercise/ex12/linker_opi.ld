/*
 *使用domain cpu，記憶體 address 從 0x00000000 ~ 0x80000000
 */
OUTPUT_ARCH(riscv) /* 指定輸出檔案的目標架構為RSIC-V */
ENTRY(_start) /* kernel會從"_start"符號開始載入 */

/*
 *記憶體 layout
 */
SECTIONS
{
    . = 0x00200000; /* "."代表位置計數器(location counter)，起始記憶體位至設定為 0x00200000 */
    _start = .; /* 將當前 address 附值給符號 "_start" */
    
    /* 存放可執行的 machine code */
    .text : { *(.text.boot) *(.text) } /* 確保啟動用的 boot code 放在最前面，剩下的排在後面 */
    /* 唯讀資料段(Read-Only Data)，存放常數、字串 */
    .rodata : { *(.rodata) }
    /* 資料段，存放已初始化的全域和靜態變數 */ 
    .data : { *(.data) }
    /* bss 段，存放未初始化的全域變數，使用時須先清除成0 */
    .bss : {
        __bss_start = .; /* bss 的起始 address */
        *(.sbss) *(.bss) /* 包含所有的 bss 資料(sbss 是小型 bss) */
        __bss_stop = .; /* 標記 bss 終止 address */
    }
    . = ALIGN(16); /*將當前 address 進行 16 bytes的對齊(也就是16-byte aligned) */
    . = . + 0x4000; /* 手動將 address 向後移動 0x4000(16KB) */
    _end = .; /* 標記結束點 */
}
