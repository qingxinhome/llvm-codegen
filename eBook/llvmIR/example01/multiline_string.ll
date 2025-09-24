; 定义跨行字符串常量
@long_str = private unnamed_addr constant [48 x i8] c"This is a long\0A
string spanning\0A
multiple lines\00"

; 声明 puts 函数
declare i32 @puts(ptr)

; 定义 main 函数
define i32 @main() {
entry:
  call i32 @puts(ptr @long_str)
  ret i32 0
}