## 1、意料外数据躲过边界检查

来自https://pdfs.semanticscholar.org/9254/27b24d25fce4f52b7986ad923877d817f3ab.pdf

Windows 2000

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/思考题-整数溢出-1.png)

## 2、整数下溢产生负数，从而产生未预料到的路径分支

来自：https://pdfs.semanticscholar.org/9254/27b24d25fce4f52b7986ad923877d817f3ab.pdf

Windows 2003 server

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/思考题-整数溢出-2.png)

## 3、存在漏洞的数据判断会导致运算产生溢出

来自https://pdfs.semanticscholar.org/9254/27b24d25fce4f52b7986ad923877d817f3ab.pdf

Baidu Hi CVE-2008-6444

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/思考题-整数溢出-3.png)

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/思考题-整数溢出-4.png)

## 4、没有检测不安全数据的范围引发可执行代码的漏洞

来自：http://jst.tsinghuajournals.com/CN/article/downloadArticleFile.do?attachType=PDF&id=152907

CVE-2013-1763

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/思考题-整数溢出-5.png)

## 5、将十进制数转化为浮点值时导致的运行错误

CVE-2010-4476

[Oracle](http://www.linuxidc.com/topicnews.aspx?tid=12) Java在处理畸形的数值时存在漏洞，远程攻击者可利用此漏洞造成以Java编写的应用程序挂起，造成拒绝服务。

在转换十进制数字2.2250738585072012e-308为双精度二进制浮点值时，Java的运行时和编辑器都进入到无限循环。此数字应转换为0x1p-1022即DBL_MIN，但是java陷入0x1p-1022和0x0.fffffffffffffp-1022之间的循环。

## 6、未进行数据完整性校验触发缓冲区溢出

来自：https://paper.seebug.org/586/

CVE-2017-0101

GDI子系统中，在函数 `EngRealizeBrush` 中引擎模拟实现笔刷绘制时，系统根据笔刷图案位图的大小以及目标设备表面的像素颜色格式计算应该分配的内存大小，但是没有进行必要的数值完整性校验，导致可能发生潜在的整数向上溢出的问题，致使实际上分配极小的内存块，随后函数对分配的 `ENGBRUSH` 对象成员域进行初始化。在整数溢出发生的情况下，如果分配的内存块大小小于 `ENGBRUSH` 类的大小，那么在初始化成员域的时候就可能触发缓冲区溢出漏洞，导致紧随其后的内存块中的数据被覆盖。

接下来函数调用 `SURFMEM::bCreateDIB` 分配临时的位图表面对象，并在其中对数值的有效性进行再次校验，判断数值是否大于 `0x7FFFFFFF`。但在此时校验的数值比分配的缓冲区大小数值小 `0x84`，因此如果实际分配的缓冲区是小于 `0x40` 字节的情况，那么在函数 `SURFMEM::bCreateDIB` 中校验的数值就将不符合函数 `SURFMEM::bCreateDIB` 的要求，导致调用失败，函数向上返回，并在上级函数中释放分配的 `ENGBRUSH` 对象。

在上级函数中在释放先前分配 `ENGBRUSH` 对象时，如果先前的成员域初始化操作破坏了位于同一内存页中的下一个内存块的 `POOL_HEADER` 结构，那么在释放内存时将会引发 `BAD_POOL_HEADER` 的异常。通过巧妙的内核池内存布局，使目标 `ENGBRUSH` 对象的内存块被分配在内存页的末尾，这样一来在释放内存块时将不会校验相邻内存块 `POOL_HEADER` 结构的完整性。

利用整数向上溢出导致后续的缓冲区溢出漏洞，使函数在初始化 `ENGBRUSH` 对象的成员域时，将原本写入 `ENGBRUSH` 对象的数据覆盖在下一内存页起始位置的位图表面 `SURFACE` 对象中，将成员域 `sizlBitmap.cy` 覆盖为 `0x6` 等像素位格式的枚举值，致使目标位图表面对象的可控范围发生改变。通过与位于同一内存页中紧随其后的内核 GDI 对象或下一内存页相同位置的位图表面对象相互配合，实现相对或任意内存地址的读写。

## 7、未验证传输包长度引起的整数溢出进而导致内存越界

来自：https://paper.seebug.org/1152/

微信视频通话

微信视频通话接通后，通话两端建立网络直连传递RTP报文。微信客户端传输RTP包过程中，采用了一套加密机制。但是微信客户端在RTP解密之前，没有很好验证RTP包长度。当攻击者发送很短的RTP包的时候，会引起接受端处理RTP包过程中长度计算的整数下溢出，进而导致内存越界访问。

## 8、客户端诱导导致的整数溢出漏洞

来自：https://paper.seebug.org/959/

linux内核 CVE-2019-11477

1. 客户端连接服务端（同时三次握手过程中强制设置接受mss最大值为8）
2. 客户端诱导服务端发送超长报文给客户端，贴近最大允许长度32k
3. 客户端不断发送重传要求，服务端重复发送17次报文填满skb分片队列，导致tcp_gso_segs变量整数溢出，导致服务器远程拒绝服务。

## 9、整数溢出产生负数分配空间失败，没有替换回原信息

来自：https://paper.seebug.org/240/

windows IE CVE-2014-6332

在获取完已分配数组的大小后，程序根据待分配数组的SAFEARRAYBOUND信息来修改SAFEARRAY指针指向的原SAFEARRAYBOUND信息，即其中的cElements和lLbound，以此来获取待分配数组的大小。但由于之后jge指令将新增空间大小0x842141d0当成了负数，即整数溢出，导致程序进入错误的处理分支，新空间会分配失败，但函数在返回前并没有将原先备份的SAFEARRAYBOUND信息替换回去，从而分配的数组空间没变cElements值却改变了，因此corrupt后的SAFEARRAY结构可被用于内存的越界访问。

## 10、可以扣除低于账户的资产导致的溢出漏洞

来自：https://paper.seebug.org/632/

https://medium.com/@ebanisadr/how-800k-evaporated-from-the-powh-coin-ponzi-scheme-overnight-1b025c33b530

CVE-2018-10299

在PoWH实施ERC-20的过程中发现了一个漏洞，该漏洞使一个人可以“批准”另一个用户代表他们转让令牌。恶意用户可能使第二帐户能够从第一帐户出售硬币。**但是，从第一个帐户中出售的硬币将从第二个帐户的余额中扣除**。结果（无符号）整数下溢将使第二个账户的PoWH硬币余额非常大。
