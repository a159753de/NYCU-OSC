#include <stdint.h>
#include <stddef.h>

#ifndef FDT_H
#define FDT_H

#define FDT_MAGIC 0xd00dfeed
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

struct fdt_header
{
    uint32_t magic; // 代表整個device tree blob的開始，值為 0xd00dfeed
    uint32_t totalsize;
    uint32_t off_dt_struct;  // fdt Header 的起始位址加上此 offset，就是 Struct Block 的起始位址
    uint32_t off_dt_strings; // fdt Header 的起始位址加上此 offset，就是 Strings Block 的起始位址
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

static inline const void *align_up(const void *ptr, size_t align)
{
    return (const void *)(((uintptr_t)ptr + align - 1) & ~(align - 1));
}

int fdt_path_offset(const void *fdt, const char *path);

const void *fdt_getprop(const void *fdt,
                        int nodeoffset,
                        const char *name,
                        int *lenp);

#endif /* FDT_H */