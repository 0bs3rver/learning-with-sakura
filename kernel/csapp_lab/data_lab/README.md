## Datalab

### bitxor

百度回忆了一下异或公式 a⊕b = (¬a ∧ b) ∨ (a ∧¬b)

```C
int bitXor(int x, int y) {
  return ~(~x & ~y)&(~(x&y));
}
```

### tmin

我思考了很久return minimum two's complement integer的 two‘s是什么意思

然后就去看师傅的笔记了

丢人.jpg

原来是要得到最小补码么....

int的最小值是符号位为1后面全0

```c
int tmin(void) {
  return 0x1 << 31;
}
```

### isTmax

int最大值是符号为0后面全为1

```C
int isTmax(int x) {
  int i = x+1;//Tmin,1000...
  x=x+i;//-1,1111...
  x=~x;//0,0000...
  i=!i;//exclude x=0xffff...
  x=x+i;//exclude x=0xffff...
  return !x;
}
```



### allOddBits

构造出奇数位全1的数 `mask` ，然后获取输入 `x` 值的奇数位，其他位清零（`mask&x`），然后与 `mask` 进行异或操作，若相同则最终结果为0，然后返回其值的逻辑非。

```c
int allOddBits(int x) {
  int mask = 0xAA+(0xAA<<8);
  mask=mask+(mask<<16);
  return !((mask&x)^mask);
}
```

### negate

按位取反再加一就好了

```c
int negate(int x) {
  return ~x + 1;
}
```

### isAsciiDigit

分开判断。高四位等于0x3，低四位不大于9

```c
int isAsciiDigit(int x) {
  return (!((x>>4)^0x3)) & (!((0x9+(~(x&0xF)+1))>>31));
}
```

### conditional

要通过位运算完成判断的效果

```c
int conditional(int x, int y, int z) {
  x = !!x;
  x = ~x+1;
  return (x&y)|(~x&z);
}
```

### isLessOrEqual

把比较转化为做减法，然后再对结果，x，y的符号位进行比较，考虑溢出情况

```c
int isLessOrEqual(int x, int y) {
    int x1 = (x>>31);
    int y1 = (y>>31);
    int z = (y+(~x+1))>>31;
  return (x1&(!y1))|(!(x1^y1)&!z);
}
```

### logicalNeg

执行！但是不能使用！

方法是把值全部集中在一位，最后对这一位进行比较

```c
int logicalNeg(int x) {
  x=(x>>16)|x;
  x=(x>>8)|x;
  x=(x>>4)|x;
  x=(x>>2)|x;
  x=(x>>1)|x;
  return ~x&0x1;
}
```

### howManyBits

对于正数只要找到最高位位置就可以了，但负数由于补码表示的原因不可以。并且负数要比正数多出1个符号位，所以我们先把负数转化为多一位的正数（值不重要），然后找出最高非0位返回位置就可以了。

```c
int howManyBits(int x) {
  int n = 0 ;
  x^=(x<<1);
  n +=  (!!( x & ((~0) << (n + 16)) )) << 4;
  n +=  (!!( x & ((~0) << (n + 8)) )) << 3;
  n +=  (!!( x & ((~0) << (n + 4)) )) << 2;
  n +=  (!!( x & ((~0) << (n + 2)) )) << 1;
  n +=  (!!( x & ((~0) << (n + 1)) ));
  return n+1;
}
```

### floatScale2

计算2*浮点数，把无穷小，0，无穷大，NaN四种情况后，阶码+1返回即可

```c
unsigned floatScale2(unsigned uf) {
  int exp = (uf&0x7f800000)>>23;
  int sign = uf&(1<<31);
  if(exp==0) return uf<<1|sign;
  if(exp==255) return uf;
  exp++;
  if(exp==255) return 0x7f800000|sign;
  return (exp<<23)|(uf&0x807fffff);
}
```

### floatFloat2Int

将float转化为int，还是先考虑特殊情况，然后分别取出各个地方，分开讨论即可

```c
int floatFloat2Int(unsigned uf) {
  int s_    = uf>>31;
  int exp_  = ((uf&0x7f800000)>>23)-127;
  int frac_ = (uf&0x007fffff)|0x00800000;
  if(!(uf&0x7fffffff)) return 0;

  if(exp_ > 31) return 0x80000000;
  if(exp_ < 0) return 0;

  if(exp_ > 23) frac_ <<= (exp_-23);
  else frac_ >>= (23-exp_);

  if(!((frac_>>31)^s_)) return frac_;
  else if(frac_>>31) return 0x80000000;
  else return ~frac_+1;
}
```

### floatPower2

求2.0的x次方

```c
unsigned floatPower2(int x) {
  int exp = x + 127;
  if (exp <= 0)
    return 0;
  if (exp >= 255)
    return 0x7f800000;
  return exp << 23;
}
```

