## 练习1

1. 操作系统镜像文件ucore.img是如何一步一步生成的？(需要比较详细地解释Makefile中每一条相关命令和命令参数的含义，以及说明命令导致的结果)
2. 一个被系统认为是符合规范的硬盘主引导扇区的特征是什么？

### 题目1:

搭配答案以及参考链接可以比较清晰的得到答案，Makefile文件一下给我看懵了...（感谢zhz救我于水火）
然后进行简单的溯源及替代能够大概明白具体进行的操作

总结：

- 编译libs和kern下所有的.S和.c文件，得到.o文件，链接所有.o文件，得到bin/kernel文件
- 编译boot下所有.S和.c文件，得到.o文件，链接所有.o文件，得到bin/bootblock.out文件
- 编译生成sign文件
- 使用bin/sign工具将obj/bootblock.out转换生成512字节的bin/bootblock目标文件，并将bin/bootblock的最后两个字节设置为0x55AA
- 为bin/ucore.img分配5G空间，将bin/bootblock复制到ucore.img第一个block，将bin.kernel复制到ucore.img第二个block

### 题目2:

- 大小为512个字节
- 最后以0x55AA结尾



## 练习2

1. 从CPU加电后执行的第一条指令开始，单步跟踪BIOS的执行。
2. 在初始化位置0x7c00设置实地址断点,测试断点正常。
3. 从0x7c00开始跟踪代码运行,将单步跟踪反汇编得到的代码与bootasm.S和 bootblock.asm进行比较。
4. 自己找一个bootloader或内核中的代码位置，设置断点并进行测试。

### 题目1、2

需要修改gbdinit为

```
file bin/kernel
target remote :1234
set architecture i8086
b *0x7c00
continue
x /2i $pc
# break kern_init
# continue
```

注意不要手贱点QEMU的界面，会捕获鼠标和键盘，至少我的虚拟机完全拿不出来了

只能从外界强制重启

然后make debug后会出现乱码，使用ctrl+x+a退出tui模式即可

### 题目3、4

看起来差不多



## 练习3

1. 为何开启A20，以及如何开启A20
2. 如何初始化GDT表
3. 如何使能和进入保护模式

### 题目1

根据ucore实验手册的附录A”关于A20 Gate"总结可得

开启A20是为了能够在保护模式下正常访问超出1MB的内存且不会发生回卷，否则在保护模式下只能访问奇数位的内存

打开A20 Gate的具体步骤大致如下：

1. 等待8042 Input buffer为空；
2. 发送Write 8042 Output Port （P2）命令到8042 Input buffer；
3. 等待8042 Input buffer为空；
4. 将8042 Output Port（P2）得到字节的第2位置1，然后写入8042 Input buffer；

```
seta20.1:               # 等待8042键盘控制器不忙
	    inb $0x64, %al      # 
	    testb $0x2, %al     #
	    jnz seta20.1        #
	
	    movb $0xd1, %al     # 发送写8042输出端口的指令
	    outb %al, $0x64     #
	
	seta20.1:               # 等待8042键盘控制器不忙
	    inb $0x64, %al      # 
	    testb $0x2, %al     #
	    jnz seta20.1        #
	
	    movb $0xdf, %al     # 打开A20
	    outb %al, $0x60     # 
```

### 题目2

```
lgdt gdtdesc       //载入GDT表

gdtdesc:
    .word 0x17                                      # sizeof(gdt) - 1
    .long gdt                                       # address gdt
    
gdt:
    SEG_NULLASM                                     # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)           # code seg for bootloader and kernel
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel    
    
    #define SEG_ASM(type,base,lim)                                  \
    .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);          \
    .byte (((base) >> 16) & 0xff), (0x90 | (type)),             \
        (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)
```

### 题目3

将cr0的最低为设置为1

```
    movl %cr0, %eax
    orl $CR0_PE_ON, %eax
    movl %eax, %cr0
    
    .set CR0_PE_ON,             0x1                     # protected mode enable flag
```



## 练习4

1. bootloader如何读取硬盘扇区的？
2. bootloader是如何加载ELF格式的OS？

### 问题1

硬盘数据是储存到硬盘扇区中，一个扇区大小为512字节。读一个扇区的流程（可参看boot/bootmain.c中的readsect函数实现）大致如下：

1. 等待磁盘准备好
2. 发出读取扇区的命令
3. 等待磁盘准备好
4. 把磁盘扇区数据读到指定内存

```c
/* waitdisk - wait for disk ready */
static void
waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40)//不断取高两位进行匹配，最高两位变成01才返回
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void
readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();
    //outb()   I/O 上写入 8 位数据 ( 1 字节 )
  
    outb(0x1F2, 1);                         // 读取扇区数目为1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    //扇区起始编号32位，分4部分放在四个寄存器中
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4); // 幻数4因为这里以DW为单位
}
```

### 问题2

```C
/* bootmain - the entry of bootloader */
void
bootmain(void) {
    //读取头部
    // read the 1st page off disk
    readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

    //判断magic
    // is this a valid ELF?
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph, *eph;

    // 获取程序头表在在ELF文件结构中的偏移，获取程序头表表项的数量
    // 程序头表是一个结构数组，每一个数组元素就是一个表项（应该是有8项）
    // 因为可执行文件需要将.init节，.text节，.radata节，.data节，.bss节映射到存储空间的相应的段里面（代码段，数据段
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
      // 将ELF文件内数据载入内存
      // ELF文件0x1000位置后面的0xd1ec比特被载入内存0x00100000
	    // ELF文件0xf000位置后面的0x1d20比特被载入内存0x0010e000
    for (; ph < eph; ph ++) {
        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }

    // 根据ELF头部储存的入口信息，找到内核入口
    // 可执行目标文件中e_entry给出执行程序时第一条指令的地址（0x8048580)
    // 可重定位文件中e_entry为0
    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();

bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);
}
```



## 练习5

1. 我们需要在lab1中完成kdebug.c中函数print_stackframe的实现，可以通过函数print_stackframe来跟踪函数调用堆栈中记录的返回地址。

### 题目1

函数中的注释挺详细的

```c
uint32_t ebp = read_ebp(), eip = read_eip();
    int i, j;
    for(i = 0; ebp != 0 && i < STACKFRAME_DEPTH; i++){
        cprintf("ebp:0x%08x eip:0x%08x args:", ebp, eip);
        uint32_t *args = (uint32_t *) ebp + 2;//参数首地址
        for (j = 0; j < 4; j++) {
            cprintf("0x%08x", args[j]); //打印四个参数
        }
        cprintf("\n");
        print_debuginfo(eip - 1); //打印函数信息
        eip = ((uint32_t *)ebp)[1];
        ebp = ((uint32_t *)ebp)[0];
    }
```



## 练习6

1. 中断描述符表（也可简称为保护模式下的中断向量表）中一个表项占多少字节？其中哪几位代表中断处理代码的入口？
2. 请编程完善kern/trap/trap.c中对中断向量表进行初始化的函数idt_init。在idt_init函数中，依次对所有中断入口进行初始化。使用mmu.h中的SETGATE宏，填充idt数组内容。每个中断的入口由tools/vectors.c生成，使用trap.c中声明的vectors数组即可。
3. 请编程完善trap.c中的中断处理函数trap，在对时钟中断进行处理的部分填写trap函数中处理时钟中断的部分，使操作系统每遇到100次时钟中断后，调用print_ticks子程序，向屏幕上打印一行文字”100 ticks”。

### 题目1

表结构如下，一个表项占8个字节

```c
struct gatedesc {
    unsigned gd_off_15_0 : 16;        // low 16 bits of offset in segment
    unsigned gd_ss : 16;            // segment selector
    unsigned gd_args : 5;            // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;            // reserved(should be zero I guess)
    unsigned gd_type : 4;            // type(STS_{TG,IG32,TG32})
    unsigned gd_s : 1;                // must be 0 (system)
    unsigned gd_dpl : 2;            // descriptor(meaning new) privilege level
    unsigned gd_p : 1;                // Present
    unsigned gd_off_31_16 : 16;        // high bits of offset in segment
};
```

其中2-3字节是段选择子，0-1字节和6-7字节拼成位移，
两者联合便是中断处理程序的入口地址。

### 题目2

SETGATE函数

```
#define SETGATE(gate, istrap, sel, off, dpl) {            \
    (gate).gd_off_15_0 = (uint32_t)(off) & 0xffff;        \
    (gate).gd_ss = (sel);                                \
    (gate).gd_args = 0;                                    \
    (gate).gd_rsv1 = 0;                                    \
    (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;    \
    (gate).gd_s = 0;                                    \
    (gate).gd_dpl = (dpl);                                \
    (gate).gd_p = 1;                                    \
    (gate).gd_off_31_16 = (uint32_t)(off) >> 16;        \
}
```

```c
extern uintptr_t __vectors[]; //保存vextors.S中256个中断处理例程到入口地址数组
    int i;
    for (i = 0; i< sizeof(idt) / sizeof(struct gatedesc); i++){
        //第二个参数0代表中断门，第三个参数是中断处理例程的代码段GD_KTEXT，第四个参数是对应的偏移量，第五个参数是特权级
        SETGATE(idt[i], 0, GD_KTEXT, __vectors[i], DPL_KERNEL);
    }
    //用户态到内核态
    SETGATE(idt[T_SWITCH_TOK], 0, GD_KTEXT, __vectors[T_SWITCH_TOK], DPL_USER);
    //将中断门描述符表的起始地址装入IDTR寄存器中
    lidt（&idt_pd);
```

### 题目3

trap函数中调用的是trap_dispatch函数，所以在trap_dispatch函数中添加

```c
case IRQ_OFFSET + IRQ_TIMER:
    ticks ++;
    if (ticks % TICK_NUM == 0) {
            print_ticks();
    }
```



## 扩展练习

给我看懵了，对着别人的写的，我太菜了呜呜呜

```c
//lab1/kern/trap/trap.c
struct trapframe switchk2u, *switchu2k; //首先定义两个结构体指针
/* trap_dispatch - dispatch based on what type of trap occurred */
static void
trap_dispatch(struct trapframe *tf) {
    char c;

    switch (tf->tf_trapno) {
    case IRQ_OFFSET + IRQ_TIMER:
        /* LAB1 YOUR CODE : STEP 3 */
        /* handle the timer interrupt */
        /* (1) After a timer interrupt, you should record this event using a global variable (increase it), such as ticks in kern/driver/clock.c
         * (2) Every TICK_NUM cycle, you can print some info using a funciton, such as print_ticks().
         * (3) Too Simple? Yes, I think so!
         */
        ticks ++;
        if (ticks % TICK_NUM == 0) {
            print_ticks();
        }
        break;
    case IRQ_OFFSET + IRQ_COM1:
        c = cons_getc();
        cprintf("serial [%03d] %c\n", c, c);
        break;
    case IRQ_OFFSET + IRQ_KBD:
        c = cons_getc();
        cprintf("kbd [%03d] %c\n", c, c);
        break;
    //LAB1 CHALLENGE 1 : YOUR CODE you should modify below codes.
    case T_SWITCH_TOU:
            if (tf->tf_cs != USER_CS) { //判断是否为用户态CS
                switchk2u = *tf; //设置tf指针指向的trapframe结构体中的参数为用户的段寄存器
                switchk2u.tf_cs = USER_CS;
                switchk2u.tf_ds = switchk2u.tf_es = switchk2u.tf_ss = USER_DS;
                switchk2u.tf_esp = (uint32_t)tf + sizeof(struct trapframe) - 8; //设置esp的值
                switchk2u.tf_eflags |= FL_IOPL_MASK; //添加对IO的修改权限,可以在用户态模式下读写IO
                *((uint32_t *)tf - 1) = (uint32_t)&switchk2u; //切换栈空间,栈空间为设置的临时栈,iret返回时,会从switchk2u处开始恢复数据
            }
            break;
    case T_SWITCH_TOK:
            if (tf->tf_cs != KERNEL_CS){//判断是否为内核态CS
                tf->tf_cs = KERNEL_CS; //替换段寄存器为内核态的段寄存器
                tf->tf_ds = tf->tf_es = KERNEL_DS;
                tf->tf_eflags &= ~FL_IOPL_MASK; //取消FL_IOPL_MASK掩码,禁止了用户态对IO的读写
                switchu2k = (struct trapframe *)(tf->tf_esp - (sizeof(struct trapframe) - 8));
                //拷贝sizeof(struct trapframe) - 8大小的数据到switchu2k临时栈
                memmove(switchu2k, tf, sizeof(struct trapframe) - 8);
                *((uint32_t *)tf - 1) = (uint32_t)switchu2k;
            }
        break;
    case IRQ_OFFSET + IRQ_IDE1:
    case IRQ_OFFSET + IRQ_IDE2:
        /* do nothing */
        break;
    default:
        // in kernel, it must be a mistake
        if ((tf->tf_cs & 3) == 0) {
            print_trapframe(tf);
            panic("unexpected trap in kernel.\n");
        }
    }
}
```

```c
//对lab1/kern/init/init.c中 lab1_switch_to_user与lab1_switch_to_kernel 进行修改
//kern_init()中的对 lab1_switch_test(); 的注释取消
static void
lab1_switch_to_user(void) {
    //LAB1 CHALLENGE 1 : TODO
    //从中断返回时，会多pop两位，并用这两位的值更新ss、sp， 所以要先把栈压两位。
	asm volatile (
	    "sub $0x8, %%esp \n"
	    "int %0 \n"
	    "movl %%ebp, %%esp"
	    : 
	    : "i"(T_SWITCH_TOU)
	);
}

static void
lab1_switch_to_kernel(void) {
    //LAB1 CHALLENGE 1 :  TODO
	asm volatile (
	    "int %0 \n"
	    "movl %%ebp, %%esp \n"
	    : 
	    : "i"(T_SWITCH_TOK)
	);
}
```



参考：

https://github.com/tina2114/Sakura_University/tree/master/第十一至十二周/LAB%201

https://github.com/iTassel/Learning_From_Skr/blob/master/ucore_Kernel/lab1/ucore%20lab1%20HW.md

ucore实验参考答案
