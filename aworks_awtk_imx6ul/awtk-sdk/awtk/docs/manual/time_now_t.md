## time\_now\_t
### 概述
获取当前时间的函数。
这里的当前时间是相对的，在嵌入式系统一般相对于开机时间。
它本身并没有任何意义，一般用来计算时间间隔，如实现定时器和动画等等。
----------------------------------
### 函数
<p id="time_now_t_methods">

| 函数名称 | 说明 | 
| -------- | ------------ | 
| <a href="#time_now_t_time_now_ms">time\_now\_ms</a> | 获取当前时间(毫秒)。 |
| <a href="#time_now_t_time_now_s">time\_now\_s</a> | 获取当前时间(秒)。 |
| <a href="#time_now_t_time_now_us">time\_now\_us</a> | 获取当前时间(微秒)。 |
#### time\_now\_ms 函数
-----------------------

* 函数功能：

> <p id="time_now_t_time_now_ms">获取当前时间(毫秒)。

* 函数原型：

```
uint64_t time_now_ms ();
```

* 参数说明：

| 参数 | 类型 | 说明 |
| -------- | ----- | --------- |
| 返回值 | uint64\_t | 返回当前时间(毫秒)。 |
#### time\_now\_s 函数
-----------------------

* 函数功能：

> <p id="time_now_t_time_now_s">获取当前时间(秒)。

* 函数原型：

```
uint64_t time_now_s ();
```

* 参数说明：

| 参数 | 类型 | 说明 |
| -------- | ----- | --------- |
| 返回值 | uint64\_t | 返回当前时间(秒)。 |
#### time\_now\_us 函数
-----------------------

* 函数功能：

> <p id="time_now_t_time_now_us">获取当前时间(微秒)。

* 函数原型：

```
uint64_t time_now_us ();
```

* 参数说明：

| 参数 | 类型 | 说明 |
| -------- | ----- | --------- |
| 返回值 | uint64\_t | 返回当前时间(微秒)。 |