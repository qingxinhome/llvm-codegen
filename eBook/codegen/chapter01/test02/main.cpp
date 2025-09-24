#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

using namespace llvm;

int main() {
    LLVMContext context;
    Module * module = new Module("example", context);
    IRBuilder<> builder(context);

    // 创建全局字符串常量
    Constant *str = ConstantDataArray::getString(context, "Welcome to llvm");
    GlobalVariable * global_greeting = new GlobalVariable(*module, str->getType(), true,
        GlobalValue::PrivateLinkage, str, "greeting");
    global_greeting->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);

    // 创建全局变量counter
    GlobalVariable * global_counter = new GlobalVariable(*module, Type::getInt32Ty(context), false,
        GlobalValue::ExternalLinkage, ConstantInt::get(Type::getInt32Ty(context), 0), "counter");

    // 声明puts函数， 使用PointerType::get
    FunctionType * putsTy = FunctionType::get(Type::getInt32Ty(context), {PointerType::get(context, 0)},  false);
    FunctionCallee puts = module->getOrInsertFunction("puts", putsTy);

    // 创建main函数
    FunctionType * mainTy = FunctionType::get(Type::getInt32Ty(context), {}, false);
    Function * main = Function::Create(mainTy, Function::ExternalLinkage, "main", module);
    BasicBlock * entry_block = BasicBlock::Create(context, "entry", main);
    builder.SetInsertPoint(entry_block);

    // 分配局部变量  %temp = alloca i32
    Value * temp = builder.CreateAlloca(Type::getInt32Ty(context), nullptr, "temp");
    // store i32 5, ptr %temp
    builder.CreateStore(ConstantInt::get(Type::getInt32Ty(context), 5), temp);

    // 加载 temp，执行 shl 和 add
    Value * load_temp = builder.CreateLoad(Type::getInt32Ty(context), temp); // %0 = load i32, ptr %temp
    Value * shifted = builder.CreateShl(load_temp, 3);  // %1 = shl i32 %0, 3
    Value * finalResult = builder.CreateAdd(shifted, ConstantInt::get(Type::getInt32Ty(context), 2), "final_result"); // %final_result = add i32 %1, 2

    // 存储到 counter
    builder.CreateStore(finalResult, global_counter);

    // 调用 puts
    builder.CreateCall(puts, {global_greeting});
    builder.CreateRet(finalResult);

    // 输出 IR
    module->print(outs(), nullptr);
    delete module;
}
