## 这是什么？

这是一份 skr_university 的学习记录

用以记录和鞭策自己在忙完学校与生活事情之余不要懈怠

由于时间比较久，所以前面是周报，后面是日报

# learning_list

<details>
<summary>2020.7.26: week 1 学习两部南京大学的计算机系统基础课</summary>

- [x] [南京大学计算机系统基础MOOC（一）](https://www.bilibili.com/video/BV1kE411X7S5) ：操作系统概述、运算与基础
- [x] [南京大学计算系系统基础MOOC（二）](https://www.bilibili.com/video/BV1rE41127Re) ：CPU基础知识
  </details>

<details>
<summary>2020.8.2: week 2 学习第三部南京大学的计算机系统基础课，写CSAPP前三个lab</summary>

- [x] [南京大学计算机系统基础MOOC（三）](https://www.bilibili.com/video/BV1jE411874k) ：进程、中断与其他

- [x] [CSAPP第三版全lab](http://csapp.cs.cmu.edu/3e/labs.html) ：与深入理解计算机系统相配套的实验
  - [data_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/data_lab) 主要是位运算的奇技淫巧
  - [bomb_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/bomb_lab) 简单的逆向题
  - [attack_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/attack_lab) 简单的pwn题
    </details>

<details>
<summary>2020.8.9: week 3 读CSAPP原书，写 Arch 和 Cache 两个lab</summary>

- [x] [CSAPP第三版全lab](http://csapp.cs.cmu.edu/3e/labs.html) ：与深入理解计算机系统相配套的实验
  - [arch_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/arch_lab) 学习流水线Y86-64处理器的设计和实现，同时对处理器和基准测试程序进行优化
  - [cache_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/cache_lab) 实现 cache，编写代码进行矩阵转置，尽可能的减少cache的miss次数
    </details>

<details>
<summary>2020.8.16: week 4 读CSAPP原书，写 Shell 和 Malloc 两个lab</summary>

- [x] [CSAPP第三版全lab](http://csapp.cs.cmu.edu/3e/labs.html) ：与深入理解计算机系统相配套的实验
  - [shell_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/shell_lab) 做一个完整的 shell 的功能
  - [malloc_lab](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/csapp_lab/malloc_lab) 实现动态内存申请器
    </details>

<details>
<summary>2020.8.23: week 5 听清华大学操作系统课程，写配套的 ucore lab1+拓展，写思考题</summary>

- [x] [ucore_OS](https://chyyuu.gitbooks.io/ucore_os_docs/content/) ：清华大学操作系统课程配套实验
  - [ucore_lab1](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/thu_ucore/lab1) 分析和实现bootloader

- [x] [思考题](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/%E6%80%9D%E8%80%83%E9%A2%98-10%E7%A7%8D%E6%95%B4%E6%95%B0%E6%BA%A2%E5%87%BA%E5%92%8C%E6%B5%AE%E7%82%B9%E7%B2%BE%E5%BA%A6%E5%AF%BC%E8%87%B4%E7%9A%84%E6%BC%8F%E6%B4%9E) ：十种整数溢出和浮点精度导致的漏洞
  </details>

<details>
<summary>2020.8.30: week 6 听清华大学操作系统课程，写配套的 ucore lab2、lab3</summary>

- [x] [ucore_OS](https://chyyuu.gitbooks.io/ucore_os_docs/content/) ：清华大学操作系统课程配套实验
  - [ucore_lab2](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/thu_ucore/lab2) 实现内存分配与页表相关
  - [ucore_lab3](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/thu_ucore/lab3) 地址映射与页面替换算法
    </details>

<details>
<summary>2020.9.6: week 6 听清华大学操作系统课程，写配套的 ucore lab4</summary>

- [x] [ucore_OS](https://chyyuu.gitbooks.io/ucore_os_docs/content/) ：清华大学操作系统课程配套实验
  - [ucore_lab4](https://github.com/0bs3rver/learning-with-sakura/tree/master/kernel/thu_ucore/lab4) 进程、线程与切换
    </details>

<details>
<summary>2020.9.13: week 7 看 googlectf_echo 相关的 socket 基础，注释源代码 </summary>

- [x] [googlectf2020_echo](https://github.com/0bs3rver/learning-with-sakura/tree/master/ctf/googlectf_echo) ：uaf 堆题漏洞
  </details>

<details>
<summary>2020.9.20: week 8 调试完毕 googlectf_echo ，调试 plaidctf_reee </summary>

- [x] [plaidctf2020_reee](https://github.com/0bs3rver/learning-with-sakura/tree/master/ctf/plaidctf2020_reee) ：自解密，花指令，gdb调试记得带参数
  </details>

<details>
<summary>2020.9.27: week 9 调试完毕 plaidctf_reee，搭建 iot 调试环境 </summary>

- [x] [iot调试环境笔记](https://github.com/0bs3rver/learning-with-sakura/tree/master/IOT)
- [x] [CTF特训营_stm32](https://github.com/0bs3rver/learning-with-sakura/tree/master/IOT/challenge/CTF%E7%89%B9%E8%AE%AD%E8%90%A5_stm32)
  </details>

<details>
<summary>2020.10.4: week 10 学 mdk 内容，看 debugger 原理，长亭视频 </summary>

- [x] [iot调试环境笔记](https://github.com/0bs3rver/learning-with-sakura/tree/master/IOT)
- [x] [debugger](https://github.com/0bs3rver/learning-with-sakura/tree/master/debugger)
- [x] [长亭hitcon演讲](https://www.bilibili.com/video/BV1gf4y1D7L2?t=19) 如何从0开始攻破 iot 设备
  </details>

<details>
<summary>2020.10.11: week 11 复现 sandybox，看操作系统真相还原到第三章，尝试复现vivotek </summary>

- [x] [plaidctf2020_sandybox](https://github.com/0bs3rver/learning-with-sakura/tree/master/ctf/plaidctf2020_sandybox) ：沙箱逃逸
  </details>

<details>
<summary>2020.10.12: day 78-80 搭建 raspberrypi 环境 </summary>

- [x] [raspberry启动](https://github.com/0bs3rver/learning-with-sakura/tree/master/IOT/raspberrypi) ：无显示器与键盘启动树莓派
  </details>

<details>
<summary>2020.10.15: day 81-84 复现完成 vivotek 远程栈溢出漏洞 </summary>

- [x] [vivotek摄像头漏洞](https://github.com/0bs3rver/learning-with-sakura/tree/master/IOT/challenge/vivotek%E8%BF%9C%E7%A8%8B%E6%A0%88%E6%BA%A2%E5%87%BA%E6%BC%8F%E6%B4%9E) ：远程 httpd 服务栈溢出
  </details>

<details>
<summary>2020.10.19: day 85 写 pwnable.tw start </summary>

- [x] [pwnable.tw_start](https://github.com/0bs3rver/pwnable.tw/tree/main/start-%E6%A0%88%E6%BA%A2%E5%87%BA%E6%89%A7%E8%A1%8Cshellcode) ：栈溢出执行shellcode
  </details>

<details>
<summary>2020.10.20: day 86-87 阅读 iot 基础知识与漏洞挖掘相关文章，写 pwnable.tw orw </summary>
  
- [x] [IOT安全实战资料收集整合](https://zybuluo.com/H4l0/note/1524758)
  - [iot入门知识与工具使用-持续更新版](https://github.com/0bs3rver/learning-with-sakura/blob/master/IOT/iot%E5%85%A5%E9%97%A8%E7%9F%A5%E8%AF%86%E5%8F%8A%E5%B7%A5%E5%85%B7%E4%BD%BF%E7%94%A8-%E6%8C%81%E7%BB%AD%E6%9B%B4%E6%96%B0%E7%89%88.md)
- [x] [pwnable.tw orw](https://github.com/0bs3rver/pwnable.tw/tree/main/orw-shellcode%E7%94%A8%E4%B8%AD%E6%96%AD%E8%AF%BB%E5%8F%96flag) ：shellcode 用中断读取 flag
</details>

<details>
<summary>2020.10.22: day 88 头疼，上午极度不适，勉强看了看 pwnable.tw 的 calc </summary>
</details>

<details>
<summary>2020.10.23: day 89 写完了 pwnable.tw 的 calc </summary>
  
- [x] [pwnable.tw_calc](https://github.com/0bs3rver/pwnable.tw/tree/main/calc-%E9%80%BB%E8%BE%91%E6%BC%8F%E6%B4%9E%E5%AE%9E%E7%8E%B0%E4%BB%BB%E6%84%8F%E5%9C%B0%E5%9D%80%E8%AF%BB%E5%86%99) ：逻辑漏洞实现任意地址读写
</details>

<details>
<summary>2020.10.24: day 90-91 打 byte ctf </summary>
</details>

<details>
<summary>2020.10.26: day 92 在写 pwnable.tw 的 3*17 顺便看了看 main 函数的启动流程 </summary>
  
- [x] [linux编程之 main 函数启动过程](https://blog.csdn.net/gary_ygl/article/details/8506007) 
</details>

<details>
<summary>2020.10.27: day 93 写完了 pwnable.tw 的 3x17 </summary>
  
- [x] [pwnable.tw_3x17](https://github.com/0bs3rver/pwnable.tw/tree/main/3x17-%E4%BB%BB%E6%84%8F%E5%9C%B0%E5%9D%80%E5%86%99%E6%9E%84%E9%80%A0ROP) ：任意地址写构造ROP
</details>

<details>
<summary>2020.10.28: day 94 写完了 pwnable.tw 的 dubblesort </summary>
  
- [x] [pwnable.tw_dubblesort](https://github.com/0bs3rver/pwnable.tw/tree/main/dubblesort-%E4%BF%9D%E6%8A%A4%E5%85%A8%E5%BC%80%E6%A0%88%E6%BA%A2%E5%87%BA) ：保护全开的栈溢出
</details>

<details>
<summary>2020.10.29: day 95 研究了一下小米摄像头，还没啥头绪 </summary>
</details>


<details>
<summary>2020.10.30: day 96 满课+补作业 </summary>
</details>

<details>
<summary>2020.10.31: day 97 尝试写 pwnable.tw 的 hacknote </summary>
  
- [ ] pwnable.tw_hacknote
</details>

<details>
<summary>2020.11.1: day 98 打铁人三项 </summary>
</details>

<details>
<summary>2020.11.2: day 99 写完了 pwnable.tw 的 hacknote </summary>

- [x] [pwnable.tw_hacknote](https://github.com/0bs3rver/pwnable.tw/tree/main/hacknote-%E7%94%A8main_arena%E6%B3%84%E6%BC%8Flibc) ：用main_arena泄漏libc
</details>

<details>
<summary>2020.11.3: day 100 没要到固件，自己拆摄像头把焊锡拽断了.... </summary>
</details>

<details>
<summary>2020.11.4: day 101 写完了 pwnable.tw 的 silver_bullet </summary>
  
- [x] [pwnable.tw_bullet-strncat](https://github.com/0bs3rver/pwnable.tw/tree/main/silver_bullet-strncat%E5%AE%9E%E7%8E%B0%E6%A0%88%E6%BA%A2%E5%87%BA) ：strncat实现栈溢出
</details>

<details>
<summary>2020.11.5: day 102 看 pwnable.tw 的 applestore 与补作业 </summary>
  
- [ ] pwnable.tw_applestore 弄懂利用思路
</details>

<details>
<summary>2020.11.6: day 103 试图用电烙铁焊锡，然后把自己给烫伤了... </summary>
</details>

<details>
<summary>2020.11.7: day 104 开始学习codeql </summary>
  
- [ ] CodeQL
</details>
 
<details>
<summary>2020.11.8: day 105 补作业 </summary>
</details>

<details>
<summary>2020.11.9: day 106 codeql有点推不动，先继续解决遗留下来的applestore，但是ubuntu炸了。惨惨 </summary>
  
- [ ] pwnable.tw_applestore 写到泄漏libc与heap基址
</details>


<details>
<summary>2020.11.10: day 107 接着推applestore，装py3的pwntools环境 </summary>
  
- [ ] pwnable.tw_applestore 写到如何利用delete的地址写
</details>

<details>
<summary>2020.11.11: day 108 写完了applestore，焊小米摄像头的板子 </summary>
  
- [x] [pwnable.tw_applestore](https://github.com/0bs3rver/pwnable.tw/tree/main/applestore-%E5%88%A9%E7%94%A8%E6%A0%88%E5%B9%B3%E8%A1%A1%E6%8E%A7%E5%88%B6%E5%86%85%E5%AD%98) ：利用栈平衡控制内存
- [ ] 焊板子大失败，决定用砂纸磨磨然后换个思路弄
</details>

<details>
<summary>2020.11.12: day 109 开始读《程序员的自我修养》并写读书笔记 </summary>
  
- [ ] 《程序员的自我修养》1.1 从Hello World开始
</details>

<details>
<summary>2020.11.13: day 110 完成了《程序员的自我修养》第一章读书笔记 </summary>
  
- [x] [《程序员的自我修养》第一章 温故而知新](https://github.com/0bs3rver/reading-notes/blob/main/%E7%A8%8B%E5%BA%8F%E5%91%98%E7%9A%84%E8%87%AA%E6%88%91%E4%BF%AE%E5%85%BB/%E7%AC%AC%E4%B8%80%E7%AB%A0%20%E6%B8%A9%E6%95%85%E8%80%8C%E7%9F%A5%E6%96%B0.md) ：操作系统做什么、内存如何分配与线程基础
</details>

<details>
<summary>2020.11.14: day 111 复现完成华为HG532命令注入漏洞 </summary>
  
- [x] [CVE-2017-17215-华为HG632](https://github.com/0bs3rver/learning-with-sakura/tree/master/IOT/challenge/%E5%8D%8E%E4%B8%BAHG532%E5%91%BD%E4%BB%A4%E6%B3%A8%E5%85%A5%E6%BC%8F%E6%B4%9E) ：upnp服务中未对语句进行过滤就拼接用system执行，从而可以通过命令注入完成任意命令执行
</details>

<details>
<summary>2020.11.15: day 112 体测，晚上看了会《程序员的自我修养》 </summary>
  
- [ ] 《程序员的自我修养》笔记写到了2.1
</details>

<details>
<summary>2020.11.16: day 113 写完了《程序员的自我修养》第二章，开始看pwnable.tw的realloc </summary>
  
- [x] [《程序员的自我修养》第二章 编译和链接](https://github.com/0bs3rver/reading-notes/blob/main/%E7%A8%8B%E5%BA%8F%E5%91%98%E7%9A%84%E8%87%AA%E6%88%91%E4%BF%AE%E5%85%BB/%E7%AC%AC%E4%BA%8C%E7%AB%A0%20%E7%BC%96%E8%AF%91%E5%92%8C%E9%93%BE%E6%8E%A5.md) ：预编译、汇编、编译与链接
- [ ] pwnable.tw realloc
</details>

<details>
<summary>2020.11.17: day 114 分析完了realloc，弄glibc环境 </summary>
  
- [ ] pwnable.tw realloc
</details>

<details>
<summary>2020.11.18: day 115 《程序员的自我修养》，写题目 </summary>
  
- [ ] 《程序员的自我修养》笔记写到了3.2
- [ ] 写笔试题，自己好菜啊...
</details>

<details>
<summary>2020.11.19: day 116 面试被问了，赶了篇shellcode的博客 </summary>
  
- [x] [如何编写一个shellcode](http://0bs3rver.space/2020/11/19/%E5%A6%82%E4%BD%95%E7%BC%96%E5%86%99%E4%B8%80%E4%B8%AAshellcode/) ：32与64位的shellcode编写
</details>

<details>
<summary>2020.11.20: day 117 写完了pwnable.tw 的 re-alloc </summary>
  
- [x] [pwnable.tw re-alloc](https://github.com/0bs3rver/pwnable.tw/tree/main/re-alloc-%E5%88%A9%E7%94%A8realloc%E5%AE%9E%E7%8E%B0uaf%E6%9B%B4%E6%94%B9got%E8%A1%A8) ：利用realloc实现uaf更改got表
</details>

<details>
<summary>2020.11.21: day 118 补了篇反弹shell的博客 </summary>
  
- [x] [如何构建一个反弹shell](http://0bs3rver.space/2020/11/22/%E5%A6%82%E4%BD%95%E6%9E%84%E5%BB%BA%E4%B8%80%E4%B8%AA%E5%8F%8D%E5%BC%B9shell/) ：构建反弹shell
</details>

<details>
<summary>2020.11.22: day 119 补作业 </summary>

</details>

<details>
<summary>2020.11.23: day 120 《程序员的自我修养》，写tcachetear，蓝牙键盘突然坏掉了...</summary>
  
- [ ] 《程序员的自我修养》笔记写到了3.3
- [x] 搞定蓝牙键盘
- [ ] 写pwnable.tw Tcache Tear
</details>

<details>
<summary>2020.11.24: day 121 接着写tcachetear，看glibc2.26新添加的tcache原理 </summary>
  
- [ ] 写pwnable.tw Tcache Tear
</details>

<details>
<summary>2020.11.25: day 122 继续看《程序员的自我修养》，面试 </summary>
  
- [ ] 《程序员的自我修养》笔记写到了3.4
- [ ] 面试
</details>

<details>
<summary>2020.11.26: day 123 写完了tcachetear </summary>
  
- [x] [pwnable.tw Tcache Tear](https://github.com/0bs3rver/pwnable.tw/tree/main/TcacheTear-tcache-dup%26%26house-of-spirit%26%26__free_hook) ：tcache-dup完成任意地址任意写，house-of-spirit泄漏libc基址，改写__free_hook从而劫持控制流
</details>


<details>
<summary>2020.11.27: day 124 开始写seethefile，学IO_FILE相关 </summary>
  
- [ ] pwnable.tw seethefile
</details>
