#define UART_BASE 0xD4017000
#define UART_RBR  (unsigned char*)(UART_BASE + 0x0)
#define UART_THR  (unsigned char*)(UART_BASE + 0x0)
#define UART_LSR  (unsigned char*)(UART_BASE + 0x14)
#define LSR_DR    (1 << 0)
#define LSR_TDRQ  (1 << 5)

char uart_getc() {
    while ((*UART_LSR & LSR_DR) == 0);
    char c = (char)*UART_RBR;
    return c == '\r' ? '\n' : c;
}

void uart_putc(char c) {
     // 處理 Backspace (0x08) 或 Delete (0x7f)
    // 處理順序：先發送退格符號 '\b'，然後發送空格 ' ' 以覆蓋原有字符，最後再發送退格符號 '\b' 重新定位
    if (c == '\b' || c == 0x7f) {
        // 1. 發送第一個 \b
        while ((*UART_LSR & LSR_TDRQ) == 0);
        *UART_THR = '\b';

        // 2. 發送空格 ' '
        while ((*UART_LSR & LSR_TDRQ) == 0);
        *UART_THR = ' ';
        
        // 3. 再次發送 \b
        while ((*UART_LSR & LSR_TDRQ) == 0);
        *UART_THR = '\b';
        
        return; // 處理完畢，直接返回
    }
    while ((*UART_LSR & LSR_TDRQ) == 0);
    *UART_THR = c;
    if (c == '\n'){
        while ((*UART_LSR & LSR_TDRQ) == 0);
        uart_putc('\r');
    }
}

void uart_puts(const char* s) {
    while (*s)
        uart_putc(*s++);
}


void uart_hex(unsigned long h) {
    uart_puts("0x");
    unsigned long n;
    for (int c = 60; c >= 0; c -= 4) {
        n = (h >> c) & 0xf;
        n += n > 9 ? 0x57 : '0';
        uart_putc(n);
    }
}
