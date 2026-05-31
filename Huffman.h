/**
 * Huffman.h
 * 哈夫曼编码：文本压缩
 * @author lql
 * @date 2026-05-08
 */
#ifndef HUFFMAN_H
#define HUFFMAN_H

/* 全局变量外部声明 */
#define MAX_HUFFMAN_TREE_HEIGHT 100 // 哈夫曼树的最大高度
#define MAX_CHAR 256                // 最大字符数,ASCII 码共有 256 个可能字符

extern char huffmancodes[MAX_CHAR][MAX_HUFFMAN_TREE_HEIGHT]; // 存储编码数组
/* 结构体定义 */

// 哈夫曼树节点结构
typedef struct MinHeapNode
{
    char data;                 // 存储字符
    unsigned freq;             // 字符出现的次数
    struct MinHeapNode *left;  // 左子树指针
    struct MinHeapNode *right; // 右子树指针
} huffmannode;

// 最小堆结构
typedef struct MinHeap
{
    unsigned capacity;   // 堆的容量，可以装多少个节点
    unsigned curr_num;   // 当前入堆的节点的数量
    huffmannode **array; // 指针数组，存储每个树节点的指针
} minheap;

// 所有核心数据缓冲区
typedef struct
{
    huffmannode *root;            // 哈夫曼树根节点指针
    char *input_text;         // 用户输入的原始文本
    char data[MAX_CHAR];          // 字符集
    int freq[MAX_CHAR];           // 动态字频统计表
    int size;                     // 实际字符去重后的数量
    int final_freq[MAX_CHAR];     // 浓缩后的字频数组
    char *compressed_stream; // 压缩后的 0/1 字符串流
    char *decompressed_text;  // 解压还原后的文本
} databuffer;

extern databuffer buf;

/* 功能函数声明 */

// 创建新节点
huffmannode *creatNode(char data, unsigned freq);

// 创建最小堆
minheap *creatminheap(unsigned capacity);

// 交换两个节点指针
void swapMinHeapNode(huffmannode **a, huffmannode **b);

// 下沉调整小顶堆
void minHeapify(minheap *heap, int index);

// 弹出堆中频率最小的节点
huffmannode *extractmin(minheap *heap);

// 向最小堆中插入节点
void insertminheap(minheap *heap, huffmannode *node);

// 初始化并构建最小堆
minheap *buildminheap(char data[], int freq[], int size);

// 构建哈夫曼树
huffmannode *buildhuffmantree(char data[], int freq[], int size);

// 遍历树并生成编码存入数组
void storecode(huffmannode *node, int arr[], int index);

// 一键生成哈夫曼编码总函数
void generatecodes(char data[], int freq[], int size);

// 压缩函数：文字转 0/1 字符串
void compresstext(const char *inputtext, char *outputbuffer);

// 解压函数：0/1 字符串转文字
void decompress(huffmannode *root, const char *binarystring, char *desttext);

#endif // HUFFMAN_H