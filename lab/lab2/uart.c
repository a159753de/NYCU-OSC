#include "header/uart.h"

// 定義全域變數（給予預設值 0）
unsigned long uart_base = 0;
unsigned long uart_lsr_offset = 0;

char uart_getc()
{
    while ((*UART_LSR & LSR_DR) == 0)
        ;
    char c = (char)*UART_RBR;
    return c == '\r' ? '\n' : c;
}

void uart_putc(char c)
{
    // 處理 Backspace (0x08) 或 Delete (0x7f)
    // 處理順序：先發送退格符號 '\b'，然後發送空格 ' ' 以覆蓋原有字符，最後再發送退格符號 '\b' 重新定位
    if (c == '\b' || c == 0x7f)
    {
        // 1. 發送第一個 \b
        while ((*UART_LSR & LSR_TDRQ) == 0)
            ;
        *UART_THR = '\b';

        // 2. 發送空格 ' '
        while ((*UART_LSR & LSR_TDRQ) == 0)
            ;
        *UART_THR = ' ';

        // 3. 再次發送 \b
        while ((*UART_LSR & LSR_TDRQ) == 0)
            ;
        *UART_THR = '\b';

        return; // 處理完畢，直接返回
    }
    while ((*UART_LSR & LSR_TDRQ) == 0)
        ;
    *UART_THR = c;
    if (c == '\n')
    {
        while ((*UART_LSR & LSR_TDRQ) == 0)
            ;
        uart_putc('\r');
    }
}

void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

void uart_hex(unsigned long h)
{
    uart_puts("0x");
    unsigned long n;
    for (int c = 60; c >= 0; c -= 4)
    {
        n = (h >> c) & 0xf;
        n += n > 9 ? 0x57 : '0';
        uart_putc(n);
    }
}

// 排空垃圾資料
void uart_flush_rx()
{
    unsigned int timeout_counter = 0;
    const unsigned int TIMEOUT_LIMIT = 0x100000; // 根據你的 CPU 時脈調整這個等待上限

    uart_puts("\n[INFO] Flushing garbage data from line... please wait.\n");

    // 持續檢查 buffer 內是否有資料，直到完全排空
    while (1)
    {
        // check 接收 buffer 是否有資料 (燈號為 1)
        if ((*UART_LSR & LSR_DR) != 0)
        {
            // 有資料進來！把資料讀出來，但不存進任何變數 (直接丟進垃圾桶)
            volatile char trash = *UART_RBR;
            (void)trash; // 告訴編譯器我們知道這個變數沒被用到，避免警告

            // 既然還有資料在傳，就把計時器歸零，繼續等
            timeout_counter = 0;
        }
        else
        {
            // 沒資料了，計時器開始增加
            timeout_counter++;

            // 如果空轉了很久 (超過設定的上限)，代表 Python 腳本終於全部傳完了
            if (timeout_counter > TIMEOUT_LIMIT)
            {
                uart_puts("[INFO] Line is clear. Returning to safety.\n");
                break; // 跳出迴圈，安全離開
            }
        }
    }
}

// 把 unsign int 轉成 10進位
void uart_put_uint(unsigned int num)
{
    // 32-bit 無號整數最大值為 4294967295 (10位數)，加上預留空間
    char buf[16];
    int i = 0;

    // 處理特例：數字為 0
    if (num == 0)
    {
        buf[i++] = '0';
    }
    else
    {
        // 提取每一位數字 (從個位數開始)
        while (num > 0)
        {
            buf[i++] = (num % 10) + '0'; // 取餘數並轉成 ASCII 字元
            num /= 10;
        }
    }

    buf[i] = '\0'; // 字串結尾

    // 反轉字串(因為剛剛是從個位數開始存入)
    int start = 0;
    int end = i - 1;
    while (start < end)
    {
        char temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }

    uart_puts(buf);
}