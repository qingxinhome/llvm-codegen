

### 一、LLVM IR 是什么

​	LLVM 的 IR 有 3 种形式： 汇编形式（可读的`.ll`文件）、 二进制形式（`.bc`文件）， 以及内存中的二进制形式。 这 3 种表示是等价的。 一般来说， IR 均是可读的汇编形式， 约定以 .ll 作为文件扩展名。

#### **1. 主要特点**

​	IR 是语言无关 （ language-independent） 和平台无关 （ target-independent， 或称目标无关） 的， 即不依赖于前端的高级语言以及后端平台， 可以说是一种低级平台无关语言。它介于高级编程语言（如C/C++、Rust）和特定硬件的目标机器代码之间，起到桥梁作用。 它的特性介绍如下。

-  类 RISC 精简指令集的三地址指令。
-  使用无限虚拟寄存器的 SSA 形式。
- 低层次控制流结构。
- Load / Store 指令使用类型指针。

- **平台无关**：IR不依赖于特定的硬件架构，同一个IR代码可以被编译到不同平台（如x86、ARM）。
- **SSA形式**：每个变量只有一次赋值，优化时更容易追踪数据流。
- **模块化**：IR以模块为单位，包含全局变量、函数声明和定义等。
- **强类型系统**：所有变量和操作都有明确的类型（如`i32`表示32位整数，`float`表示浮点数）。
- **可读性**：文本形式的IR类似于汇编语言，但更高级，易于理解和调试。

IR 语言中的变量可以是下面的类型：

1） 全局变量： 以@开头， 形如@var = common global i32 0， align 4

2） 局部变量： 以%开头， 形如%2 = load i32 ∗ %1， align 4

3） 函数参数： 形如 define i32 @fact （i32 %n）

### 2. **基本结构**

LLVM IR由以下主要部分组成：

- **模块（Module）**：IR的顶层容器，包含全局变量、函数和元数据。
- **函数（Function）**：包含参数、返回类型和基本块。
- **基本块（Basic Block）**：一组按顺序执行的指令，只有入口和出口。
- **指令（Instruction）**：如算术运算（`add`、`mul`）、内存操作（`load`、`store`）、控制流（`br`、`ret`）等。

LLVM 提供的优化工具可用于修改 IR。 优化工具可以读取 IR 输入文件 （包括汇编形式和二进制形式）， 然后按照用户指定的顺序对其执行特定 LLVM Pass。 命令行语法是：

```bash
opt < input.ir [arguments][pass-name]> output.ir
```

这个工具非常强大， 可以按照任意顺序执行 LLVM 的任意 Pass， 同时可以利用-stats 参数选项输出Pass 的统计信息； 用-time-passes 统计并输出每个 Pass 的执行时间。 例如， 在对 bar.ll 这个 IR 模块添加 O3 优化选项时， 经过了哪些 Pass， 以及这些 Pass 的执行时间是多少， 可以使用以下所示的命令

输出：

```bash
opt < bar.ll -O3 -stats-time-passes > bar-O3.ll
```



### 二、LLVM IR编译过程

#### Clang 示例

要解释使用 LLVM 后端的编译器整体架构， 就可以 C 语言编译器 Clang 为例。

在一个 x86_64 指令集的 `RHEL`系统上， 有一个最简单的 C 程序`test.c`：

```cpp
int main(){
    return 0;
}
```

使用

```bash
clang test.c -o test
```

究竟经历了哪几个步骤呢？ 接下来就介绍这些步骤。

#### 1.前端的语法分析

首先， Clang 的前端编译器会将这个 C 语言代码进行预处理、 词法分析、 语法分析、 语义分析，也就是常说的源代码解析。 这里不同语言会有不同的做法。 总之， 这里是将源码转化为内存中有意义的数据， 表示这个代码究竟想表达什么。

可以使用

```bash
clang -Xclang -ast-dump -fsyntax-only test.c
```

test.c经过编译器前端的预处理、 词法分析、 语法分析、 语义分析后， 生成 AST：

```c
TranslationUnitDecl 0xcad1068 <<invalid sloc>> <invalid sloc>
|-TypedefDecl 0xcad1898 <<invalid sloc>> <invalid sloc> implicit __int128_t '__int128'
| `-BuiltinType 0xcad1630 '__int128'
|-TypedefDecl 0xcad1908 <<invalid sloc>> <invalid sloc> implicit __uint128_t 'unsigned __int128'
| `-BuiltinType 0xcad1650 'unsigned __int128'
|-TypedefDecl 0xcad1c10 <<invalid sloc>> <invalid sloc> implicit __NSConstantString 'struct __NSConstantString_tag'
| `-RecordType 0xcad19e0 'struct __NSConstantString_tag'
|   `-Record 0xcad1960 '__NSConstantString_tag'
|-TypedefDecl 0xcad1cb8 <<invalid sloc>> <invalid sloc> implicit __builtin_ms_va_list 'char *'
| `-PointerType 0xcad1c70 'char *'
|   `-BuiltinType 0xcad1110 'char'
|-TypedefDecl 0xcad1fb0 <<invalid sloc>> <invalid sloc> implicit __builtin_va_list 'struct __va_list_tag[1]'
| `-ConstantArrayType 0xcad1f50 'struct __va_list_tag[1]' 1 
|   `-RecordType 0xcad1d90 'struct __va_list_tag'
|     `-Record 0xcad1d10 '__va_list_tag'
`-FunctionDecl 0xcb31938 <test.c:1:1, line:3:1> line:1:5 main 'int ()'
  `-CompoundStmt 0xcb31a58 <col:11, line:3:1>
    `-ReturnStmt 0xcb31a48 <line:2:5, col:12>
      `-IntegerLiteral 0xcb31a28 <col:12> 'int' 0
```

这一长串输出让人眼花缭乱， 然而， 只需要关注最后四行：

```c
`-FunctionDecl 0xcb31938 <test.c:1:1, line:3:1> line:1:5 main 'int ()'
  `-CompoundStmt 0xcb31a58 <col:11, line:3:1>
    `-ReturnStmt 0xcb31a48 <line:2:5, col:12>
      `-IntegerLiteral 0xcb31a28 <col:12> 'int' 0
```

​	这才是源码的 AST。 可以看出， 经过 Clang 前端的预处理、 词法分析、 语法分析、 语义分析后，代码被分析成一个函数， 其函数体是一个复合语句， 这个复合语句包含一个返回语句， 返回语句中使用了一个整型字面量 0。

​	因此， 基于 LLVM 的编译器的第一步， 就是将源码转化为内存中的 AST。



#### 2. LLVM前端生成中间代码

​	第二步， 就是**根据内存中的 AST 生成 LLVM 中间表示代码** （有些比较新的编译器还会先将AST 转化为 MLIR ， 再转化为 IR）。

​	**写编译器的最终目的， 这是将源码交给 LLVM 后端处理， 让 LLVM 后端进行优化， 并编译到相应的平台**。 而 LLVM 后端代码为提供的中间表示， 就是 LLVM IR。 只需要将内存中的 AST 转化为 LLVMIR， 就可以放手不管了， 接下来的所有事都由 LLVM 后端实现。

​	关于 LLVM IR， 下文会详细解释。 现在先看看将 AST 转化之后， 会产生什么样的 LLVM IR。 使用:

```bash
clang -S -emit-llvm test.c
```

这时， 会生成一个 test.ll文件， 其内容如下:

```c
; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.1.8 (https://github.com/llvm/llvm-project.git 3b5b5c1ec4a3095ab096dd780e84d7ab81f3d7ff)"}
```

这里看上去更加让人迷惑。 然而， 同样只需要关注以下6行内容：

```
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  ret i32 0
}
```

这是 AST 转化为 LLVM IR 的核心部分， 可以隐约感受到这个代码所表达的意思。



#### 3. LLVM 后端优化 IR

​	**LLVM 后端在读取了 IR 之后， 就会对这个 IR 进行优化**。 这在 LLVM 后端中是由 opt 这个组件完成的， 它会根据输入的 LLVM IR 和相应的优化等级， 进行相应的优化， 并输出对应的 LLVM IR。

可以用

```bash
opt test.ll -S --O3
```

对相应的代码进行优化， 也可以直接用

```bash
clang -S -emit-llvm -O3 test.c
```

优化， 并输出相应的优化结果：

```c
; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable
define dso_local noundef i32 @main() local_unnamed_addr #0 {
entry:
  ret i32 0
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 18.1.8 (https://github.com/llvm/llvm-project.git 3b5b5c1ec4a3095ab096dd780e84d7ab81f3d7ff)"}
```

​	观察@main 函数， 可以发现其函数体确实减少了很多内容。

​	但实际上， 上述这个优化代码只能通过 `clang -S -emit-llvm -O3 test.c` 生成。 如果对之前生成的 `test.ll`使用 `opt test.ll -S -O3`， 这是不会有变化的。 因为在 Clang 的修改中， 默认给所有 O0 优化级别的函数增加 optnone 属性， 所以会导致函数不被优化。 如果要使 `opt test.ll -S -O3`正确运行， 那么生成 test.ll时

​	需要使用以下命令。

```bash
clang -cc1 -disable-O0-optnone -S -emit-llvm test.c
```



#### 4. LLVM 后端生成汇编代码

**生成汇编代码**:

LLVM 后端优化的最后一步， 就是由 LLVM IR 生成汇编代码， 这是由 llc 这个组件完成的。

可以用：

```bash
llc test.ll
```

生成 test s， 其内容如下：

```assembly
        .text
        .file   "test.c"
        .globl  main                            # -- Begin function main
        .p2align        4, 0x90
        .type   main,@function
main:                                   # @main
        .cfi_startproc
# %bb.0:                                # %entry
        xorl    %eax, %eax
        retq
.Lfunc_end0:
        .size   main, .Lfunc_end0-main
        .cfi_endproc
                                        # -- End function
        .ident  "clang version 18.1.8 (https://github.com/llvm/llvm-project.git 3b5b5c1ec4a3095ab096dd780e84d7ab81f3d7ff)"
        .section        ".note.GNU-stack","",@progbits
```

这就回到了熟悉的汇编代码中。

有了汇编代码， 就需要调用操作系统自带的汇编器与链接器， 最终生成可执行程序。

#### 总结

根据上面讲的原理， 一个基于 LLVM 后端的编译器的整体使用过程是：

```bash
.c --frontend--> AST --frontend-->LLVM IR--LLVM opt-->LLVM IR--LLVM llc 
--> .s Assembly --OS Assembler--> .o --OS Linker--> executable
```

由此可见， **LLVM IR 是连接编译器前端与 LLVM 后端的一个桥梁。 同时， 整个 LLVM 后端也是围绕 LLVM IR 来运行的**。

那么， LLVM IR 究竟是什么呢？ 它的英文全称是 LLVM Intermediate Representation， 也就是 **LLVM的中间表示**。 事实上， LLVM IR 同时表示了三种内容：

- 内存中的 LLVM IR
-  比特码形式的 LLVM IR
-  可读形式的 LLVM IR

**内存中的 LLVM IR 是编译器最常接触的一种形式， 也是其最本质的形式**。 当在内存中处理 AST时， 需要根据当前的项， 生成对应的 LLVM IR， 这也就是编译器前端所做的事。 编译器前端API可以用许多语言编写， LLVM 也为许多语言提供了绑定(LLVM 前端接口提供了多种语言的实现C/C++/Python等)， 但其本身还是用 C++写的， 所以这里就以 C++为例。

**LLVM 的 C++接口在 llvm / IR 目录下提供了许多头文件， 如 llvm / IR/ Instructions h 等， 可以使用其中的值、 函数、 返回指令等成千上万的类来完成工作。 也就是说， 并不需要把 AST 变成一个个字符串， 如 ret i32 0 等， 而是需要将 AST 变成 LLVM 提供的API中 IR 类的实例(也就是用LLVM的Codegen代码描述IR )， 然后在内存中交给 LLVM 后端处理。**

**而比特码形式和可读形式， 则是将内存中的 LLVM IR 持久化的方法**。 比特码是采用特定格式的二进制序列。 而可读形式的 LLVM IR，则是采用特定格式的人可读的代码。

可以用以下所示命令：

```bash
clang -S -emit-llvm test.c
```

生成可读形式的 LLVM IR 文件 test.ll, 需要采用以下所示命令：

```bash
clang -c -emit-llvm test.c
```

生成比特码形式的 LLVM IR 文件 `test.bc`， 需要采用以下所示命令：

```bash
llvm-as test.ll
```

将比特码文件 test bc 转化为可读形式的 test ll。需要采用以下所示命令：

```bash
llvm-dis test.bc
```

