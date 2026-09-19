#include "header/sbi.h"
#include "header/tools.h"
#include "header/uart.h"
#include "header/fdt.h"

// 連續讀取指定數量的位元組
void uart_read_bytes(unsigned char *buffer, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        // 呼叫 uart_getc() 來收字元
        buffer[i] = (unsigned char)uart_getc();
    }
}

void do_load()
{
    uart_puts("Loading...\n");
    // 檢查 header 是否正確
    char header[8];
    uart_read_bytes((unsigned char *)header, 8);

    // 解析 header
    char magic[4] = {header[0], header[1], header[2], header[3]};
    unsigned int kernel_size = *(unsigned int *)&header[4]; // 將 header 的後 4 Byte 轉換為 unsigned int

    // 驗證 Magic Number
    if (strncmp(magic, "BOOT", 4) != 0)
    {
        // 驗證失敗
        uart_puts("\nInvalid magic number.\n");
        uart_flush_rx(); // 排空垃圾資料
        return;
    }
    else
    {
        // 驗證成功
        uart_puts("\nMagic number is valid.\n");

        uart_puts("Expected Kernel Size: ");
        uart_put_uint(kernel_size);
        uart_puts(" bytes\n");

        uart_puts("Start loading kernel...\n");

        // 開始將資料存入 memory中
        unsigned char *kernel_addr = (unsigned char *)KERNEL_LOAD_ADDR; // 預設的 kernel 載入地址

        // 有資料的話，就讀取並放入記憶體
        for (unsigned int i = 0; i < kernel_size; i++)
        {
            char c = uart_getc(); // 從 UART 讀取一個字元
            kernel_addr[i] = (unsigned char)c;

            // 每接收 1024 bytes (1KB)，印出一個點點 '.'
            if (i % 1024 == 0)
            {
                uart_putc('.');
            }
        }

        uart_puts("[SUCCESS] Kernel successfully loaded into RAM!\n");

        // 跳轉到 kernel 入口
        void (*kernel_entry)(void) = (void (*)(void))KERNEL_LOAD_ADDR;
        kernel_entry();
    }
}

// 依據接收 a0, a1 暫存器的值(dtb資訊存放在 a1 中)
void start_kernel(unsigned long hartid, unsigned long dtb_pa)
{
    // 獲取 dtb 中 的 UART_BASE 和 UART_LSR_OFFSET 的值，並存入全域變數
    // int len;
    // int offset = fdt_path_offset(dtb_pa, "/soc/serial");
    // if (offset >= 0)
    // {
    //     // 這是 orangePi 的 UART
    //     uart_lsr_offset = 0x14;
    // }
    // else
    // {
    //     offset = fdt_path_offset(dtb_pa, "/soc/uart");
    //     if (offset >= 0)
    //     {
    //         // 這是 qemu 的 UART
    //         uart_lsr_offset = 0x5;
    //     }
    //     else
    //     {
    //         // 沒有找到 UART 的 node，無法進行後續的輸出
    //         while (1)
    //             ;
    //     }
    // }

    // // 找到 node，就可以從 node 中的 reg 屬性取得 UART 的 base address
    // const void *prop = fdt_getprop(dtb_pa, offset, "reg", &len);
    // const uint64_t *reg = (const uint64_t *)prop;
    // uart_base = bswap64(reg[0]); // UART base address 存入全域變數

    uart_puts("My kernel is running!\n");
    uart_puts("opi-rv2> ");

    char str_buf[1024];
    str_buf[0] = '\0';
    int index = 0;

    while (1)
    {
        char c = uart_getc();
        uart_putc(c);
        if (c == '\r' || c == '\n')
        {
            str_buf[index] = '\0';

            if (strcmp(str_buf, "hello") == 0)
            {
                uart_puts("Hello World.\n");
            }
            else if (strcmp(str_buf, "help") == 0)
            {
                uart_puts("Available commands:\r\n  help  - Show all commands\r\n  hello - Print Hello World\r\n  info  - Print system information\r\n");
            }
            else if (strcmp(str_buf, "info") == 0)
            {
                uart_puts("System Information:\n");
                uart_puts("  OpenSBI specification version: ");
                uart_hex(sbi_get_spec_version());
                uart_puts("\n");

                uart_puts("  Implementation ID: ");
                uart_hex(sbi_get_impl_id());
                uart_puts("\n");

                uart_puts("  Implementation version: ");
                uart_hex(sbi_get_impl_version());
                uart_puts("\n");
            }
            else if (strcmp(str_buf, "load") == 0)
            {
                do_load();
            }
            else if (strcmp(str_buf, "") == 0)
            {
                uart_puts("\r");
            }
            else
            {
                const char *unknown = "Unknown command.\n";
                while (*unknown)
                {
                    uart_putc(*unknown++);
                }
                uart_puts("Use \"help\" to get commands.\n");
            }

            uart_puts("opi-rv2> ");
            index = 0; // reset the index for the next input
        }
        else
        {
            if (c == '\b' || c == 0x7f)
            {
                if (index > 0)
                    index--;
            }
            else if (index < (int)sizeof(str_buf) - 1)
            {
                str_buf[index++] = c;
            }
        }
    }
}
