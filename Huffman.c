/**
 * Huffman.c
 * 哈夫曼编码：文本压缩
 * @author lql
 * @date 2026-05-08
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Huffman.h"

/* 构造huffman树 */

// 1.创建新节点
huffmannode *creatNode(char data, unsigned freq)
{
    // 初始化空间
    huffmannode *node = malloc(sizeof(huffmannode));

    // 赋值
    node->data = data;
    node->freq = freq;

    // 新节点没有左右孩子，赋空
    node->left = NULL;
    node->right = NULL;

    return node;
}

// 2.创建最小堆
minheap *creatminheap(unsigned capacity)
{
    // 初始化空间
    minheap *heap = malloc(sizeof(minheap));

    // 赋值
    heap->capacity = capacity;
    heap->curr_num = 0;
    heap->array = malloc(heap->capacity * sizeof(huffmannode *));

    return heap;
}

// 3.交换函数：交换 huffmannode*（节点的地址）
void swapMinHeapNode(huffmannode **a, huffmannode **b)
{
    huffmannode *t = *a;
    *a = *b;
    *b = t;
}

// 4.下沉函数：初始化为小顶堆，比较每个节点的频率
void minHeapify(minheap *heap, int index)
{
    int smallest = index; // 假设当前节点最小
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // 检查左孩子：
    // 1.判断是否越界：左孩子必须要小于当前入堆的节点的数量
    // 2.判断是否调换：当前堆中左孩子的频率是否小于当前堆中最小节点的频率，小于则调换
    if (left < heap->curr_num && heap->array[left]->freq < heap->array[smallest]->freq)
    {
        smallest = left;
    }

    // 检查右孩子：
    // 1.判断是否越界：右孩子也必须要小于当前入堆的节点的总数
    // 2.判断是否调换：同上
    if (right < heap->curr_num && heap->array[right]->freq < heap->array[smallest]->freq)
    {
        smallest = right;
    }
    // 如果最小的节点不是当前节点则交换
    if (smallest != index)
    {
        swapMinHeapNode(&heap->array[index], &heap->array[smallest]);
        // 递归，继续向下调整
        minHeapify(heap, smallest);
    }
}

// 5.弹出堆中频率最小的节点
huffmannode *extractmin(minheap *heap)
{
    // 构建的小顶堆，堆顶为最小节点
    huffmannode *extract_value = heap->array[0];

    // 把当前堆堆底的值置换到堆顶，重新下沉
    heap->array[0] = heap->array[heap->curr_num - 1];

    // 堆中节点实际数量减一
    --heap->curr_num;

    // 从堆顶重新下沉
    minHeapify(heap, 0);

    return extract_value;
}

// 6.向最小堆中插入节点
void insertminheap(minheap *heap, huffmannode *node)
{
    // 当前入堆的节点的数量加一
    ++heap->curr_num;

    // 新插入的节点要放入堆底，上浮
    int insert_value = heap->curr_num - 1;

    // 上浮
    while (insert_value && node->freq < heap->array[(insert_value - 1) / 2]->freq)
    {
        // 如果新节点的频率小于父节点，那么新节点就上浮
        heap->array[insert_value] = heap->array[(insert_value - 1) / 2];
        insert_value = (insert_value - 1) / 2;
    }

    // 现在已经确定好了要插入的位置，将新节点插入
    heap->array[insert_value] = node;
}

// 7.把字符和字频数组打包，初始化为小顶堆
minheap *buildminheap(char data[], int freq[], int size)
{
    minheap *heap = creatminheap(size);

    // 把每个字符及其对应的频率存入数组
    for (int i = 0; i < size; i++)
    {
        heap->array[i] = creatNode(data[i], freq[i]);
    }

    // 更新当前堆中实际存入节点的数量
    heap->curr_num = size;

    // 开始对所有存入的节点进行下沉
    for (int i = (size - 2) / 2; i >= 0; i--)
    {
        minHeapify(heap, i);
    }

    // 返回构建完毕的最小堆的指针
    return heap;
}

// 8.合并最小节点，构建哈夫曼树
huffmannode *buildhuffmantree(char data[], int freq[], int size)
{
    huffmannode *left, *right, *top;

    // 将创建好的所有节点都放入最小堆中
    minheap *heap = buildminheap(data, freq, size);

    // 将最小堆中的所有节点弹出，存入哈夫曼树
    while (heap->curr_num != 1)
    {
        // 弹出两个频率最小的节点，分别为左孩子和右孩子
        left = extractmin(heap);
        right = extractmin(heap);

        // 创建新节点，频率为当前两个最小节点的频率的和
        top = creatNode('\0', left->freq + right->freq);
        top->left = left;
        top->right = right;

        // 将创建的新节点重新插入堆中参与下一轮比较
        insertminheap(heap, top);
    }
    // 循环结束时，最小堆中只剩下最大的那个节点，为哈夫曼树的根节点，也需要弹出
    return extractmin(heap);
}

/* 根据构建好的哈夫曼树进行哈夫曼编码 */
char huffmancodes[MAX_CHAR][MAX_HUFFMAN_TREE_HEIGHT]; // 存储编码数组

// 1.存储哈夫曼编码码字函数，递归遍历哈夫曼树，把所有节点化作 0/1 存入编码数组
void storecode(huffmannode *node, int arr[], int index)
{
    // 如果左孩子存在，则说明往左走得通
    if (node->left)
    {
        arr[index] = 0;                        // 左，当前节点记为 0
        storecode(node->left, arr, index + 1); // 继续处理左孩子
    }

    // 如果右孩子存在，则说明往右走得通
    if (node->right)
    {
        arr[index] = 1;                         // 右，当前节点记为 1
        storecode(node->right, arr, index + 1); // 继续往下递归
    }

    // 如果左右节点都不存在，则在叶子节点，即存储字符的节点
    if (!(node->left || node->right))
    {
        int i;
        int ascii_index = (unsigned char)node->data; // huffmancodes 是个二维数组，第一维是字符的 ASCII 值，必须保证下标非负
        for (i = 0; i < index; i++)
        {
            huffmancodes[ascii_index][i] = arr[i] + '0'; // 将数字 0 1变成可以打印的字符 0 1
        }
        huffmancodes[ascii_index][i] = '\0'; // 字符串结束符
    }
}

// 2.根据输入的字符及其频率，生成哈夫曼编码，并存储到全局编码表 huffmancodes 中
void generatecodes(char data[], int freq[], int size)
{
    // 构建哈夫曼树
    huffmannode *node = buildhuffmantree(data, freq, size);

    // 初始化临时路径记录数组
    int arr[MAX_HUFFMAN_TREE_HEIGHT];
    int index = 0;

    // 调用编码函数
    storecode(node, arr, index);
}

// 3.压缩：将原始文本根据密码本转换成 0/1 组成的压缩字符串
void compresstext(const char *inputtext, char *outputbuffer)
{
    // 初始化输出缓冲区
    outputbuffer[0] = '\0';

    //
    for (int i = 0; inputtext[i] != '\0'; i++)
    {
        int ascii_index = (unsigned char)inputtext[i]; // 防止出现负数下标

        strcat(outputbuffer, huffmancodes[ascii_index]); // 字符串拼接
    }
}

// 4.解压缩 顺着哈夫曼树，把 0/1 字符串重新还原成原始文本
void decompress(huffmannode *root, const char *binarystring, char *desttext)
{
    // 如果树根为空，则直接返回
    if (!root)
    {
        return;
    }

    huffmannode *node = root; // 哈夫曼树的入口
    int dest_index = 0;

    for (int i = 0; binarystring[i] != '\0'; i++)
    {
        if (binarystring[i] == '0') // 等于 0 则往左走
        {
            node = node->left;
        }
        else if (binarystring[i] == '1') // 等于 1 则往右走
        {
            node = node->right;
        }
        // 如果没有左右孩子，则说明到达叶子节点，成功找打字符
        if (!(node->left || node->right))
        {
            desttext[dest_index] = node->data; // 将找到的字符存入还原数组
            dest_index++;
            node = root; // 指针重新回到树根，准备下一次还原
        }
    }

    // 在还原数组末尾加上结束字符
    desttext[dest_index] = '\0';
}