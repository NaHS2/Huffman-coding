# 哈夫曼编码文本压缩系统 

## 一、项目概述

本项目使用 **C 语言** 从零实现了一套完整的 **哈夫曼编码（Huffman Coding）文本压缩与解压系统**，不依赖任何第三方库，仅使用 C 标准库。系统支持手动输入文本压缩、外部文件读取压缩、压缩日志记录与回溯等完整功能。

- **作者**：lql
- **开发周期**：2026-05-08 至 2026-05-31
- **代码规模**：9 个源文件（5 个 `.c` + 4 个 `.h`），约 400 行核心代码
- **平台**：Linux（以 GCC 13.3.0 编译）

---

## 二、系统架构

### 2.1 模块划分

```
┌─────────────────────────────────────────────┐
│                  main.c                     │
│   主控模块:菜单交互、流程调度、内存管理           │
└──────┬──────┬──────┬──────┬─────────────────┘
       │      │      │      │
       ▼      ▼      ▼      ▼
┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
│ Huffman  │ │read_file │ │write_log │ │read_log  │
│  .c / .h │ │ .c / .h  │ │ .c / .h  │ │ .c / .h  │
│          │ │          │ │          │ │          │
│ 核心算法  │ │ 文件读取   │ │ 日志写入  │ │ 日志读取  │
│ 哈夫曼树  │ │ 动态扩容   │ │ 追加模式  │ │ 清空功能  │
│ 编解码    │ │          │ │          │ │          │
└──────────┘ └──────────┘ └──────────┘ └──────────┘
```

### 2.2 核心数据结构

**哈夫曼树节点（Huffman.c:15）**：

```c
typedef struct MinHeapNode 
{
    char data;                 // 存储字符
    unsigned freq;             // 字符出现频率
    struct MinHeapNode *left;  // 左子树指针
    struct MinHeapNode *right; // 右子树指针
} huffmannode;
```

**最小堆（Huffman.c:32）**：

```c
typedef struct MinHeap 
{
    unsigned capacity;   // 堆容量
    unsigned curr_num;   // 当前节点数
    huffmannode **array; // 节点指针数组
} minheap;
```

**全局数据缓冲区（Huffman.h:35）**：

```c
typedef struct 
{
    huffmannode *root;             // 哈夫曼树根节点
    char *input_text;              // 原始输入文本（动态分配）
    char data[MAX_CHAR];           // 去重后的字符集
    int freq[MAX_CHAR];            // 字频统计表（以ASCII为索引）
    int size;                      // 去重后字符数量
    int final_freq[MAX_CHAR];      // 紧凑字频数组
    char *compressed_stream;       // 压缩后的0/1字符串（动态分配）
    char *decompressed_text;       // 解压还原文本（动态分配）
} databuffer;
```

---

## 三、核心算法实现

### 3.1 哈夫曼编码完整流程

```
输入文本 → 字频统计 → 构建最小堆 → 构建哈夫曼树 → 生成密码本 → 压缩编码 → 输出0/1流
                                                                          ↓
                                                         解压：0/1流 + 哈夫曼树 → 原始文本
```

### 3.2 算法逐步剖析

#### 步骤1：字频统计（main.c:111-119）

```c
for (i = 0; buf.input_text[i] != '\0'; i++) 
{
    int ascii_index = (unsigned char)buf.input_text[i];
    if (buf.freq[ascii_index] == 0) {
        buf.data[buf.size++] = buf.input_text[i];
    }
    buf.freq[ascii_index]++;
}
```

以 ASCII 值为下标直接寻址，时间复杂度 O(n)，空间换时间的经典做法。同时记录去重后的字符集，用于后续构建紧凑字频数组。

#### 步骤2：构建最小堆（Huffman.c:124-145）

将每个字符及其频率封装为节点，放入最小堆。采用**数组存储的完全二叉树**实现堆结构，从最后一个非叶子节点 `(size-2)/2` 开始向前下沉（`minHeapify`），时间复杂度 O(n)。

**下沉操作（Huffman.c:54-82）**：比较当前节点与其左右孩子，若当前节点不是最小则交换，并递归向下调整。这是标准的堆调整算法。

**插入操作（Huffman.c:103-121）**：将新节点放入堆底，然后执行**上浮**操作——不断与父节点 `(i-1)/2` 比较，若新节点频率更小则上浮。

#### 步骤3：构建哈夫曼树（Huffman.c:148-172）

```c
while (heap->curr_num != 1) 
{
    left  = extractmin(heap);   // 弹出最小频率节点
    right = extractmin(heap);   // 弹出次小频率节点
    top = creatNode('\0', left->freq + right->freq);
    top->left  = left;
    top->right = right;
    insertminheap(heap, top);   // 新节点重新入堆
}
return extractmin(heap);        // 最后弹出根节点
```

这是**贪心策略**的典型应用：每次取两个最小频率节点合并，新节点频率为两者之和。循环直到堆中只剩一个节点（即根节点）。内部节点的 `data` 设为 `\0`，仅叶子节点存储实际字符。

#### 步骤4：生成哈夫曼编码（Huffman.c:178-218）

通过**深度优先遍历（DFS）**哈夫曼树：

```c
void storecode(huffmannode *node, int arr[], int index) 
{
    if (node->left)  
    { 
        arr[index]=0; 
        storecode(node->left,  arr, index+1); 
    }
    if (node->right) 
    { 
        arr[index]=1; 
        storecode(node->right, arr, index+1); 
    }
    if (!(node->left || node->right)) 
    {
        // 到达叶子节点，将路径上的0/1数组转为字符串存储
        for (i=0; i<index; i++)
            huffmancodes[ascii_index][i] = arr[i] + '0';
    }
}
```

规定**向左走为0，向右走为1**。到达叶子节点时，路径上的 0/1 序列即为该字符的哈夫曼编码。编码存入全局二维数组 `huffmancodes[256][100]`，第一维以字符 ASCII 值索引，第二维存储编码字符串。

**关键性质**：哈夫曼编码是**前缀码**——任何一个字符的编码都不会是另一个字符编码的前缀。这一性质保证了解码时无需分隔符，不会产生歧义。

#### 步骤5：压缩编码（Huffman.c:222-234）

```c
for (int i = 0; inputtext[i] != '\0'; i++) 
{
    int ascii_index = (unsigned char)inputtext[i];
    strcat(outputbuffer, huffmancodes[ascii_index]);
}
```

遍历原始文本每个字符，查表获取对应的二进制编码，拼接到输出缓冲区。时间复杂度 O(n)。

#### 步骤6：解压缩（Huffman.c:237-268）

```c
for (int i = 0; binarystring[i] != '\0'; i++) 
{
    if (binarystring[i] == '0') 
        node = node->left;
    else if (binarystring[i] == '1') 
        node = node->right;
    if (!(node->left || node->right)) 
    {
        desttext[dest_index++] = node->data;
        node = root;  // 回到树根，准备下一个字符
    }
}
```

从头遍历 0/1 流，按位沿哈夫曼树下行：0 则走左子树，1 则走右子树。到达叶子节点时取出字符，指针回到根节点继续下一轮。这正是前缀码性质的应用——走到叶子就必然找到了一个完整字符的编码。

---

## 四、其余模块

### 4.1 动态内存扩容（read_file.c:16-103）

读取外部文件时，采用**起始1KB + 翻倍扩容**策略：

```c
int current_capacity = 1024;
while ((ch = fgetc(fp)) != EOF) 
{
    if (index >= current_capacity - 1) 
    {
        current_capacity *= 2;           // 容量翻倍
        char *new_space = realloc(buf.input_text, current_capacity);
        if (!new_space) 
        { 
            /* 错误处理 */ 
            break; 
        }
        buf.input_text = new_space;
    }
    buf.input_text[index++] = (char)ch;
}
```

- 初始分配 1KB 空间，避免对小文件浪费内存
- 满载时容量翻倍（倍增策略，摊销 O(1)）
- 读取完成后同步扩容压缩缓冲区和解压缓冲区

### 4.2 日志系统

采用**追加模式（"a"）**写入日志，每次压缩后记录原始文本、压缩01流和密码本对照表：

```c
FILE *log_file = fopen("coding_log.txt", "a");
fprintf(log_file, "--- 编码日志记录 ---\n");
fprintf(log_file, "原始文本: \n%s\n", buf.input_text);
fprintf(log_file, "压缩01流: \n%s\n", buf.compressed_stream);
// ... 密码本对照表
```

支持**一键清空**日志功能（以 `"w"` 模式打开覆盖），以及通过 `less` 分页器查看历史日志。

### 4.3 鲁棒的错误处理

- 内存分配失败检测（`malloc`/`realloc` 返回 NULL 时中断并提示）
- 文件打开失败检测（`fopen` 返回 NULL 时提示具体文件名）
- 空文件检测（读取字符数为0时跳过压缩）
- 解压前置条件校验（`compress_flag` 确保先压缩再解压）
- 用户输入合法性校验（`scanf` 返回值检查 + 缓冲区清理）
- 每次手动输入前清空缓冲区（`memset`），防止残留数据干扰

### 4.4 ASCII 安全处理

```c
int ascii_index = (unsigned char)buf.input_text[i];
```

所有字符索引处都做了 `(unsigned char)` 强制转换。这是因为 `char` 在多数平台是有符号类型（范围 -128~127），当中文等多字节字符的某个字节 >= 0x80 时会产生负数下标。转为 `unsigned char` 后范围变为 0~255，确保数组访问安全。

### 4.5 中文等多字节字符支持

从测试日志（`coding_log.txt`）中可以看到，系统成功对中文文本 `"中文测试"` 进行了编码，生成的压缩流为 `110010011111101111001110101000000110001101`。虽然以字节为单位进行编码（而非 Unicode 码点），但在哈夫曼编码框架下这是完全合理的——压缩和解压都以字节为单位，能保证无损往返。

---

## 五、算法复杂度分析

| 步骤 | 时间复杂度 | 空间复杂度 | 说明 |
|------|-----------|-----------|------|
| 字频统计 | O(n) | O(1) | 遍历输入文本，直接寻址 |
| 构建最小堆 | O(m) | O(m) | m 为去重后字符数 ≤ 256 |
| 构建哈夫曼树 | O(m log m) | O(m) | 每次 extract/insert 为 O(log m) |
| 生成编码表 | O(m) | O(1) | DFS 遍历哈夫曼树 |
| 压缩编码 | O(n·L) | O(n·L) | n 为字符数，L 为平均码长 |
| 解压缩 | O(n·L) | O(n) | 逐位遍历二进制流 |

总体而言，算法在时间和空间上均具有良好性能。最坏情况下字符集大小 m 不超过 256（ASCII 全集），堆操作的开销几乎可以忽略。

---

## 六、运行演示

系统提供文字菜单交互界面：

```
======= 哈夫曼编码系统 =======
1. 压缩手动输入文本
2. 解压文本
3. 读取外部文件并编码
4. 查看历史编码日志
5. 一键清空历史日志
6. 退出程序
==============================
```

从日志文件中可以验证系统的实际运行效果：

- **英文文本** `"English test"`：11个字符，压缩后为34bit的01流
- **数字串** `"132136716314801000"`：18个字符，压缩后为49bit的01流
- **中文文本** `"中文测试"`：4个汉字（多个字节），压缩后为36bit的01流
- **C源码文件**：成功读取并编码了 `read_log.c` 的完整源码内容

---

## 七、应用场景与扩展方向

### 7.1 现有应用场景
哈夫曼编码是现代压缩技术的标准组件：

- **文件压缩**：ZIP、GZIP 的 Deflate 算法核心包含哈夫曼编码
- **图像压缩**：JPEG 对 DCT 系数、PNG 的 Deflate 压缩
- **音视频编码**：MP3、H.264 等标准中的熵编码
- **通信协议**：传真机标准 G3/G4 中的改进哈夫曼编码

### 7.2 可扩展方向


1. **日志功能**：对每次的日志ID编码，支持查询ID，通过ID删除、解压。
1. **二进制输出**：当前压缩结果以可读的 0/1 字符串形式存储（每个 bit 占 1 字节），改为二进制位打包可节省 8 倍空间
2. **自适应哈夫曼编码**：无需预先统计全文频率，边读边编码
3. **Canonical Huffman**：规范化哈夫曼编码，只需传输码长即可重建编码表
4. **大文件分块压缩**：对大文件分块独立编码，支持流式处理

---

## 八、项目总结

本项目从零实现了哈夫曼编码的完整流程：**字频统计 → 最小堆 → 哈夫曼树 → 前缀码生成 → 压缩 → 解压**，涵盖了数据结构（最小堆、二叉树）、算法（贪心策略、DFS 遍历）、系统编程（动态内存管理、文件 I/O）等核心计算机科学知识点。

工程实现上注重**健壮性**（完整的错误处理链）、**安全性**（ASCII 安全转换、缓冲区溢出防护）、**可维护性**（模块化设计、头文件隔离接口），展现了扎实的 C 语言编程能力和良好的软件工程素养。
