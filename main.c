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

// 所有核心数据缓冲区
typedef struct
{
    huffmannode *root;            // 哈夫曼树根节点指针
    char input_text[256];         // 用户输入的原始文本
    char data[MAX_CHAR];          // 字符集
    int freq[MAX_CHAR];           // 动态字频统计表
    int size;                     // 实际字符去重后的数量
    int final_freq[MAX_CHAR];     // 浓缩后的字频数组
    char compressed_stream[2048]; // 压缩后的 0/1 字符串流
    char decompressed_text[256];  // 解压还原后的文本
} databuffer;

databuffer buf = {NULL, {0}, {0}, {0}, 0, {0}, {0}, {0}};

// 函数声明
void main_compress(void);
void main_decompress(void);

int compress_flag = 0;

int main()
{
    int choice;
    do
    {
        printf("\n======= 哈夫曼编码系统 =======\n");
        printf("1. 压缩文本 \n");
        printf("2. 解压文本 \n");
        printf("3. 退出程序\n");
        printf("==============================\n");
        printf("请选择操作 (1-3): ");

        // 判断是否输入合法
        if (scanf("%d", &choice) != 1)
        {
            printf("输入非法！请输入数字 1、2 或 3。\n");
            while (getchar() != '\n'); // 清空输入缓冲区残余的垃圾字符
            continue;
        }
        while (getchar() != '\n'); // 清空敲数字时留下的回车符

        switch (choice)
        {
        case 1:
            main_compress();
            break;

        case 2:
            main_decompress();
            break;

        case 3:
            printf("程序已安全退出。\n");
            break;

        default:
            printf("无效选项，请重新输入。\n");
            break;
        }

    } while (choice != 3);

    return EXIT_SUCCESS;
}

// 压缩
void main_compress(void)
{
    int i;

    printf("\n请输入你要压缩的文本: ");
    fgets(buf.input_text, sizeof(buf.input_text), stdin);
    buf.input_text[strcspn(buf.input_text, "\n")] = '\0'; // 抹除末尾换行符

    if (strlen(buf.input_text) == 0)
    {
        printf("输入为空，无法压缩！\n");
        return;
    }
    // 初始化统计变量
    buf.size = 0;
    memset(buf.freq, 0, sizeof(buf.freq)); // 把 freq 这个字频统计数组里的所有内容瞬间全部清零

    // 统计字频
    for (i = 0; buf.input_text[i] != '\0'; i++)
    {
        int ascii_index = (unsigned char)buf.input_text[i];
        if (buf.freq[ascii_index] == 0)
        {
            buf.data[buf.size] = buf.input_text[i];
            buf.size++;
        }
        buf.freq[ascii_index]++;
    }

    // 做成紧凑字频数组
    for (i = 0; i < buf.size; i++)
    {
        buf.final_freq[i] = buf.freq[(unsigned char)buf.data[i]];
    }

    // 生成密码本并存入全局变量 huffmancodes
    generatecodes(buf.data, buf.final_freq, buf.size);

    compress_flag = 1;

    // 打印密码本
    printf("\n【哈夫曼密码本】:\n");
    for (i = 0; i < buf.size; i++)
    {
        int ascii_index = (unsigned char)buf.data[i];
        printf("字符 '%c' 的二进制编码为: %s\n", buf.data[i], huffmancodes[ascii_index]);
    }

    // 执行转换，将文字打成 0/1 压缩流
    compresstext(buf.input_text, buf.compressed_stream);
    printf("\n【压缩成功！】\n");
    printf("生成的二进制流为: %s\n", buf.compressed_stream);
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