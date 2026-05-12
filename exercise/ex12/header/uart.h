#ifndef UART_H
#define UART_H

#if defined(USE_QEMU)
    #define UART_BASE 0x10000000UL
    #define UART_RBR (unsigned char *)(UART_BASE + 0x0)
    #define UART_THR (unsigned char *)(UART_BASE + 0x0)
    #define UART_LSR (unsigned char *)(UART_BASE + 0x5)
    #define LSR_DR (1 << 0)
    #define LSR_TDRQ (1 << 5)
#else
    #define UART_BASE 0xD4017000
    #define UART_RBR (unsigned char *)(UART_BASE + 0x0)  // Receiver Holding Register (讀取接收到的字元)
    #define UART_THR (unsigned char *)(UART_BASE + 0x0)  // Transmitter Holding Register (寫入要傳送的字元)
    #define UART_LSR (unsigned char *)(UART_BASE + 0x14) // Line Status Register (檢查是否準備好讀/寫,以及其他狀態)
    #define LSR_DR (1 << 0)                              // Data Ready - 接收緩衝區有資料可讀
    #define LSR_TDRQ (1 << 5)                            // Transmitter Data Request - 發送緩衝區空，可以寫入新資料
#endif // USE_QEMU


#endif // UART_H
