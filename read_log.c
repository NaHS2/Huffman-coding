/**
 * read_log.c
 * 哈夫曼编码：安全读取外部历史日志文件
 * @author lql
 * @date 2026-05-31
 */

#include <stdio.h>
#include <stdlib.h> 
#include "read_log.h"

void read_codeing_log(void)
{
    system("less -S coding_log.txt");
}