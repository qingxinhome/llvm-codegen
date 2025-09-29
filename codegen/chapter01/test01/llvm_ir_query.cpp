#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

int main() {
    // 初始化 Context 和 Module
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module(new llvm::Module("query_module", context));
    llvm::IRBuilder<> builder(context);

    // 定义函数: int process_row(int age)
    llvm::IntegerType * int32_type = llvm::Type::getInt32Ty(context);
    llvm::FunctionType * func_type = llvm::FunctionType::get(int32_type, {int32_type}, false);
    // 真正的module被智能指针包装了， module是unique_ptr智能指针
    llvm::Function * func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "process_row", module.get());

    // 创建基本块
    llvm::BasicBlock * entry = llvm::BasicBlock::Create(context, "entry", func);
    llvm::BasicBlock * then_bb = llvm::BasicBlock::Create(context, "then", func);
    llvm::BasicBlock * else_bb = llvm::BasicBlock::Create(context, "else", func);

    // 设置插入点
    builder.SetInsertPoint(entry);

    // 获取参数age
    llvm::Argument * arg_age = func->arg_begin();
    arg_age->setName("age");

    // 过滤: age > 30
    llvm::Value * cmp = builder.CreateICmpSGT(arg_age, builder.getInt32(30));
    builder.CreateCondBr(cmp, then_bb, else_bb);

    // Then 分支: 返回 age + 10
    builder.SetInsertPoint(then_bb);
    llvm::Value * result = builder.CreateAdd(arg_age, builder.getInt32(10), "result");
    builder.CreateRet(result);

    // Else 分支: 返回 0
    builder.SetInsertPoint(else_bb);
    builder.CreateRet(builder.getInt32(0));

    // 验证 Module
    if (llvm::verifyModule(*module, &llvm::errs())) {
        llvm::errs() << "Error verifying module!\n";
        return 1;
    }

    // 输出 IR
    module->print(llvm::outs(), nullptr);
    return 0;
}

/*
; ModuleID = 'query_module'
source_filename = "query_module"

define i32 @process_row(i32 %age) {
entry:
  %0 = icmp sgt i32 %age, 30
  br i1 %0, label %then, label %else

then:                                             ; preds = %entry
  %result = add i32 %age, 10
  ret i32 %result

else:                                             ; preds = %entry
  ret i32 0
}
 */