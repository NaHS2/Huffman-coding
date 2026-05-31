/**
 * main.c
 * 哈夫曼编码：文本压缩
 * @author lql
 * @date 2026-05-08
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Huffman.h"
#include "write_log.h"
#include "read_file.h"
#include "read_log.h"

databuffer buf = {NULL, NULL, {0}, {0}, 0, {0}, NULL, NULL};

// 函数声明
void main_compress(void);
void main_decompress(void);

int compress_flag = 0;

int main()
{
    // 动态分配缓冲空间
    buf.input_text = malloc(1024);
    buf.compressed_stream = malloc(8192);
    buf.decompressed_text = malloc(1024);
    int choice;
    do
    {
        printf("\n======= 哈夫曼编码系统 =======\n");
        printf("1. 压缩手动输入文本 \n");
        printf("2. 解压文本 \n");
        printf("3. 读取外部文件并编码 \n");
        printf("4. 查看历史编码日志 \n");
        printf("5. 一键清空历史日志 \n");
        printf("6. 退出程序\n");
        printf("==============================\n");
        printf("请选择操作 (1-6): ");

        // 判断是否输入合法
        if (scanf("%d", &choice) != 1)
        {
            printf("输入非法！\n");
            while (getchar() != '\n')
                ; // 清空输入缓冲区残余的垃圾字符
            continue;
        }
        while (getchar() != '\n')
            ; // 清空敲数字时留下的回车符

        switch (choice)
        {
        case 1:
            // 每次手动接收前，清空缓冲区，防止老数据残留
            memset(buf.input_text, 0, 1024);
            printf("\n请输入你要压缩的文本: ");
            fgets(buf.input_text, 1024, stdin);
            buf.input_text[strcspn(buf.input_text, "\n")] = '\0';

            main_compress();
            break;

        case 2:
            main_decompress();
            break;

        case 3:
            encode_external_file();
            break;
        case 4:
            read_coding_log();
            break;
        case 5:
            clear_coding_log();
            break;

        case 6:
            // 释放内存
            free(buf.input_text);
            free(buf.compressed_stream);
            free(buf.decompressed_text);
            buf.input_text = NULL;
            buf.compressed_stream = NULL;
            buf.decompressed_text = NULL;
            printf("程序已安全退出。\n");
            break;

        default:
            printf("无效选项，请重新输入。\n");
            break;
        }

    } while (choice != 6);

    return EXIT_SUCCESS;
}

// 压缩
void main_compress(void)
{
    int i;

    // 初始化统计变量
    buf.size = 0;
    memset(buf.freq, 0, sizeof(buf.freq));

    // 统计字频
    for (i = 0; buf.input_text[i] != '\0'; i++)
    {
        int ascii_index = (unsigned char)buf.input_text[i];
        if (buf.freq[ascii_index] == 0)
        {
            buf.data[buf.size++] = buf.input_text[i];
        }
        buf.freq[ascii_index]++;
    }

    // 做成紧凑字频数组
    for (i = 0; i < buf.size; i++)
    {
        buf.final_freq[i] = buf.freq[(unsigned char)buf.data[i]];
    }

    // 一键生成密码本
    generatecodes(buf.data, buf.final_freq, buf.size);

    compress_flag = 1;

    // 打印密码本到屏幕
    printf("\n【哈夫曼密码本】:\n");
    for (i = 0; i < buf.size; i++)
    {
        int ascii_index = (unsigned char)buf.data[i];
        printf("字符 '%c' 的二进制编码为: %s\n", buf.data[i], huffmancodes[ascii_index]);
    }

    // 执行转换，将文字打成 0/1 压缩流
    compresstext(buf.input_text, buf.compressed_stream);
    printf("\n【压缩成功！】\n");

    if (strlen(buf.compressed_stream) < 100)
        printf("生成的二进制流为: %s\n", buf.compressed_stream);

    // 追加日志
    write_coding_log();
}

void main_decompress(void)
{
    // ====== 解压功能 ======
    if (compress_flag == 0)
    {
        printf("\n【错误】当前缓冲区内没有压缩好的二进制流，请先选择 1 进行压缩！\n");
        return;
    }

    printf("\n当前待解压的二进制流为: %s\n", buf.compressed_stream);

    // 重新构建哈夫曼树拿到根节点
    buf.root = buildhuffmantree(buf.data, buf.final_freq, buf.size);

    // 顺着树还原文本
    decompress(buf.root, buf.compressed_stream, buf.decompressed_text);
    printf("【解压成功！】\n");
    printf("还原后的原始文本为: %s\n", buf.decompressed_text);
}