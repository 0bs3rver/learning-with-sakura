## 实验目的

练习1：给未被映射的地址映射上物理页

练习2：补充完成基于FIFO的页面替换算法

## 知识整理

### 关键数据结构

#### vma_struct

在ucore中描述应用程序对虚拟内存“需求”的数据结构是vma_struct

```c
struct vma_struct {
    // 指向了一个mm_struct结构体,此结构体描述了同一页目录表内所有虚拟内存空间的属性
    struct mm_struct *vm_mm;
    uintptr_t vm_start; 	// 一个连续地址的虚拟内存空间的起始位置
    uintptr_t vm_end; 		// 一个连续地址的虚拟内存空间的结束位置
    uint32_t vm_flags; 		// 此虚拟内存空间的rwx属性
    // 由小到大将用vma_struct表示的虚拟内存空间双向链表连接
    list_entry_t list_link;
};
```

```c
//vm_flags表示了这个虚拟内存空间的属性
#define VM_READ 0x00000001 	//只读
#define VM_WRITE 0x00000002	//可读写
#define VM_EXEC 0x00000004	//可执行
```

#### mm_struct

vm_mm是一个指针，指向一个比vma_struct更高的抽象层次的数据结构mm_struct，这里把一个mm_struct结构的变量简称为mm变量。这个数据结构表示了包含所有虚拟内存空间的共同属性

```c
struct mm_struct {
    // 将同一页目录表的所有vma_struct结构体表示的虚拟内存空间的链表头
    list_entry_t mmap_list; //双向链表头，链接了所有属于同一页目录表的虚拟内存空间
    // 考虑局部性,此空间之后将会用到,避免虚拟内存空间的短时间内再次查询,提高效率
    struct vma_struct *mmap_cache; //指向当前正在使用的虚拟内存空间
    pde_t *pgdir; 	//指向的就是 mm_struct数据结构所维护的页表
    int map_count; 	//记录mmap_list里面链接的vma_struct的个数
    void *sm_priv; 	//  链接记录页访问情况的链表头*
};
```

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/ucore_lab3_1.png)

### 相关函数

```c
vma_struct:
	vma_create:			生成一个vma_struct,用于管理传入的虚拟内存空间范围与属性
    insert_vma_struct:	按照[vma->vm_start,vma->vm_end]由小到大插入到mm指针指向的mm_struct中mmap_list链表
    find_vma:			根据传入的参数地址address和mm指针,从mm结构体的mmap_list链表中查找包含此address的vma_struct
mm_struct:
	mm_create:			生成一个mm_struct结构体,由kmalloc分配一处空间
	mm_destroy:		   	删除一个mm_struct结构体,由kfree释放
```

```c
struct Page {  
……   
list_entry_t pra_page_link;   //构成一个按访问时间排序的链表（相当于视频中的栈）
uintptr_t pra_vaddr;   //记录此物理页对应的虚拟页起始地址。
};

// 为了实现各种页替换算法，设计了一个页替换算法的类框架swap_manager:
struct swap_manager
{
     const char *name;
     /* Global initialization for the swap manager */
     int (*init)            (void);
     /* 在mm_struct中初始化priv数据 */
     int (*init_mm)         (struct mm_struct *mm);
     /* tick中断发生时调用 */
     int (*tick_event)      (struct mm_struct *mm);
     /* 将可交换页面映射到mm_struct时调用 */
     int (*map_swappable)   (struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in);
     /* 当页面标记为共享时，将调用此例程以从交换管理器中删除addr条目 */
     int (*set_unswappable) (struct mm_struct *mm, uintptr_t addr);
     /* 挑选要换出的页 */
     int (*swap_out_victim) (struct mm_struct *mm, struct Page **ptr_page, int in_tick);
     /* check the page relpacement algorithm */
     int (*check_swap)(void);     
};
//这里关键的两个函数指针是map_swappable和swap_out_vistim，前一个函数用于记录页访问情况相关属性，后一个函数用于挑选需要换出的页。显然第二个函数依赖于第一个函数记录的页访问情况。tick_event函数指针也很重要，结合定时产生的中断，可以实现一种积极的换页策略。
}; 
```

### Page Fault异常处理

#### 原因

1. 页表项为0,不存在映射关系
2. 相应物理页帧不在内存中(页表项非0,但页存在,swap分区 或者 磁盘中)
3. 无访问权限(Present为1,但低权限访问高权限内存空间 OR 程序试图写属性只读的页)

页访问异常错误码有32位.位0为１表示对应物理页不存在;位１为１表示写异常(比如写了只读页;位２为１表示访问权限异常(比如用户态程序访问内核空间的数据))

#### errorCode

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/ucore_lab3_2.png)

```
EXT External event (bit 0):			值为 1 时表示硬件外部中断.
IDT Descriptor location (bit 1):	置为 1 时表示errorcode 的索引部分引用的是 IDT 的一个门描述符,置为 0 时表示引用 GDT 或者当前 LDT 的描述符.
TI (bit 2):							只在 IDT 位为 0 时有用.此时 TI 表示errorcode 的索引部分是 LDT,为 1 是是 GDT.
```

#### Page Fault errorCode

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/ucore_lab3_3.png)

1. 产生异常的线性地址保存在CR2寄存器,进入内核态,压入EFLAGS,CS,EIP,errorCode,以保存当前中断的程序现场
2. CPU将中断号0xE对应的中断服务例程的地址加载到CS和EIP寄存器中,开始执行中断服务例程
3. ucore开始处理异常中断,保存硬件没有保存的寄存器
4. 在vectors.S中vector14处将中断号压入内核栈，然后在trapentry.S中的标号__alltraps处把DS、ES和其他通用寄存器都压栈
5. 在trap.c的trap函数开始了中断服务例程的处理流程
   调用顺序为:trap--> trap_dispatch-->pgfault_handler-->do_pgfault[主要函数]

#### CR2寄存器

![](https://space.0bs3rver.workers.dev/0bs3rver/Picture/master//blogimg/ucore_lab3_4.png)

#### do_pgfault中对异常的处理

根据CR2中的物理地址以及errorCode的错误类型查找此地址是否在某vma结构描述范围内,以及内存权限是否正确

   如果全符合,则是合法访问,但没有建立映射,则分配一个空闲的内存页,修改页表完成虚地址到物理地址的映射,刷新TLB,然后调用iret中断,返回到产生页访问异常的指令处重新执行此指令.
   如果该虚地址不在某vma结构体描述的范围内,则判定为非法访问.

### 页替换算法

#### 先进先出(First In First Out, FIFO)页替换算法

```
算法总是淘汰最先进入内存的页,在内存中驻留时间最久的页予以淘汰,将调入内存的页以先后形成一个队列的结构
	缺点:适合程序以线性顺序访问地址空间效果才好,否则效率不高
		在增加放置页的页帧的情况下，反而使页访问异常次数增多
```

#### 时钟(Clock)页替换算法

```
时钟页替换算法把各个页面组织成环形链表的形式,类似于一个钟的表面,指针指向最先进入的内存页
当该页被访问时,CPU中的MMU硬件将把访问位置"1"
当操作系统需要淘汰页时,对当前指针指向的页所对应的页表项进行查询
	如果访问位为"0",则淘汰此页,如果此页被写过,则换到磁盘中
	如果访问位为"0",则将页表项的此位 置"0",继续访问下一页
与FIFO算法类似,但是跳过了访问位为1的页
```

#### 改进的时钟(Enhanced Clock)页替换算法

```
在时钟算法的基础上,加上了一位修改位,用于表示页是否被写过
	(0,0) 优先淘汰,未修改也未访问
	(0,1) 其次淘汰,未访问但被修改
	(1,0) 再次选择,被修改但未访问
	(1,1) 最后选择,最近使用且修改
此算法与时钟算法相比,进一步减少了磁盘的I/O操作次数,但为了寻找到可淘汰的页,需多次进行扫描,降低了算法的执行效率
```

#### 虚拟内存中页与硬盘中扇区的对应关系

```
swap_entry_t
-------------------------
| offset | reserved | 0 |
-------------------------
24 bits   7 bits    1 bit
bit 0  :	如果页被置换到硬盘中,最低位present为0,即 PTE_P标记为0,表示虚实地址映射关系不存在
bit 1-7:	暂时保留,之后用于各种扩展
bit 8-24:	表示此页在硬盘上的起始扇区的位置[扇区index索引]
为了区别页表项中0 与 swap分区的映射,将swap分区中的索引为0的扇区空出来,则页表项中高24位不为0,则两者即可区分
当程序运行中访问内存产生page fault异常时,如何判定这个引起异常的虚拟地址内存访问是越界、写只读页的"非法地址"访问还是由于数据被临时换出到磁盘上或还没有分配内存的“合法地址”访问?
	将异常的结果储存在Page Fault errorCode中,其中的bit 0 1 2可表示异常原因
何时进行请求调页/页换入换出处理?
	当访问的异常地址属于某个vma描述的范围内,但保存在swap分区中,则是执行页换入的时机,调用swap_in函数完成页面换入
	换出分积极换出策略和消极换出策略
		积极换出策略:	指操作系统周期性地(或在系统不忙的时候)主动把某些认为“不常用”的页换出到硬盘上,确保系统中总有一定数量的空闲页存在
		消极换出策略：只是当试图得到空闲页时,发现当前没有空闲的物理页可供分配,才会将不常用的页换出到硬盘
```

## 练习1

处理流程：

- 根据从CPU的控制寄存器CR2中获取的页访问异常的物理地址以及根据errorCode的错误类型来查找此地址是否在某个VMA的地址范围内以及是否满足正确的读写权限
- 如果该虚地址不在某VMA范围内，则认为是一次非法访问
- 如果在此范围内且权限也正确，则认为是一次合法访问，但没有建立虚实对应关系。所以会分配一个空闲的内存页，并且修改页表完成虚地址到物理地址的映射，刷新TLB，然后调用iret中断，返回到页访问异常的指令处重写执行该指令

```c
// 获取页表项，但找不到虚拟地址所对应的页表项
    if ((ptep = get_pte(mm->pgdir,addr,1)) == NULL){
        cprintf("get_pte in do_pgfault failed\n");
        goto failed;
    }

    // 页表项为0，不存在映射关系，则要建立虚拟地址和物理地址的映射关系
    if (*ptep == 0){
        // 权限不够,失败
        // Present为1,但低权限访问高权限内存空间 OR 程序试图写属性只读的页
        if (pgdir_alloc_page(mm->pgdir,addr,perm) == NULL){
            cprintf("pgdir_alloc_page in do_pgfault failed");
            goto failed;
        }
    }
    else {
        // 页表项非空，尝试换入页面
        if (swap_init_ok){
            struct Page *page = NULL; // 根据mm结构和addr地址，尝试将硬盘中的内容换入至page中
            if ((ret = swap_in(mm,addr,&page)) != 0){
                cprintf("swap_in in do_pgfault failed\n");
                goto failed;
            }
            page_insert(mm->pgdir,page,addr,perm); //建立虚拟地址和物理地址之间的对应关系
            swap_map_swappable(mm,addr,page,1); //将此页面设置为可交换的
            page->pra_vaddr = addr;
        }
        else{
            cprintf("no swap_init_ok but ptep is %x,failed\n",*ptep);
            goto failed;
        }
    }
```

## 练习2

先进先出(First In First Out, FIFO)页替换算法：

该算法总是淘汰最先进入内存的页，即选择在内存中驻留时间最久的页予以淘汰。只需把一个应用程序在执行过程中已调入内存的页按先后次序链接成一个队列，队列头指向内存中驻留时间最久的页，队列尾指向最近被调入内存的页。这样需要淘汰页时，从队列头很容易查找到需要淘汰的页。

FIFO算法只是在应用程序按线性顺序访问地址空间时效果才好，否则效率不高。因为那些常被访问的页，往往在内存中也停留得最久，结果它们因变“老”而不得不被置换出去。FIFO算法的另一个缺点是，它有一种异常现象（Belady现象），即在增加放置页的页帧的情况下，反而使页访问异常次数增多。

```c
// FIFO初始化
static int
_fifo_init_mm(struct mm_struct *mm)
{     
     list_init(&pra_list_head);//先将按访问时间排序的链表进行初始化
     mm->sm_priv = &pra_list_head;//把mm变量指向用来链接记录页访问情况的属性指向该链表
     //cprintf(" mm->sm_priv %x in fifo_init_mm\n",mm->sm_priv);
     return 0;
}

//将最近被用到的页面添加到算法所维护的次序队列。
static int
_fifo_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    list_entry_t *head=(list_entry_t*) mm->sm_priv;//获取页访问情况的链表头
    list_entry_t *entry=&(page->pra_page_link);//获取最近被使用到的页面
    assert(entry != NULL && head != NULL);
    list_add(head, entry);//头插，将最近被用到的页面添加到记录页访问情况的链表

    return 0;
}

//查询哪个页面需要被换出。
static int
_fifo_swap_out_victim(struct mm_struct *mm, struct Page ** ptr_page, int in_tick)
{
     list_entry_t *head=(list_entry_t*) mm->sm_priv;//获取按访问时间排序的链表
         assert(head != NULL);
     assert(in_tick==0);
     list_entry_t *le = head->prev; //找到要被换出的页（即链表尾，找的是第一次访问时间最远的页）
     assert(head != le);
     struct Page *p = le2page(le,pra_page_link); //找到page结构的head
     list_del(le); //将进来最早的页面从队列中删除
     assert (p != NULL);
     *ptr_page = p;

     return 0;
}
```





参考：

https://github.com/tina2114/Sakura_University/tree/master/第十一至十二周/LAB%203

https://github.com/iTassel/Learning_From_Skr/tree/master/ucore_Kernel/lab3