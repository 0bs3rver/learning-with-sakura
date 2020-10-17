树莓派的使用经历～ 但是和漏洞调试相关的还是会放在漏洞那
## 启动与ssh

无显示器路由器连接参考：https://blog.csdn.net/qq_43038786/article/details/87983252

补充：这里我用的是ios，没找着查看个人热点连接的app，所以用了安卓备用机的热点，然后 ios 上下一个 Fing 软件然后连上去，即可查看树莓派的 ip 地址。

## 关机

先执行命令 `sudo poweroff` 再断电，不然会损坏 tf 卡。

## 安装工具

gdb插件-gef https://github.com/hugsy/gef

但是这里我的网很慢，下不了 py 文件，所以用了简单粗暴的复制粘贴大法。
