# 简介

这是一个开发[Air780E](https://air780e.cn)的Demo.

测试硬件采用合宙EVB-780E-H开发板。

## 已知问题

- C++静态类的构造函数不可用,可编译但需要谨慎使用（若必须使用，请充分测试）。
- C++全局类的构造函数不可用,可编译但需要谨慎使用（若必须使用，请充分测试）。
- C++异常可能出现问题(不确定),可编译但需要谨慎使用（若必须使用，请充分测试）。

## 源代码下载

由于本源代码包含第三方源代码,故直接下载可能有部分源代码缺失，需要通过以下方法解决:

- 在进行git clone 使用--recurse-submodules参数。

- 若已通过git clone下载,则在源代码目录中执行以下命令下载子模块:

  ```bash
   git submodule update --init --recursive
  ```

## 资源文件

类似于桌面程序的资源文件。源代码实现的目录为 [rc](rc/)。

在固件编写中，很多时候需要大量的固定数据，直接手工嵌入到C文件里比较麻烦。

通过读取文件转换到对应C文件可大大节省时间，可添加常用的文件（如各种证书）或者不适宜放在可读写的文件系统中的文件(如需要在格式化中保留或者初始参数)。转换程序源代码为[rc/fsgen.cpp](rc/fsgen.cpp)。

使用步骤如下:

- 将待添加的文件放入 rc/fs目录下。

- 使用文件名调用以下函数(需包含相应头文件RC.h):

  ```c++
  //通过名称获取资源大小
  size_t RCGetSize(const char * name);
  
  //通过名称获取资源指针
  const unsigned char * RCGetHandle(const char * name);
  ```

# 硬件

## 状态灯

状态灯占用GPIO27，在开发板上为NET_STATUS灯。

# 编译

- 操作系统: Windows 

## 编译准备

- 本工程采用xmake管理编译树，因此需要安装好xmake，其官方网址为https://xmake.io。
- 编译过程中可能需要下载文件，因此需要在整个编译过程中保持网络畅通。

## 脚本说明

- build.bat:构建工程文件并编译。
- clean.bat:清理构建文件

## 固件烧录

正常编译完成后,最终的固件在 sdk\out目录下的子目录中,后缀名为binpkg。

此时可直接使用合宙官方的Luatools的固件下载烧录:

- USB BOOT烧录。使用USB连接PC，按下BOOT按键后进行一次复位,然后直接下载。

# 调试

采用Luatools的4G模块USB打印,采用USB调试。

