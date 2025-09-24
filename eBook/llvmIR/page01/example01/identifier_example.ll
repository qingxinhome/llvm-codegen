; 全局变量（命名全局标识符）
@my_golbal = global i32 42

; 函数声明（命名全局标识符）
declare i32 @puts(ptr)

; 函数定义（命名全局标识符）
define i32 @main() {
entry:
    ; 命名局部变量
    %my_var = alloca i32
    store i32 10, ptr %my_var

    ; 无名局部变量（自动编号为 %0）
    %0 = load i32, ptr %my_var
    ; 无名局部变量（编号为 %1）
    %1 = add i32 %0, 8

    ; 命名局部变量（显式命名）
    %result = shl i32 %1, 3 ;左移3位，等价于 *8

    ; 调用 puts，传递全局字符串
    call i32 @puts(ptr @.str)
    ret i32 %result
}

; 全局字符串常量（命名全局标识符）
@.str = private unnamed_addr constant [12 x i8] c"Hello World\00"


; 步骤 1：验证 LLVM IR 语法
; 将 identifier_example.ll 转换为字节码（.bc）：
; llvm-as identifier_example.ll -o identifier_example.bc
; 说明：llvm-as 将 .ll 文件汇编为 LLVM 位码（bitcode），检查语法是否正确。

; 步骤 2：生成支持 PIE 的目标文件
; 使用 llc 生成目标文件，并明确指定位置无关代码（PIC）：
; llc -relocation-model=pic -filetype=obj identifier_example.bc -o identifier_example.o
; 关键选项：
; -relocation-model=pic：告诉 llc 生成位置无关代码，兼容 PIE。
; -filetype=obj：直接生成目标文件（.o），跳过汇编代码（.s）。

; 步骤 3：链接生成可执行文件
; 使用 clang 链接目标文件：
; clang identifier_example.o -o identifier_example
; 说明：
; clang 自动链接标准 C 库（libc），因为 identifier_example.ll 调用了 puts。
; 由于 identifier_example.o 现在是 PIC 兼容的，链接器不会报 R_X86_64_32 错误。

; 步骤 4：运行生成的可执行文件：
; ./identifier_example