; 定义全局字符串常量
@msg = private unnamed_addr constant [14 x i8] c"Hello, LLVM!\0A\00"

; 声明 puts 函数
declare i32 @puts(ptr)

; 定义 main 函数
define i32 @main() {
entry:
    ; 调用 puts 打印字符串
    call i32 @puts(ptr @msg)
    ret i32 0
}

; llvm-as string_constant.ll -o string_constant.bc
; llc -relocation-model=pic -filetype=obj identifier_example.bc -o string_constant.o
; clang identifier_example.o -o identifier_example\
; ./string_constant