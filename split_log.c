/**
 * split_log.c
 * 哈夫曼编码：日志切分
 * @author lql
 * @date 2026-05-31
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split_log.h"


static long get_file_size(const char *filename)
{
    FILE *src = fopen(filename, "rb");
    if (!src)
    {
        return EXIT_FAILURE;
    }
    fseek(src, 0, SEEK_END);

    long size = ftell(src);

    fclose(src);
    return size;
}

// 按压缩操作批次切分
void split_log_file(void)
{
    const char *src_filename = "coding_log.txt";
    long file_size = get_file_size(src_filename);

    if (file_size < 0)
    {
        printf("\n【提示】当前目录下未检测到历史日志文件 (coding_log.txt)，无需切分。\n");
        return;
    }
    if (file_size == 0)
    {
        printf("\n【提示】历史日志文件大小为 0 字节，无需切分。\n");
        return;
    }

    FILE *fin = fopen(src_filename, "r");
    if (!fin)
    {
        printf("【系统错误】日志文件打开失败！\n");
        return;
    }

    char buffer[512];           // 缓冲区
    char out_filenames[256];    // 文件名数组
    FILE *fout = NULL;          // 文件名指针，用于判断是否在写
    int batch_count = 0; 

    // 逐行扫描
    while (fgets(buffer, sizeof(buffer), fin))
    {
        // 扫描到日志标志
        if (strstr(buffer, "--- 编码日志记录 ---"))
        {
            if (fout != NULL)
            {
                fclose(fout);
                fout = NULL;
            }

            snprintf(out_filenames, sizeof(out_filenames), "%s.data_%d", src_filename, batch_count);
            fout = fopen(out_filenames, "wb");
            if (!fout)
            {
                printf("【错误】无法创建切分文件: %s\n", out_filenames);
                fclose(fin);
                return;
            }

            batch_count++;
            printf(" 成功生成切分文件 -> %s\n", out_filenames);
        }

        // 如果当前有打开的批次文件，就把日志内容恢复
        if (fout != NULL)
        {
            fprintf(fout, "%s", buffer);
        }
    }
    if (fout != NULL)
    {
        fclose(fout);
    }
    fclose(fin);

    printf("===================================================\n");
    if (batch_count > 0)
    {
        printf("【切分成功】总计从日志中成功提取出 %d 次压缩操作。\n", batch_count);
    }
    else
    {
        printf("【提示】日志内未发现标准的“--- 编码日志记录 ---”标记，未执行切分。\n");
    }
}