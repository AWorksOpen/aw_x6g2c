# awtk-widget-qr

qr 控件。用于显示二维码控件。

![](docs/images/ui.jpg)

* 本项目使用 [libqrencode](https://github.com/fukuchi/libqrencode) 生成二维码。

* 本项目使用 [awtk-widget-generator](https://github.com/zlgopen/awtk-widget-generator) 生成代码框架。

## 准备

1. 获取 awtk 并编译

```
git clone https://github.com/zlgopen/awtk.git
cd awtk; scons; cd -
```

2. 获取 awtk-widget-qr 并编译
```
git clone https://github.com/zlgopen/awtk-widget-qr.git
cd awtk-widget-qr; scons
```

## 运行

1. 生成示例代码的资源

```
python scripts/update_res.py all
```
> 也可以使用 Designer 打开项目，之后点击 “打包” 按钮进行生成；
> 如果资源发生修改，则需要重新生成资源。

如果 PIL 没有安装，执行上述脚本可能会出现如下错误：
```cmd
Traceback (most recent call last):
...
ModuleNotFoundError: No module named 'PIL'
```
请用 pip 安装：
```cmd
pip install Pillow
```

2. 编译

* 编译PC版本

```
scons
```

* 编译LINUX FB版本

```
scons LINUX_FB=true
```

> 完整编译选项请参考[编译选项](https://github.com/zlgopen/awtk-widget-generator/blob/master/docs/build_options.md)


3. 运行

```
./bin/demo
```

## 文档

[如何使用 qr 控件](docs/how_to_use_qr.md)

[完善自定义控件](https://github.com/zlgopen/awtk-widget-generator/blob/master/docs/improve_generated_widget.md)
