## 实验目的

练习1：实现 first-fit 连续物理内存分配算法

练习2：实现寻找虚拟地址对应的页表项

练习3：释放某虚地址所在的页并取消对应二级页表项的映射

## 知识整理

### 链表指针

```c
struct list_entry {
    struct list_entry *prev, *next;
};
```

用于双向链表指针的管理的结构

### 管理页的数据结构

每一个物理页的属性用结构Page来表示

```c
struct Page {
    int ref;        // page frame's reference counter
    uint32_t flags; // array of flags that describe the status of the page frame
    unsigned int property;// the num of free block, used in first fit pm manager
    list_entry_t page_link;// free list link
};
```

### 管理内存的数据结构

当内存页逐渐的分配与释放,残留的小的连续内存空间块由free_area_t数据结构管理

```c
/* free_area_t - maintains a doubly linked list to record free (unused) pages */
typedef struct {
            list_entry_t free_list;                                // the list header
            unsigned int nr_free;                                 // # of free pages in this free list
} free_area_t;
```

free_list: 管理双向链表的链表头 

nr_free: 当前空间页的个数

物理内存地址最大为maxpa,页大小为PGSIZE,那么需要管理的物理页数量为:

```c
npage = maxpa / PGSIZE
```

管理上述所有内存的Page结构所占空间为:

```c
sizeof(struct Page) * npage
```

将上述所有Page结构所占空间存放于 bootloader加载ucore的结束地址,以end指针作记录,页向上对齐后以pages指针作为作为存放的起始位置

```c
pages = (struct Page *)ROUNDUP((void *)end, PGSIZE);
```

[pages,sizeof(struct Page) * npage]之间作为上述所有Page结构存放的内存地址

故 所需管理的内存起始地址为

```c
uintptr_t freemem = PADDR((uintptr_t)pages + sizeof(struct Page) * npage); //PADDR函数用于获取一个内核的虚拟地址(地址在KERNBASE之上)
```

pmm_manager用于物理内存页管理的结构

```c
struct pmm_manager {
            const char *name; //物理内存页管理器的名字
            void (*init)(void); //初始化内存管理器
            void (*init_memmap)(struct Page *base, size_t n); //初始化管理空闲内存页的数据结构
            struct Page *(*alloc_pages)(size_t n); //分配n个物理内存页
            void (*free_pages)(struct Page *base, size_t n); //释放n个物理内存页
            size_t (*nr_free_pages)(void); //返回当前剩余的空闲页数
            void (*check)(void); //用于检测分配/释放实现是否正确的辅助函数
};
```

```c
如何在建立页表的过程中维护全局段描述符表(GDT)和页表的关系，确保ucore能够在各个时间段上都能正常寻址?
	在建立页表的时候，此时还是链接时由bootloader生成的GDT.在建立页表完成后，需要更新全局GDT.这才真正开启了分页机制.
对于哪些物理内存空间需要建立页映射关系?
	从ucore结束的地址开始 即 pages处
具体的页映射关系是什么?
	页目录 -> 页表 -> 页
页目录表的起始地址设置在哪里?
	#define VPT                 0xFAC00000
	pde_t * const vpd = (pde_t *)PGADDR(PDX(VPT), PDX(VPT), 0);
	vpd变量的值就是页目录表的起始虚地址0xFAFEB000
页表的起始地址设置在哪里,需要多大空间?
	此时描述内核虚拟空间的页目录表的虚地址为0xFAFEB000,大小为4KB.页表的理论连续虚拟地址空间0xFAC00000~0xFB000000,大小为4MB.因为这个连续地址空间的大小为4MB,可有1M个PTE,即可映射4GB的地址空间.
	又因为ucore只支持896MB的物理内存空间,故定义 #define KERNTOP (KERNBASE + KMEMSIZE)=0xF8000000 最大为0xF8000000的内核虚地址
	所以最大内核虚地址KERNTOP的页目录项虚地址为  vpd+0xF8000000>>22 = 0xFAFEB000+0x3E0=0xFAFEB3E0
	最大内核虚地址KERNTOP的页表项虚地址为:		 vpt+0xF8000000>>12 = 0xFAC00000+0xF8000=0xFACF8000
	
如何设置页目录表项的内容?
	PTE_U:	位3,表示用户态的软件可以读取对应地址的物理内存页内容
	PTE_W:	位2,表示物理内存页内容可写
	PTE_P:	位1，表示物理内存页存在
	页目录表项的末尾比特位可以表示对应页表的存在与否 以及 读写权限
如何设置页表项的内容?
	页表项的末尾比特位可以表示对应页的存在与否 以及 读写权限
```

### 二级页表

二级页表用来管理线性地址与物理地址间的映射关系

```c
假定当前物理内存0~16MB，每物理页（也称Page Frame）大小为4KB，则有4096个物理页，也就意味这有4个页目录项和4096个页表项需要设置。
一个页目录项(Page Directory Entry，PDE)和一个页表项(Page Table Entry，PTE)占4B。
即使是4个页目录项也需要一个完整的页目录表（占4KB）。而4096个页表项需要16KB（即4096*4B）的空间，也就是4个物理页，16KB的空间。
所以对16MB物理页建立一一映射的16MB虚拟页，需要5个物理页，即20KB的空间来形成二级页表。
```

## 练习1

```c
static void
default_init(void) {
    list_init(&free_list); // 创建初始空闲链表头
    nr_free = 0; // 空闲块总数初始化
}
```

```c
static void
default_init_memmap(struct Page *base, size_t n) {
    // 传进来的第一个参数是某个连续地址的空闲块的起始页
    // 第二个参数是页个数
    assert(n > 0);
    struct Page *p = base;
    for (; p != base + n; p ++) {
        assert(PageReserved(p)); // 判断此页是否为保留页
        p->flags = p->property = 0; // flag位与块内空闲页个数初始化
        set_page_ref(p, 0); // page->ref = val;
    }
    base->property = n;
    SetPageProperty(base); // 将其标记为已占有的物理内存空间
    nr_free += n;
    list_add(&free_list, &(base->page_link)); // 运用头插法将空闲块插入链表
}
```

```c
default_alloc_pages(size_t n) {
    // 边界情况检查
    assert(n > 0);
    if (n > nr_free) {
        return NULL;
    }
    struct Page *page = NULL;
    list_entry_t *le = &free_list;
    // 若list_next == &free_list代表该循环双向链表被查询完毕
    while ((le = list_next(le)) != &free_list) {
        struct Page *p = le2page(le, page_link) // 由list_entry_t结构转换为Page结构，找到该page结构的头地址
        if (p->property >= n) {
            page = p; // 如果该空闲块里面的空闲页个数满足要求，就找到了
            break;
        }
    }
    // 匹配空闲块成功后的处理
    if (page != NULL) {
        list_del(&(page->page_link)); //将此块取出
        // 如果空闲页个数比要求的多
        if (page->property > n) {
            struct Page *p = page + n;
            p->property = page->property - n;
            SetPageProperty(p); // 标为已使用块
            list_add(&(page->page_link), &(p->page_link));
    }
        list_del(&(page->page_link)); //从链表中删除
        nr_free -= n;
        ClearPageProperty(page);
    }
    return page;
}
```

```c
static void
default_free_pages(struct Page *base, size_t n) {
    assert(n > 0);
    struct Page *p = base;
    for (; p != base + n; p ++) {
        // 检测flag的bit 0是否是0，bit 1是否是0。即是否被保留，是否被free
        assert(!PageReserved(p) && !PageProperty(p));
        p->flags = 0;
        set_page_ref(p, 0); // 此页被引用次数清零
    }
    base->property = n;
    SetPageProperty(base); // 设置为保留页
    list_entry_t *le = list_next(&free_list);
    // 找到要free的页，将其合并
    while (le != &free_list) {
        p = le2page(le, page_link);
        le = list_next(le);
        // 这里是两种情况，检测前后向合并
        if (base + base->property == p) {
            base->property += p->property;
            ClearPageProperty(p);
            list_del(&(p->page_link));
        }
        else if (p + p->property == base) {
            p->property += base->property;
            ClearPageProperty(base);
            base = p;
            list_del(&(p->page_link));
        }
    }
    nr_free += n;
    le = list_next(&free_list);
    while (le != &free_list) //检测需释放的这段内存页 是否与空间内存页相邻
    {
        p = le2page(le, page_link);
        if (base + base->property <= p)
        {
            assert(base + base->property != p);
            break;
        }
        le = list_next(le);
    }
    list_add_before(&free_list, &(base->page_link)); 
}
```

## 练习2

题目中给出的参考：

- PDX(la)： 返回虚拟地址la的页目录索引

- KADDR(pa): 返回物理地址pa相关的内核虚拟地址

- set_page_ref(page,1): 设置此页被引用一次

- page2pa(page): 得到page管理的那一页的物理地址

- struct Page * alloc_page() : 分配一页出来

- memset(void * s, char c, size_t n) : 设置s指向地址的前面n个字节为字节‘c’

- PTE_P 0x001 表示物理内存页存在

- PTE_W 0x002 表示物理内存页内容可写

- PTE_U 0x004 表示可以读取对应地址的物理内存页内容

  ```c
  pte_t *
  get_pte(pde_t *pgdir, uintptr_t la, bool create) {
          pde_t *pdep = &pgdir[PDX(la)];  //尝试获得页表
          if (!(*pdep & PTE_P)) { //检测标志位，如果获取不成功
              struct Page *page;
              //假如不需要分配或是分配失败
              if (!create || (page = alloc_page()) == NULL) { 
                  return NULL;
          }
          set_page_ref(page, 1); //引用次数加一
          uintptr_t pa = page2pa(page);  //得到该页物理地址
          memset(KADDR(pa), 0, PGSIZE); //物理地址转虚拟地址，并初始化
          *pdep = pa | PTE_U | PTE_W | PTE_P; //修改页的权限 读/写/存在标志位,并填入页目录项
      }
      return &((pte_t *)KADDR(PDE_ADDR(*pdep)))[PTX(la)]; 
      //KADDR(PDE_ADDR(*pdep)):这部分是由页目录项地址得到关联的页表物理地址， 再转成虚拟地址
      //PTX(la)：返回虚拟地址la的页表项索引
      //最后返回的是虚拟地址la对应的页表项入口地址
  }
  ```

## 练习3

#### tlb_invalidate函数

```c
void
tlb_invalidate(pde_t *pgdir, uintptr_t la) {
    if (rcr3() == PADDR(pgdir)) { 
        invlpg((void *)la);
    }
}
```

#### page_ref_dec函数

```c
static inline int
page_ref_dec(struct Page *page) {
    page->ref -= 1; //引用数减一
    return page->ref;
}
```

#### page_remove_pte()

```c
static inline void
page_remove_pte(pde_t *pgdir, uintptr_t la, pte_t *ptep) {
    if (*ptep & PTE_P){ // 二级页表项存在
        struct Page *page = pte2page(*petp); //找到页表项
        if (page_ref_dec(page) == 0){ // 此页的被引用数为0，即无其他进程对此页进行引用
            free_pages(page);
        }
        *ptep = 0; // 该页目录项清零
        tlb_invalidate(pgdir,la); //当修改的页表是进程正在使用的那些页表，使之无效。
    }
```



参考：

https://github.com/tina2114/Sakura_University/tree/master/第十一至十二周/LAB%202

https://github.com/iTassel/Learning_From_Skr/blob/master/ucore_Kernel/lab2/ucore%20lab2%20HW.md