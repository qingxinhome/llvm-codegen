; 全局字符串常量
@greeting = private unnamed_addr constant [16 x i8] c"Welcome to LLVM\00"

; 全局变量
@counter = global i32 0

; 声明 puts 函数
declare i32 @puts(ptr)

; 定义 main 函数
define i32 @main() {
entry:
    ; 命名局部变量，分配栈空间
    %temp = alloca i32
    ; 存储局部变量 temp 值
    store i32 5, ptr %temp

    ; 无名局部变量，加载 temp
    %0 = load i32, ptr %temp
    ; 无名局部变量，计算 temp * 8
    %1 = shl i32 %0, 3

    ; 命名局部变量，存储结果
    %final_result = add i32 %1, 2

    ; 更新全局 counter
    store i32 %final_result, ptr @counter

    ; 打印greeting
    call i32 @puts(ptr @greeting)
    ret i32 %final_result
}
