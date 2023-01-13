版本记录
========
.. describe:: 2.0.5 <2023-01-12>
    
    * update the DDR3 initialization configuration of uboot_burn_AWorks tools.
    
.. describe:: 2.0.4 <2022-12-27>
    
    * gpmi support EDO mode.

.. describe:: 2.0.3 <2022-11-8>
    
    * adapter littlefs
    
.. describe:: 2.0.2 <2022-02-14>
    
    * fixed an external hardware watchdog issue.
    
.. describe:: 2.0.1 <2022-02-09>
    
    * awtk:update aworks_awtk_imx6ul to 1.0.1
    
.. describe:: 2.0.0 <2021-11-29>
    * 发布2.0.0正式版本sdk包

.. describe:: 2.0.0-beta.2 <2021-10-28>
    * 解决第二轮测试BUG
    * 处理了examples子模块BUG
    * 除了gprs组件接收短信BUG
    
.. describe:: 2.0.0-beta.1 <2021-10-28>
    * 解决第一轮测试BUG

.. describe:: 2.0.0-beta <2021-03-06>
    * 移动awtk工程至prjects目录下
    * 添加gcov相关测试配置
    
.. describe:: 1.1.0       <2020-07-04>
    * 增加软件复位的功能

    * 文件系统：         修改文件系统默认挂载磁盘数为5
    * lffs文件系统：     更新坏块处理功能
                         处理查找系统信息块时，未检查坏块的问题
                         mkfs 时增加检查该 dev_path 是否有被挂载的功能
                         处理链表未初始化的问题，优化低格时速度较慢问题
                         
    * yaffs文件系统：    处理 lseek 操作的问题

    * shell命令：        增加cp命令
                         更新wifi startap命令问题，提前检查参数格式
                         更改stack的shell指令文件，添加最大栈深信息
                                                  
    * log日志：          增加log缓存

    * gprs:              修改 ec20 结构，更新gprs状态检查机制，完善log

    * wifi:              修复wifi的join命令静态ip设置的问题
                         消除固件丢失后启动AP时发生的空指针引用
                         修复使用AW_WO_PWR_CTL关闭再重新打开wifi电源后，用户设置的漫游参数改变问题
                         
    * net：              增加以太网对虚拟网卡的支持
                         网卡管理功能增加dns事件处理
                         增加支持使用域名来检测可用网卡
                         处理udp用静态路由表查找异常问题
                         修正ping间歇读数过大与解析域名时消息显示错误问题
                         dhcps中去掉对link up和link down的操作，当dhcps参数有更新时，由应用对网口进行重启 
            
    * USB：              更新USB文件与USB库，修复若干个bug，增加mtp功能

    * canopen:           增加canopen软件库

    * drivers：          增加cyw43438 wifi驱动
                         更新nandflash组件及增加相关nanflash驱动
                         添加gm510 4g模块驱动
                        
    * projects：         增加ec200t配置文件
                         更新LCD配置文件，添加动态配置和复位功能
                         
    * examples：         增加canopen 主从站例程
                         修改usb_ms例程存储空间分配的问题
                         增加USB HOST摄像头、串口demo、U盘测速、mtp例程                            
                         增加USB Device鼠标、键盘、虚拟串口demo
                             
.. describe:: 1.0.0       <2020-02-28>

    * 增加支持AWTK应用    
    * 修改nand配置文件，增加系统数据存储段
    * 更正文档中的错误    
    * log增加时区设置
    * 更新摄像头驱动   
    * 修改目录结构，使其为英文目录
    * 增加 Wi-Fi 隐藏热点    
    * 适配 CYW43362 WIFI驱动
    * 添加 cpuload 命令的配置
    * 添加 wifi 例程
    
.. describe:: 0.0.3-alpha <2019-09-05>

    * 评估版本

.. describe:: 0.0.2-alpha <2019-03-22>

    * 评估版本
    
.. describe:: 0.0.1-alpha <2019-01-10>

    * 评估版本