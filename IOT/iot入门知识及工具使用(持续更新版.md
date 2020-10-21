[seebug-摄像头漏洞挖掘入门教程（固件篇）](https://paper.seebug.org/649/)

[固件分析技术入门知识](https://www.anquanke.com/post/id/85036)

[固件重打包及root shell获取](https://mp.weixin.qq.com/s/0ZU3aitKg-qxRh17dKd-_Q)

## 工具

### binwalk 

用于文件的切割，根据github上面的方法安装以安装依赖

在binwalk遇到大端序无法切割的情况可以先用DD截取出来之后使用objcopy转换为小端序再用binwalk提取

## DD