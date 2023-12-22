# 简介

这是一个开发[Air780E](https://air780e.cn)的Demo.

测试硬件采用合宙EVB-780E-H开发板。

## 已知问题

- C++静态类的构造函数不可用(可由应用代码支持),可编译但需要谨慎使用（若必须使用，请充分测试）。
- C++全局类的构造函数不可用(可由应用代码支持),可编译但需要谨慎使用（若必须使用，请充分测试）。
- C++异常可能出现问题(不确定),可编译但需要谨慎使用（若必须使用，请充分测试）。

## Kconfig

采用python软件包kconfiglib,通过pyinstaller打包而来,主要打包以下工具:

- genconfg
- guiconfig

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

注意:非Windows下可采用[wine](https://www.winehq.org/)编译。

## 编译准备

编译本工程的前提是能够正常编译原合宙官方SDK，详细说明见[luatos-soc-2022](https://gitee.com/openLuat/luatos-soc-2022.git)。

- 本工程采用xmake管理编译树，因此需要安装好xmake，其官方网址为[https://xmake.io](https://xmake.io)。
- 编译过程中可能需要下载文件，因此需要在整个编译过程中保持网络畅通。

### Ubuntu

对于Ubuntu可采用以下命令安装wine:

```bash
sudo apt-get install wine winbind
```

当wine安装好后,也需要安装xmake（Windows版），使用`wine 安装程序名称`启动安装程序并安装。

## 脚本说明

- build.bat:构建工程文件并编译。
- clean.bat:清理构建文件
- Kconfig.bat:调整Kconfig设置。

对于非Windows，启动脚本需要使用`wine cmd /c 脚本路径`启动脚本。

## 代码编辑

当成功执行一次构建操作后，[sdk](sdk)目录将产生CMakeLists.txt,利用[CMake](https://cmake.org/)可生成某些IDE的工程文件，可用于代码编辑(不一定可编译)。

本人常用[CodeBlocks](https://www.codeblocks.org/)作为IDE，产生CodeBlocks的工程文件步骤如下:

- 安装好CMake与[Ninja](https://ninja-build.org/)。
- 创建build目录并进入build目录
- 执行 `cmake -G "CodeBlocks - Ninja" ../sdk`生成工程文件。
- 使用CodeBlocks打开生成的cbp工程文件，进行代码编辑。

## 固件烧录

正常编译完成后,最终的固件在 sdk\out目录下的子目录中,后缀名为binpkg。

此时可直接使用合宙官方的Luatools的固件下载烧录:

- USB BOOT烧录。使用USB连接PC，按下BOOT按键后进行一次复位,然后直接下载。

# 调试

采用Luatools的4G模块USB打印,采用USB调试。

