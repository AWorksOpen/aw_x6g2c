# 1.AWTK针对AWorks平台的移植

[AWTK](https://github.com/zlgopen/awtk)是为嵌入式系统开发的GUI引擎库。

[awtk-aworks-imx6ul](https://github.com/zlgopen/awtk-aworks-imx6ul)是AWTK在AWorks(imx6ul)上的移植。

## 一、目录简介

### 1.1 awtk-port适配层

awtk-port 是 awtk gui 在 AWorks 系统上关于 imx6ul 开发板的适配层代码

### 1.2 awtk-sdk源码

awtk的源码、例程等文件

### 1.3 构建脚本

scons_lib是编译是将awtk源码编译为库的脚本，因为在工程中添加源码进行编译体量较大，且耗时较长，所以在工程中使用链接库的方式运行awtk。

**编译库的方法，进入scons_lib目录，阅读BUILDLIB.md**

## 二、工程配置

### 2.1 添加预定义宏

进入 Eclipse，导入并选中 AWorks 工程，在如下两个地方添加宏：

```
HAS_AWTK_CONFIG
```

"Project"菜单项 -> "C/C++ Build" -> "Settings" -> "Tool Settings"标签页 -> 

- "Cross ARM C Compiler" -> "Preprocessor"
- "Cross ARM C++ Compiler" -> "Preprocessor"

### 2.2 添加 AWTK 相关的头文件路径

进入eclipse, 导入并选中AWorks工程，在如下两个地方添加路径：

```
"${workspace_loc:/${ProjName}/awtk/src}"
"${workspace_loc:/${ProjName}/awtk/src/ext_widgets}"
"${workspace_loc:/${ProjName}/awtk/3rd}"
"${workspace_loc:/${ProjName}/awtk/3rd/agge}"
"${workspace_loc:/${ProjName}/awtk/3rd/nanovg}"
"${workspace_loc:/${ProjName}/awtk/3rd/nanovg/base}"
"${workspace_loc:/${ProjName}/awtk/3rd/gpinyin/include}"
"${workspace_loc:/${ProjName}/awtk/3rd/libunibreak}"
"${workspace_loc:/${ProjName}/awtk-port}"
```

"Project"菜单项 -> "C/C++ Build" -> "Settings" -> "Tool Settings"标签页 -> 

- "Cross ARM C Compiler" -> "Includes"
- "Cross ARM C++ Compiler" -> "Includes"

## 三、其他设置问题

### 3.1 内存配置

Eclipse 工程的 imx6ul_128M.ld ，需要修改显存为4M。

```
MEMORY
{
    DMAHEAP (rw) :  ORIGIN = 0x80000000, LENGTH = 8M        /* DDR2，用于DMA堆*/
    ROM(rx) :       ORIGIN = 0x80800000, LENGTH = 8M        /* DDR2，用于存放代码 */
    RAM(rw) :       ORIGIN = 0x81000000, LENGTH = 16M       /* DDR2，用于堆栈和全局数据 */
    HEAP (rw) :     ORIGIN = 0x82400000, LENGTH = 92M       /* DDR2，用于堆*/
    VRAM (rw) :     ORIGIN = 0x82000000, LENGTH = 4M        /* DDR2，用于显存 */
    OCRAM (rw) :    ORIGIN = 0x00900000, LENGTH = 128K      /* On-Chip RAM */
}
```

### 3.2 渲染模式选择

##### 使用 AGGE 模式（默认）

- 修改 copy\_files.py

  ```
  VGCANVAS = 'NANOVG'
  NANOVG_BACKEND = 'AGGE'
  ```

- 修改 awtk_config.h

  ```
  #define WITH_NANOVG_AGGE 1
  ```

- 添加工程 include 路径

  ```
  "${workspace_loc:/${ProjName}/awtk/3rd/agge}"
  ```

##### 使用 AGG 模式

- 修改 copy\_files.py

  ```
  VGCANVAS = 'NANOVG'
  NANOVG_BACKEND = 'AGG'
  ```

- 修改 awtk_config.h

  ```
  #define WITH_NANOVG_AGG 1
  ```

- 添加工程 include 路径

  ```
  "${workspace_loc:/${ProjName}/awtk/3rd/agg}"
  ```

##### 使用 CAIRO 模式

- 修改 copy\_files.py

  ```
  VGCANVAS = 'CAIRO'
  ```

- 添加工程 include 路径

  ```
  "${workspace_loc:/${ProjName}/awtk/3rd/cairo}"
  "${workspace_loc:/${ProjName}/awtk/3rd/pixman}"
  ```

## 四、Keil 环境设置问题

AWorks 默认提供的环境是 Eclipse，如果需要在 Keil 环境下使用 AWorks 和 AWTK，则有些额外的设置工作：

1. 请在 C/C++ -> Misc Controls 中加上 **--gnu** 标志 

2. 禁用 **Use MicroLib**

    