#include "header/uart.h"

char uart_getc()
{
    // TODO: Implement this function
    while (!(*UART_LSR & LSR_DR))
        ; // the 2 value is doing bitwise AND, if the result is 0, it means the data is not ready, so we keep waiting. If it's not 0, it means data is ready, and we can read it.
    return *UART_RBR;
}

void uart_putc(char c)
{
    // TODO: Implement this function
    while (!(*UART_LSR & LSR_TDRQ))
        ;
    *UART_THR = c;
}

void uart_puts(const char *s)
{
    // TODO: Implement this function
    while (*s)
    {
        // 如果遇到 \n，自動補一個 \r
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

void start_kernel()
{
    uart_puts("\nMy kernel is running!\n");
    uart_puts("opi-rv2> ");
    while (1)
    {
        // uart_putc(uart_getc());
        char c = uart_getc(); // 等待輸入

        // 把收到的字印回去，讓使用者看到自己打了什麼
        if (c == '\r' || c == '\n')
        {
            uart_puts("\nopi-rv2> ");
        }
        else
        {
            uart_putc(c);
        }
    }
}
