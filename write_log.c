/**
 * write_log.c
 * 哈夫曼编码：将编码结果与对照表写入日志文件
 * @author lql
 * @date 2026-05-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Huffman.h"

// 引入 main.c 中定义的共享数据缓冲区结构体
typedef struct
{
    huffmannode *root;            
    char input_text[256];         
    char data[MAX_CHAR];          
    int freq[MAX_CHAR];           
    int size;                     
    int final_freq[MAX_CHAR];     
    char compressed_stream[2048]; 
    char decompressed_text[256];  
} databuffer;

extern databuffer buf; // 引用全局变量

// 将当前会话数据安全追加到日志中
void write_coding_log(void)
{
    int i;
    FILE *log_file = fopen("codeing_log.txt", "a"); // 以追加模式打开
    
    if (!log_file)
    {
        printf("【警告】无法创建或打开日志文件 codeling_log.txt。\n");
        return;
    }

    // 格式化写入文件
    fprintf(log_file, "--- 编码日志记录 ---\n");
    fprintf(log_file, "原始文本: %s\n", buf.input_text);
    fprintf(log_file, "压缩01流: %s\n", buf.compressed_stream);
    fprintf(log_file, "密码本对照:\n");
    
    for (i = 0; i < buf.size; i++)
    {
        int ascii_index = (unsigned char)buf.data[i];
        fprintf(log_file, "  '%c': %s\n", buf.data[i], huffmancodes[ascii_index]);
    }
    
    fprintf(log_file, "--------------------\n\n");
    
    fclose(log_file); 
    printf("（本次编码日志已安全写入 codeing_log.txt）\n");
}