参考：https://github.com/tina2114/Sakura_University/tree/master/%E5%A4%96%E5%8D%A1%E8%B5%9B/PlaidCTF%202020/sandybox

https://ctftime.org/writeup/20115

师傅们说的挺好，没什么要补充的，需要注意的可能是这里：

接着就是来讲述漏洞点，先来看这个

```C
// simplified main tracer loop
while (1) {
    // wait for a syscall entry
    ptrace(PTRACE_SYSCALL, child_pid, 0); // 继续执行子进程，使得子进程在每次进行系统调用及结束一次系统调用时都会被内核停下来，此处是子进程进行系统调用
    waitpid(child_pid, &status, __WALL); // 等待获取子进程的信号
    
    ptrace(PTRACE_GETREGS, child_pid, 0, regs); // 获取寄存器的值
    if (check_syscall(child_pid, regs)) {
        // ALLOW SYSCALL
    } else {
        // BLOCK SYSCALL
    }

    // wait for a syscall exit
    ptrace(PTRACE_SYSCALL, child_pid, 0, 0); // 继续执行子进程，直到子进程结束一次系统调用，然后被暂停
    waitpid(child_pid, &status, __WALL); // 继续获取子进程的信号
}
```

如果我们有办法将其顺序颠倒呢？

```C
// simplified main tracer loop
while (1) {
    // wait for a syscall exit   注意此处，和上面不同
    ptrace(PTRACE_SYSCALL, child_pid, 0); // 继续执行子进程，使得子进程在每次进行系统调用及结束一次系统调用时都会被内核停下来，此处是子进程结束系统调用
    waitpid(child_pid, &status, __WALL); // 等待获取子进程的信号
    
    ptrace(PTRACE_GETREGS, child_pid, 0, regs); // 获取寄存器的值
    if (check_syscall(child_pid, regs)) {
        // ALLOW SYSCALL
    } else {
        // BLOCK SYSCALL
    }

    // wait for a syscall entry  注意此处，和上面不同
    ptrace(PTRACE_SYSCALL, child_pid, 0, 0); // 继续执行子进程，直到子进程开始一次系统调用，然后被暂停
    waitpid(child_pid, &status, __WALL); // 继续获取子进程的信号
}
```



这里漏洞利用说的 和上面不同 指的就是注释，而不是代码...不要和我一样找半天不同

并不是说我们可以把代码颠倒更改，而是两次处理信号的方式是一样的，所以我们可以通过 int 3信号不像其他系统调用，会有两次信号发出（开始和结束），它只会发出一次信号这个特点来，颠倒它处理的信号，从而绕过父进程tracer设置的syscall过滤器。

