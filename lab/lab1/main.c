extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);

#define SBI_EXT_BASE      0x10

enum sbi_ext_base_fid {
    SBI_EXT_BASE_GET_SPEC_VERSION,
    SBI_EXT_BASE_GET_IMP_ID,
    SBI_EXT_BASE_GET_IMP_VERSION,
    SBI_EXT_BASE_PROBE_EXT,
    SBI_EXT_BASE_GET_MVENDORID,
    SBI_EXT_BASE_GET_MARCHID,
    SBI_EXT_BASE_GET_MIMPID,
};

struct sbiret {
    long error;
    long value;
};

struct sbiret sbi_ecall(int ext,
                        int fid,
                        unsigned long arg0,
                        unsigned long arg1,
                        unsigned long arg2,
                        unsigned long arg3,
                        unsigned long arg4,
                        unsigned long arg5) {
    struct sbiret ret;
    register unsigned long a0 asm("a0") = (unsigned long)arg0;
    register unsigned long a1 asm("a1") = (unsigned long)arg1;
    register unsigned long a2 asm("a2") = (unsigned long)arg2;
    register unsigned long a3 asm("a3") = (unsigned long)arg3;
    register unsigned long a4 asm("a4") = (unsigned long)arg4;
    register unsigned long a5 asm("a5") = (unsigned long)arg5;
    register unsigned long a6 asm("a6") = (unsigned long)fid;
    register unsigned long a7 asm("a7") = (unsigned long)ext;
    asm volatile("ecall"
                 : "+r"(a0), "+r"(a1)
                 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                 : "memory");
    ret.error = a0;
    ret.value = a1;
    return ret;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/**
 * sbi_get_spec_version() - Get the SBI specification version.
 *
 * Return: The current SBI specification version.
 * The minor number of the SBI specification is encoded in the low 24 bits,
 * with the major number encoded in the next 7 bits. Bit 31 must be 0.
 */
long sbi_get_spec_version(void) {
    // TODO: Implement this function
    return sbi_ecall(SBI_EXT_BASE, 0x0, 0, 0, 0, 0, 0, 0).value;
}

long sbi_get_impl_id(void) {
    // TODO: Implement this function
    return sbi_ecall(SBI_EXT_BASE, 0x1, 0, 0, 0, 0, 0, 0).value;
}

long sbi_get_impl_version(void) {
    // TODO: Implement this function
    return sbi_ecall(SBI_EXT_BASE, 0x2, 0, 0, 0, 0, 0, 0).value;
}

/**
 * sbi_probe_extension() - Check if an SBI extension ID is supported or not.
 * @extid: The extension ID to be probed.
 *
 * Return: 1 or an extension specific nonzero value if yes, 0 otherwise.
 */
long sbi_probe_extension(int extid) {
    // TODO: Implement this function
    return sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, extid, 0, 0, 0, 0, 0).value;
    
}

void start_kernel() {
    uart_puts("My kernel is running!\n");
    uart_puts("opi-rv2> ");

    char str_buf[1024];
    str_buf[0] = '\0';
    int index = 0;

    while (1) {
        char c = uart_getc();
        uart_putc(c);
        if (c == '\r' || c == '\n') {
            str_buf[index] = '\0';

            if(strcmp(str_buf, "hello\0") == 0){
                uart_puts("Hello World.\n");
            }else if(strcmp(str_buf, "help\0") == 0){
                uart_puts("Available commands:\r\n  help  - Show all commands\r\n  hello - Print Hello World\r\n  info  - Print system information\r\n");
            }else if(strcmp(str_buf, "info\0") == 0){
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
            }else if (strcmp(str_buf, "\0") == 0)
            {
                uart_puts("\r");
            }else{
                char* unknown = "Unknown command.\n";
                while (*unknown) {
                    uart_putc(*unknown++);
                }
                uart_puts("Use \"help\" to get commands.\n");
            }
            
            uart_puts("opi-rv2> ");
            index = 0; // reset the index for the next input
        } else {
            str_buf[index] = c; // append the character to the string buffer
            index++;
        }
    }
}
