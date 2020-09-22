主要参考：

https://github.com/Hong5489/PlaidCTF2020/tree/master/reee

https://ctftime.org/writeup/20149

扔进ida看，发现是先对一块地方进行自解密然后再运行

可以使用gdb把内存dump出来，但是还是有很多花指令，没办法直接f5

所以就跟着下断点调试了

注意这个程序是以传参运行的形式对flag进行检测

所以用gdb调试的时候得带参数，例如下断点后 run lalalalala

而不是直接run到断点地方，不然会跑出段错误（

再就是./reee pctf{ok_nothing_too_fancy_there!} 进行测试的时候，如果bash无法识别！

使用set +H来关闭历史记录替换即可

解题脚本：

```python
from z3 import *
data = [0x48, 0x5f, 0x36, 0x35, 0x35, 0x25, 0x14, 0x2c,
        0x1d, 0x01, 0x03, 0x2d, 0x0c, 0x6f, 0x35, 0x61,
        0x7e, 0x34, 0x0a, 0x44, 0x24, 0x2c, 0x4a, 0x46,
        0x19, 0x59, 0x5b, 0x0e, 0x78, 0x74, 0x29, 0x13, 0x2c]

key = 0x50
length=33
s = Solver()
flag = []
for c in "pctf{": # Making use of the hint to set constraints
    flag.append(BitVecVal(ord(c), 8))
for i in range(length - 6):
    flag.append(BitVec("%d" % i, 8))
flag.append(BitVecVal(ord('}'), 8))

for i in range(0x539):
    for j in range(length):
        flag[j] = flag[j] ^ key
        key = flag[j] ^ key

for i in range(length):
    s.add(flag[i] == data[i])

assert s.check() == sat, "Can't find a solution"
m = s.model()
l = [(d, m[d]) for d in m]
l.sort(key=lambda x:int(str(x[0])))
print("pctf{", end="")
for i in range(len(flag) - 6):
    print(chr(int(str(l[i][1]))), end="")
print("}")
```

