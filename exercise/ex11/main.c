#define UART_BASE 0x10000000UL
#define UART_RBR  (unsigned char*)(UART_BASE + 0x0) // Receiver Holding Register (讀取接收到的字元)
#define UART_THR  (unsigned char*)(UART_BASE + 0x0) // Transmitter Holding Register (寫入要傳送的字元)
#define UART_LSR  (unsigned char*)(UART_BASE + 0x5) // Line Status Register (檢查是否準備好讀/寫,以及其他狀態)
#define LSR_DR    (1 << 0) // Data Ready - 接收緩衝區有資料可讀
#define LSR_TDRQ  (1 << 5) // Transmitter Data Request - 發送緩衝區空，可以寫入新資料

char uart_getc() {
    // TODO: Implement this function
    while (!(*UART_LSR & LSR_DR)); //the 2 value is doing bitwise AND, if the result is 0, it means the data is not ready, so we keep waiting. If it's not 0, it means data is ready, and we can read it.
    return *UART_RBR;
}

void uart_putc(char c) {
    // TODO: Implement this function
    while (!(*UART_LSR & LSR_TDRQ));
    *UART_THR = c;
    if (c == '\r'){
        uart_putc('\n');
        uart_puts("opi-rv2> ");
    }
}

void uart_puts(const char* s) {
    // TODO: Implement this function
    while (*s) {
        uart_putc(*s++);
    } 
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    uart_puts("opi-rv2> ");
    while (1) {
        uart_putc(uart_getc());
    }
}
