# 内存需求评估

## 一、awtk-mvvm

1. 一条数据绑定规则：约 180 字节；
2. 一条命令绑定规则：约 180 字节；
3. 一条动态绑定规则：约 100 字节。

> 以上绑定规则计算中的指针成员为 4 字节，在指针占用 2 字节的嵌入式平台，内存占用量更低，每条约减少 30 字节。

## 二、jerryscript (可选)

### 2.1 jerryscript 所需内存

| 名称                 | 宏                     | RAM         | 备注                                                         |
| -------------------- | ---------------------- | ----------- | ------------------------------------------------------------ |
| jerryscript 正常运行 | WITH_JS                | 约 500 KB   | jerryscript 本身 + mvvm_factory.res 资源                     |
| jerryscript 内存池   | JERRY_GLOBAL_HEAP_SIZE | 默认2048 KB | 数据层 js 脚本中定义变量将从内存池中分配内存，若内存池空间不足，程序将无法正常运行。 |

> 如果要把 awtk-mvvm 中的所有 demo 跑起来 ，jerryscript 内存池建议最低可以给 64 KB，实际上如果 demo 无数据，仅调用 mvvm_jerryscript_init() 接口进行初始化，最低只需 8 KB。

### 2.2 js 数据层

在 js 数据层中定义变量所占内存：

1. ViewModel：约 700 字节，ViewModel名称越长占用字节数越多，每增加一个字符多占用 2 字节；
2. Number： 约 200 字节；
3. String：最低约 200 字节，与字符长度有关，每增加一个字符多占用 1 字节；
4. Array： 最低约 300 字节；
5. Object：最低约 70 字节；

> 注意：
> * 以上数据都是在 ViewModel 中定义变量所占的内存，如果直接在 ViewModel 外部定义变量，内存占用量更低；
> * 以上描述中的"最低"表示该数据类型为空时占用的字节数。

> 如果需要监控 jerryscript 内存池的使用情况，可以在定义宏 JERRY_MEM_STATS=1 后，调用  jerry_get_memory_stats() 接口获取内存池状态，或者调用 jmem_heap_stats_print() 接口打印更详细的内存池使用状态。

> 注意：程序运行时，还需考虑 fb 以及其他资源文件所占用的内存，比如字库、UI、图片等。