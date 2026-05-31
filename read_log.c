/**
 * read_log.c
 * 哈夫曼编码：安全读取外部历史日志文件
 * @author lql
 * @date 2026-05-31
 */

#include <stdio.h>
#include <stdlib.h> 
#include "read_log.h"

// 打开日志
void read_coding_log(void)
{
    // 调用终端分页器 less
    system("less -S -f coding_log.txt");
}

// 一键清空日志
void clear_coding_log(void)
{
    FILE *fp = fopen("coding_log.txt", "w");
    
    if (fp)
    {
        fclose(fp); // 任务完成，直接关闭
        printf("\n【成功】历史日志已全部一键清空！\n");
    }
    else
    {
        printf("\n【错误】清空日志失败。\n");
    }
}