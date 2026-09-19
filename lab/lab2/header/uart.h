#ifndef UART_H
#define UART_H

#define LSR_DR (1 << 0)   // Data Ready Flag - 位於LSR內的第 0 個 bit，接收緩衝區有資料可讀
#define LSR_TDRQ (1 << 5) // Transmitter Data Request - 位於LSR內的第 5 個 bit，發送緩衝區空，可以寫入新資料

// extern：宣告全域變數，讓其他檔案知道有這些變數存在
extern unsigned long uart_base;
extern unsigned long uart_lsr_offset;

#if defined(USE_QEMU)
#define KERNEL_LOAD_ADDR 0x82000000UL
#define UART_RBR (unsigned char *)(uart_base + 0x0)
#define UART_THR (unsigned char *)(uart_base + 0x0)
#define UART_LSR (unsigned char *)(uart_base + uart_lsr_offset)
#else
#define KERNEL_LOAD_ADDR 0x20000000UL
#define UART_RBR (unsigned char *)(uart_base + 0x0)             // Receiver Holding Register (讀取接收到的字元)
#define UART_THR (unsigned char *)(uart_base + 0x0)             // Transmitter Holding Register (寫入要傳送的字元)
#define UART_LSR (unsigned char *)(uart_base + uart_lsr_offset) // Line Status Register (檢查是否準備好讀/寫,以及其他狀態)
#endif                                                          // USE_QEMU

char uart_getc();
void uart_putc(char c);
void uart_puts(const char *s);
void uart_hex(unsigned long h);
void uart_flush_rx();
void uart_put_uint(unsigned int num);

#endif // UART_H
