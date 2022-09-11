# OnlineGameServer
## 自定义V2版本  Actor模型 模块化编程

### 特点：

1.使用类似actor模型可以大大减少代码模块间的耦合度,  更易维护与扩展。
网络处理部分也是特殊模块，可选择使用TCP或UDP

2.无需额外调用注册函数，生成出来的模块对象自动注册

3.消息对象采用预开辟内存方式  减少频繁new带来的内存碎片等

4.可指定消息接收方（只有指定接收方可收到该消息）也可不指定（凡是注册该消息的模块都会收到消息）

5.支持Lua脚本,可同时运行LUA或C++模块，并且可相互通信，支持运行时删除，重载，新增模块

### 结构:


ModuleManager:  生成与管理Module的类，自身有一个全局消息队列,对消息进行注册与分发

ModuleBase: 模块基类 写模块时继承该类 需要覆写Init(初始化运行函数),update(每循环运行),parseMessage(解析消息函数)。生成出来的对象会自动注册到ModuleManager中进行管理。

ThreadManager：线程管理类 管理处理模块的线程 通过该类开启整个服务器运行线程

ConsoleCmd: 控制台指令类 在主线程运行 用于注册/解析控制台指令

ConsoleLog：控制台日志类 用于输出日志 (在单独的一个线程中运行 使用条件变量进行唤醒与阻塞避免一直while循环)


### 截图

在C++中注册模块

![QQ截图20220417161535](https://user-images.githubusercontent.com/60800578/163706511-e6b7432a-2140-4e3e-ab54-c11e660eb76d.png)

在Lua中注册模块

![QQ截图20220427135037](https://user-images.githubusercontent.com/60800578/165450344-5e3d55bc-9ea1-460c-bf32-912caa1317df.png)



![QQ截图20220417161731](https://user-images.githubusercontent.com/60800578/163706514-667eb01b-0770-4909-a37e-5875e93429ef.png)






构思图
![Actor游戏服务器架构思路](https://user-images.githubusercontent.com/60800578/163332879-fc3776c5-8c01-45ba-9d11-168609a29366.png)



#### lua注册的API
注册消息
ServerLuaLib.RegMessage(消息ID,模块ID)

关闭模块
ServerLuaLib.CloseModule(模块ID);

加载模块
ServerLuaLib.LoadNewModule(模块ID,模块文件名)

发送消息
ServerLuaLib.SendMessage(消息ID,数据,发送源模块ID);

获取时间戳
ServerLuaLib.GetTime();



## 自定义版本V1版（old已弃坑） （WINDOWS IOCP多线程TCP自定义游戏联机服务器框架 配合DB服务器 ）


### 特点：

使用iocp高效处理并发数据

使用消费者 生产者模式（io线程只负责收取与简单处理数据（如解密）不负责处理业务逻辑，将简单处理后数据推送给业务队列中，业务线程将从队列中读取并处理数据）

使用高效的多线程安全队列

使用哈希数组进行客户端玩家信息保存，查找读取客户端信息很高效

部分使用预开辟内存 内存池 减少new带来的消耗

数据进行简单加密传输

可自定义数据协议   （默认按照 ：头-大小-数据）

利用信号量唤醒业务线程 避免一直循环



### v1版服务器结构


![TCP游戏服务器架构](https://user-images.githubusercontent.com/60800578/135739241-7277575f-26ab-4210-9521-9185c0da1b95.png)


当前已包含简单的登录 注册功能

配合DB服务器使用https://github.com/FutureXXH/DBSERVER





