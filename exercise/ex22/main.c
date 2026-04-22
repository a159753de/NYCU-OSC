#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cpio_t
{
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
};

/**
 * @brief Convert a hexadecimal string to integer
 *
 * @param s hexadecimal string
 * @param n length of the string
 * @return integer value
 */
static int hextoi(const char *s, int n)
{
    int r = 0;
    while (n-- > 0)
    {
        r = r << 4;
        if (*s >= 'A')
            r += *s++ - 'A' + 10;
        else if (*s >= 0)
            r += *s++ - '0';
    }
    return r;
}

/**
 * @brief Align a number to the nearest multiple of a given number
 *
 * @param n number
 * @param byte alignment
 * @return aligned number
 */
static int align(int n, int byte)
{
    return (n + byte - 1) & ~(byte - 1);
}

void initrd_list(const void *rd)
{
    // TODO: Implement this function
    const char *p = (const char *)rd; // 建立指針 p 指向 initramfs.cpio 的起始位址

    while (1)
    {
        // 若 p 所指向的資料不是 "070701"，表示目前不是在 cpio 檔案的開頭
        if (memcmp(p, "070701", 6) != 0)
        {
            fprintf(stderr, "Invalid cpio archive\n");
            return;
        }

        struct cpio_t *cur_header = (struct cpio_t *)p; // 建立指向當前檔案的 cur_header 指針
        const char *filename = p + 110;                 // header 共 110 bytes，跳過 header 後，接下來就是檔案名稱

        int namesize = hextoi(cur_header->namesize, 8); // 取得檔案名稱的長度
        int filesize = hextoi(cur_header->filesize, 8); // 取得檔案內容的長度

        if (strcmp(filename, "TRAILER!!!") == 0) // 若檔案名稱為 "TRAILER!!!"，表示已經到達 cpio 檔案的結尾
        {
            break;
        }
        printf("Filesize: %d bytes, ", filesize); // 印出檔案內容的長度
        printf("Filename: %s\n", filename);       // 印出檔案名稱

        // 跳過 header 、檔名長度和檔案內容長度，並對齊
        int offset_to_data = align(110 + namesize, 4);
        int offset_to_next_header = align(offset_to_data + filesize, 4);
        p += offset_to_next_header; // 移動指針 p 到下一個檔案的 header 開始位置
    }
}

void initrd_cat(const void *rd, const char *filename)
{
    // TODO: Implement this functionx
    const char *p = (const char *)rd;

    while (1)
    {
        // 若 p 所指向的資料不是 "070701"，表示目前不是在 cpio 檔案的開頭
        if (memcmp(p, "070701", 6) != 0)
        {
            fprintf(stderr, "Invalid cpio archive\n");
            return;
        }

        struct cpio_t *cur_header = (struct cpio_t *)p; // 建立指向當前檔案的 cur_header 指針
        const char *filename_ptr = p + 110;             // header 共 110 bytes，跳過 header 後，接下來就是檔案名稱

        int namesize = hextoi(cur_header->namesize, 8); // 取得檔案名稱的長度
        int filesize = hextoi(cur_header->filesize, 8); // 取得檔案內容的長度

        if (strcmp(filename_ptr, "TRAILER!!!") == 0) // 若檔案名稱為 "TRAILER!!!"，表示已經到達 cpio 檔案的結尾
        {
            break;
        }

        if (strcmp(filename_ptr, filename) == 0) // 若檔案名稱與參數 filename 相同，表示找到了要顯示內容的檔案
        {
            const char *file_content = p + align(110 + namesize, 4); // 跳過 header 和檔名，並對齊後，就是檔案內容的起始位址
            printf("Filename: %s\n", filename_ptr);                         // 印出檔案名稱
            printf("%.*s", filesize, file_content);                  // 印出檔案內容，使用 %.*s 可以指定要印出的字串長度
            return; // 找到檔案後就可以跳出迴圈了
        }

        // 跳過 header 、檔名長度和檔案內容長度，並對齊
        int offset_to_data = align(110 + namesize, 4);
        int offset_to_next_header = align(offset_to_data + filesize, 4);
        p += offset_to_next_header; // 移動指針 p 到下一個檔案的 header 開始位置
    }
    printf("File: \"%s\" not found in the cpio archive\n", filename); // 若找不到檔案，印出錯誤訊息
}

int main()
{
    /* Prepare the initial RAM disk */
    FILE *fp = fopen("initramfs.cpio", "rb");
    if (!fp)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    void *rd = malloc(sz);
    fseek(fp, 0, SEEK_SET);
    if (fread(rd, 1, sz, fp) != sz)
    {
        fprintf(stderr, "Failed to read the device tree blob\n");
        free(rd);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    initrd_list(rd);
    printf("=============================\n");
    initrd_cat(rd, "osc.txt");
    initrd_cat(rd, "test.txt");

    free(rd);
    return 0;
}
