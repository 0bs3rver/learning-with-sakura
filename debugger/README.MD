ptrace() 系统调用可以被用来观察并控制另一进程的执行，并检查和更改程序的内存和寄存器，它主要被用于实现断点调试和系统调用跟踪。

原文代码逻辑是首先分析elf文件，将每个函数的第一个指令更改为断点

```C++
#define BKPT   0xCC
#define BKPT_MASK   0xFFFFFFFFFFFFFF00

void insert_brakepoints()//将每个函数的第一个指令更改为断点
{
    for(int i = 0; i < bp_count; ++i)
    {
        breakpoints[i].original_code = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)breakpoints[i].addr, 0);
        ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoints[i].addr, (breakpoints[i].original_code & BKPT_MASK) | BKPT); // insert breakpoint
    }
}
```

问：为什么这样可以插入断点？

因为0xCC对应的汇编指令为int3，是专门用来调试的中断指令

然后再去执行子进程，当遇到断点时获取信号打印函数名并恢复断点位置原有的指令，然后让子进程单步执行并再次放置断点。

```C++
 else
                {
                    printf("%s();\n", breakpoints[id].name);
                    regs.rip = breakpoints[id].addr;
                    ptrace(PTRACE_SETREGS, child_pid, 0, &regs);    // set rip back to good position
                    ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoints[id].addr, breakpoints[id].original_code); // return original instruction
                    ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0);     // step instruction
                    wait(NULL); // wait for singlestep
                    ptrace(PTRACE_POKETEXT, child_pid, (void*)breakpoints[id].addr, (breakpoints[id].original_code & BKPT_MASK) | BKPT); // insert breakpoint again
                }
```

问：为什么要设置第二次断点？

为了让再次调用该函数时能够断下来

最后输出结果

```
ctfer@ubuntu:~/Desktop$ ./tracer test1
PID 13119
Tracing started
===

_start();
__libc_csu_init();
_init();
frame_dummy();
register_tm_clones();
main();
func1();
func3();
func2();
func2();
func3();
__do_global_dtors_aux();
deregister_tm_clones();
_fini();
ABBChild finished
```

测试代码

```c
#include <stdio.h>

int func1()
{
    printf("A");
}

void func2()
{
    printf("B");
}

void func3()
{
}

int main()
{
    func1();
    func3();
    func2();
    func2();
    func3();
    return 0;
}
```



参考：

http://researchcomplete.blogspot.com/2016/08/on-subject-of-debuggers-and-tracers_5.html

https://omasko.github.io/2018/04/19/ptrace%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0I/
