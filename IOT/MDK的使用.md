5.x版本之后，默认不安装任何 Device Pack，即芯片支持包，而是采用组件的方式让用户自行选择所需要支持的CPU包。对应的 Device Pack 可以到这里下载：https://www.keil.com/dd2/pack/

示例中需要下载的是 STMicroelectronics STM32F1 Series Device Support

pack下载后双击运行即可

## 创建工程

创建前将要调试的hex文件与工程放在同一个目录下

Project -> New μ Vision Project...

随便找个地方保存后选择CPU

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-1.png)

选择完毕后确认，会弹出如图

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-2.png)

这个功能开发时才需要使用，这里直接Cancel即可

## 选择文件与调试

然后在 Source Group 1 上单击右键，将我们打算分析的 hex 文件加入工程

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-3.png)

在框中，文件类型选择 ALL Files (*.\*)，然后找到我们需要的hex即可

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-4.png)

会弹出一个以 " Get FileType for ..." 为标题的窗口，直接单击OK按钮即可。

右键单击 Target1 ，选择 Options for Target ' Target 1 '

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-5.png)

接着会弹出如图的 Options 选项卡，选中 Debug 选项卡，在该页面中选中左上角的 Use Simulator ，然后去掉选项卡下方的 Load Application at Startup 复选框。接下来，单击 OK ，关闭选项卡即可。

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-6.png)

然后，选择 Debug -> Start/Stop Debug Session 或者按 Ctrl+F5 启动仿真调试，进入如图窗口

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-7.png)

在底部的 Command 窗口中，输入 load confused_arm.hex ，回车确认，然后单击左上角的RST按钮，PC指针就会回到 hex 文件的入口点

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-8.png)

下端点的方法为，在要下断电的位置前的灰色或绿色区域单击即可，调试界面的总体窗口的布局，左上为寄存器，右上为反汇编，左下为调试指令窗口，右下为堆栈/内存窗口。

寄存器区只有基本的内核寄存器，如果要查看 GPIO 或 USART 的寄存器状态，就要用到其中的 System Viewer 功能了，依次选择 Peripherals -> System Viewer 。

这里包含了所有外设的寄存器，可以选择某一个外设进行查看。针对 UART ，这里自带了一个将 USART 转化为 Terminal 的小工具。单击工具栏上的 Serial Windows 选择对应的 USART 即可。

运行即可get flag，但是我运行的时候出现了错误，左下角一直报：*** error 65: access violation at 0x40021000 : no 'write' permission 和类似的错

搜索可得是keil5将旧版本文件转换后出现的错误，需要进行一些配置，依然在 debug选项卡中，还需要改动两处。参考：https://blog.csdn.net/he__yuan/article/details/81712267

改完之后正常运行即可

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/SDK-使用-9.png)