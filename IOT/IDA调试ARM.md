参考《CTF特训营》

## 手动寻找固件入口点

例子是confused_arm.hex

在Load a New File界面，Processor type要选择ARM Little-endian [ARM]，因为Intel hex格式的文件并不包含目标CPU的信息，所以最好自己指定

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-1.png)

确定后IDA会自动完成分析，Intel hex格式的程序中会包含基地址信息，可以识别到程序加载到的基地址是0x08000000

任何单片机在上电或者Reset的时候，都会事先进入Reset Handler去执行Reset代码

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-2.png)

在与硬件相关的程序中，一般都会定义一些中断向量的位置，Reset也是其中一个中断向量，在复位时，硬件会自动将PC设置为Reset的地址，而其他就是一些定时器、外设的中断向量位置，这个表就称为中断向量表。

分析以0x08000000位置开始的内容是中断向量表，那么其中就会有Reset向量的位置，看内存分布图

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-3.png)

可知第一个地址0x20000730是SRAM区域（静态随机存取存储器（Static Random-Access Memory）），Reset代码不可能在SRAM区域中，第二个地址是0x8000101，这个地址是所有地址里面最低的地址，最有可能是Reset向量的位置。

需要注意的是，这个地址的最低位数1，这也就暗示着实际地址为0x8000100+1，表示实际地址为0x8000100，且指令集为Thumb

所以需要先按ALT+G（蓝牙键盘上是opt+G）将T寄存器的值修改为1以将该段代码注释为CODE16，然后直接在0x8000100地址处按C将数据转换为指令即可

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-4.png)

可见IDA识别出了附近的函数调用，其中在loc_80000EC处设定了SP的值为0x20000730，即flash起始地址的值，表明了初始堆栈指针的位置。很明显，0x08000100地址确实是整个程序的入口点，即Reset向量的位置。

在该位置处的代码，设定了一系列初始操作（在sub_8000A00中），以及初始堆栈指针，最终跳至loc_8000188执行。

关键步骤总结：

1. 用IDA加载程序，选择正确的CPU
2. 找到中断向量表，寻找其中地址最小的指针
3. 跳至该指针处，若IDA未能正确识别代码，则手动将该处数据转为指令（对于ARM，需要自行确定该处是ARM还是THUMB指令集）

## 寄存器地址和SRAM地址的处理方法

继续分析，可以看到在函数sub_8000188()中有跳转到sub_80000f4()，然后跳转到sub_8001084()

猜测是main函数，跳转后查看伪码可以看到

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-5.png)

 有几个地方很显眼，是以0x40000000开头的地址和0x20000000开头的地址，从memory map中可以了解，前者是特殊功能寄存器的基地址，而后者是SRAM的起始地址

程序中频繁调用了这些寄存器和内存地址，但以这种格式显示，并不便于我们分析。

首先需要将操作数转为offset，需要添加一个对应地址段的segment，因为程序在加载时只识别了以0x08000000开始的segment，所以我们需要手动添加以0x40000000开头的segment，添加方法为：[Edit]-[Segments]-[Create segment...]

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-6.png)

其中，End address需要尽可能大，覆盖memory map中所关心的寄存器地址范围，但也不宜过大，不然IDA容易卡。按OK之后，segment就加上了，把鼠标放在0x40013800这个数上，按o键，数据就转换为对应的offset了。

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-7.png)

还可以更进一步将unk_40013800改为寄存器的名字，在memary map中查到是USART1的基址，所以这里可以将其名字改为USART1，按n即可。

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-8.png)

在SRAM的地址0x20000000添加segment的方法同理

## CPU高级选项

打开界面，选择ARM后，单击set使CPU设置生效，然后点击Processor options，这就是CPU的高级选项

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-9.png)

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-10.png)

然后继续点击Edit ARM architecture options

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-11.png)

Cortex M架构术语 ARMv7-M，而默认的CPU选项是any，即IDA会根据实际情况进行智能分析，如果我们这次直接选择ARMv7-M，然后进入到0x8000100处

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/IDA-ARM-12.png)

可以看到IDA已经正确识别出了入口点，并对指令做出了十分准确的分析