/**
 * read_file.c
 * 哈夫曼编码：动态扩容读取外部文本文件并自动编码
 * @author lql
 * @date 2026-05-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Huffman.h"
#include "read_file.h"

void main_compress(void);

void encode_external_file(void)
{
    char file_name[100];
    FILE *fp;
    int ch;

    // 动态扩容所需的两个核心控制变量
    int current_capacity = 1024; // 初始假设文件在 1KB 以内
    int index = 0;

    printf("\n请输入你要读取的文本文件名: ");
    // 输入
    fgets(file_name, sizeof(file_name), stdin);

    // 扫描字符串并统计数量
    file_name[strcspn(file_name, "\n")] = '\0';

    fp = fopen(file_name, "r");
    if (!fp)
    {
        printf("【错误】无法打开文件 '%s'！\n", file_name);
        return;
    }

    // 先为本次读取重置并预分配 1KB 的初始空间
    char *temp_input = realloc(buf.input_text, current_capacity);
    if (!temp_input)
    {
        printf("【系统错误】内存分配失败！\n");
        fclose(fp);
        return;
    }
    buf.input_text = temp_input;

    // 完整的读完指定文件的内容，顶满就自动翻倍扩容
    while ((ch = fgetc(fp)) != EOF)
    {
        // 如果当前装满了，立刻进行“成倍扩容”
        if (index >= current_capacity - 1)
        {
            current_capacity *= 2; // 容量翻倍

            // 使用 realloc 安全扩大内存块
            char *new_space = realloc(buf.input_text, current_capacity);
            if (!new_space)
            {
                printf("【错误】文件过大，内存不足，读取中断！\n");
                break;
            }
            buf.input_text = new_space; 
        }

        // 安全把字符塞进去
        buf.input_text[index] = (char)ch;
        index++;
    }
    buf.input_text[index] = '\0'; 

    fclose(fp); // 读完果断关闭外部文件

    if (index == 0)
    {
        printf("文件 '%s' 内容为空，未执行压缩。\n", file_name);
        return;
    }

    printf("\n【文件读取成功！】共完整读取了 %d 个字符。\n", index);

    // 扩容压缩缓冲区
    char *temp_compressed = realloc(buf.compressed_stream, index * 8 + 1024);
    if (!temp_compressed)
    {
        printf("【系统错误】同步扩容压缩缓冲区失败！\n");
        return;
    }
    buf.compressed_stream = temp_compressed;

    // 2. 扩容解压缓冲区
    char *temp_decompressed = realloc(buf.decompressed_text, index + 1024);
    if (!temp_decompressed)
    {
        printf("【系统错误】同步扩容解压缓冲区失败！\n");
        return;
    }
    buf.decompressed_text = temp_decompressed;

    main_compress();
}