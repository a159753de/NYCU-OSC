#ifndef UART_H
#define UART_H

#if defined(USE_QEMU)
#define KERNEL_LOAD_ADDR 0x82000000UL
#define UART_BASE 0x10000000UL
#define UART_RBR (unsigned char *)(UART_BASE + 0x0)
#define UART_THR (unsigned char *)(UART_BASE + 0x0)
#define UART_LSR (unsigned char *)(UART_BASE + 0x5)
#define LSR_DR (1 << 0)   // LSR內的 第 0 個 bit
#define LSR_TDRQ (1 << 5) // LSR內的 第 5 個 bit
#else
#define KERNEL_LOAD_ADDR 0x20000000UL
#define UART_BASE 0xD4017000
#define UART_RBR (unsigned char *)(UART_BASE + 0x0)  // Receiver Holding Register (讀取接收到的字元)
#define UART_THR (unsigned char *)(UART_BASE + 0x0)  // Transmitter Holding Register (寫入要傳送的字元)
#define UART_LSR (unsigned char *)(UART_BASE + 0x14) // Line Status Register (檢查是否準備好讀/寫,以及其他狀態)
#define LSR_DR (1 << 0)                              // Data Ready Flag - 位於LSR內，接收緩衝區有資料可讀
#define LSR_TDRQ (1 << 5)                            // Transmitter Data Request - 位於LSR內，發送緩衝區空，可以寫入新資料
#endif                                               // USE_QEMU

char uart_getc();
void uart_putc(char c);
void uart_puts(const char *s);
void uart_hex(unsigned long h);
void uart_flush_rx();
void uart_put_uint(unsigned int num);

#endif // UART_H
