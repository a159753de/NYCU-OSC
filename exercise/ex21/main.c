#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static inline uint32_t bswap32(uint32_t x)
{
    return __builtin_bswap32(x);
}

static inline uint64_t bswap64(uint64_t x)
{
    return __builtin_bswap64(x);
}

static inline const void *align_up(const void *ptr, size_t align)
{
    return (const void *)(((uintptr_t)ptr + align - 1) & ~(align - 1));
}

int fdt_path_offset(const void *fdt, const char *path)
{
    printf("Searching for node: %s\n", path);
    // TODO: Implement this function
    const struct fdt_header *header = (const struct fdt_header *)fdt;                     // 將傳進來的 *fdt 轉成 struct fdt_header* 的指標
    uint32_t off_dt_struct = bswap32(header->off_dt_struct);                              // 取得 off_dt_struct 的值，並由big-endian轉成little-endian
    const uint32_t *struct_block = (const uint32_t *)((const char *)fdt + off_dt_struct); // 將 void * fdt 轉成 char *，這樣後續做地址運算都會以 1 byte 為單位，然後加上 off_dt_struct 的值，最後再轉成 uint32_t* 的指標，接下來的運算都是以 4 bytes 為單位
    if (*path != '/')
        return -1; // 如果 path 的第一個字元不是 '/'，就回傳 -1(因為 path 必須以 '/' 開頭)
    if (strcmp(path, "/") == 0)
        return off_dt_struct; // 如果 path 是 "/"，就回傳 off_dt_struct 的值(因為 "/" 代表根節點，而根節點的 offset 就是 off_dt_struct 的值)

    const uint32_t *p = struct_block;             // 建立指標 p 來遍歷 struct block，初始值為 struct_block 的起始位址
    char *path_copy = strdup(path);               // 複製 path 字串，因為strtok 會修改原本的字串，但原本傳進來的path是唯讀不可修改
    char *token = strtok((char *)path_copy, "/"); // 使用 strtok 函數來分割 path_copy，分隔符是 '/'，第一次呼叫會回傳 path 中第一個 token 的位址(不是根目錄)
    if (token)
        // 進行loop搜尋
        while (1)
        {
            uint32_t tag = bswap32(*p); // 取得當前節點的 tag，並由 big-endian 轉成 little-endian

            if (tag == FDT_BEGIN_NODE)
            {
                // 如果 tag 是 FDT_BEGIN_NODE，表示開始一個新的節點
                const char *name = (const char *)(p + 1);                                   // 節點名稱緊跟在 tag 後面，所以 name 的位址是 p + 1
                int nameLen = strlen(name);                                                 // 取得 node 名稱的長度
                const uint32_t *next_p = (const uint32_t *)align_up(name + nameLen + 1, 4); // 將 name + nameLen 的位址對齊到 4 bytes，+1 是因為字元結尾是 '\0'，所以要加上 1 個 byte 的空間來存放

                // 如果node 名稱與 token相同，表示找到了對應的節點
                int tokenLen = strlen(token); // 取得 token 的長度
                if (token != NULL && strncmp(name, token, tokenLen) == 0)
                {
                    // 檢查配對是否完整：name[tokenLen] 必須是 '\0'或'@'(因為有些 node 名稱後面會加上 '@地址')
                    if (name[tokenLen] == '\0' || name[tokenLen] == '@')
                    {
                        token = strtok(NULL, "/"); // 繼續分割 path，取得下一個 token
                        if (token == NULL)
                        {
                            // 如果 token 為 NULL，代表已找到最後的node
                            free(path_copy);                            // 釋放複製的 path 字串
                            return (const char *)p - (const char *)fdt; // 用當前的位址 - fdt 的位址，就是此node 相對於 fdt 起始位址的 offset
                        }
                    }

                    // token 不為 NULL，代表還有下一個 node 要找
                    p = next_p; // 將 p 移動到下一個節點的起始位址，繼續往下搜尋
                }
                else
                {
                    // node 名稱與token不同，繼續往下搜尋
                    p = next_p; // 將 p 移動到下一個節點的起始位址
                }
            }
            else if (tag == FDT_END_NODE || tag == FDT_NOP)
            {
                // 如果 tag 是 FDT_END_NODE 或 FDT_NOP，表示結束一個節點或是無操作，繼續往下搜尋
                p++;
            }
            else if (tag == FDT_PROP)
            {
                // 如果 tag 是 FDT_PROP，表示這裡是屬性區塊
                p++;                                                          // 跳過 tag
                uint32_t propLen = bswap32(*p);                               // 取得屬性的長度，代表這個區塊的總bytes數
                p++;                                                          // 跳過存放屬性長度的地址
                p++;                                                          // 跳過存放屬性名稱的地址
                p = (const uint32_t *)align_up((const char *)p + propLen, 4); // 跳過存放屬性區塊的地址，然後將 p 對齊到 4 bytes 的邊界
            }
            else if (tag == FDT_END)
            {
                break;
            }
            else
            {
                // 如果 tag 是其他值，表示格式錯誤，直接結束搜尋
                break;
            }
        }
    free(path_copy);
    return -1; // 如果搜尋結束後還是沒有找到對應的節點，回傳 -1
}

const void *fdt_getprop(const void *fdt,
                        int nodeoffset,
                        const char *name,
                        int *lenp)
{
    // TODO: Implement this function
    struct fdt_header *header = (struct fdt_header *)fdt;
    const uint32_t *p = (const uint32_t *)((const char *)fdt + nodeoffset); // p 代表該節點的起始位址，指向該 node 的 tag: FDT_BEGIN_NODE
    p++;                                                                    // 跳過 FDT_BEGIN_NODE 的 tag
    const char *nodeName = (const char *)(p + 1);                           // 節點名稱緊跟在 tag 後面，所以 name 的位址是 p + 1
    int nodeNameLen = strlen(nodeName);                                     // 取得 node 名稱的長度
    p = (const uint32_t *)align_up(nodeName + nodeNameLen + 1, 4);          // 跳過存放 node 名稱的地址們

    while (1)
    {
        uint32_t tag = bswap32(*p); // 取得當前節點的 tag，並由 big-endian 轉成 little-endian

        if (tag == FDT_END_NODE)
        {
            // 如果 tag 是 FDT_END_NODE，表示此節點已經結束，但還沒找到屬性，回傳 NULL
            return NULL;
        }
        else if (tag == FDT_PROP)
        {
            // 如果 tag 是 FDT_PROP，表示這裡是屬性區塊，開始檢查屬性名稱是否與傳入的 name 相同
            p++; // 移動到len 的地址
            uint32_t propLen = bswap32(*p);
            p++; // 移動到 nameoff 的地址
            uint32_t nameoff = bswap32(*p);

            p++; // 移動到屬性值的位址起點

            const char *propName = (const char *)fdt + bswap32(header->off_dt_strings) + nameoff; // 屬性名稱的位址 = fdt 起始位址 + off_dt_strings 的值 + nameoff 的值

            if (strcmp(propName, name) == 0)
            {
                // 如果屬性名稱與傳入的 name 相同，表示找到了對應的屬性，回傳此屬性區塊的位址，並將屬性長度存到 lenp 指向的地址
                if (lenp)
                    *lenp = propLen;

                return (const char *)p; // 回傳此屬性區塊的位址
            }

            // 如果屬性名稱與傳入的 name 不同，表示沒有找到對應的屬性，跳過此屬性區塊
            p = (const uint32_t *)align_up((const char *)p + propLen, 4); // 跳過此屬性區塊，並對齊到 4 bytes
        }
        else if (tag == FDT_NOP)
        {
            // 如果 tag 是 FDT_NOP，表示無操作，繼續往下搜尋
            p++;
        }
        else
        {
            // 如果 tag 是其他值，表示格式錯誤，直接結束搜尋
            return NULL;
        }
    }
}

int main()
{
    /* Prepare the device tree blob */
    FILE *fp = fopen("qemu.dtb", "rb");
    if (!fp)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    void *fdt = malloc(sz);
    fseek(fp, 0, SEEK_SET);
    if (fread(fdt, 1, sz, fp) != sz)
    {
        fprintf(stderr, "Failed to read the device tree blob\n");
        free(fdt);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    /* Find the node offset */
    int offset = fdt_path_offset(fdt, "/cpus/cpu@0/interrupt-controller");
    printf("Node offset: 0x%x\n", offset);
    if (offset < 0)
    {
        fprintf(stderr, "fdt_path_offset\n");
        free(fdt);
        return EXIT_FAILURE;
    }

    /* Get the node property */
    int len;
    const void *prop = fdt_getprop(fdt, offset, "compatible", &len);
    if (!prop)
    {
        fprintf(stderr, "fdt_getprop\n");
        free(fdt);
        return EXIT_FAILURE;
    }
    printf("compatible: %.*s\n", len, (const char *)prop);

    offset = fdt_path_offset(fdt, "/memory");
    printf("Node offset: 0x%x\n", offset);
    printf("xxxxx\n");
    prop = fdt_getprop(fdt, offset, "reg", &len);
    const uint64_t *reg = (const uint64_t *)prop;
    printf("memory: base=0x%lx size=0x%lx\n", bswap64(reg[0]), bswap64(reg[1]));

    offset = fdt_path_offset(fdt, "/chosen");
    prop = fdt_getprop(fdt, offset, "linux,initrd-start", &len);
    const uint64_t *initrd_start = (const uint64_t *)prop;
    printf("initrd-start: 0x%lx\n", bswap64(initrd_start[0]));

    free(fdt);
    return 0;
}
