; 全局变量，名称包含特殊字符
@"my\22special\22var" = global i32 100

; 函数，使用转义名称的全局变量
define i32 @test_special() {
    ; 访问带转义字符的全局变量
    %1 = load i32, ptr @"my\22special\22var"
    %result =  add i32 %1, 50
    ret i32 %result
}
