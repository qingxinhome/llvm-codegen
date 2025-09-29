#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

using namespace llvm;


int main01() {
    // 默认构造
    StringRef empty;
    outs() << "Empty:" << (empty.empty() ? "true" : "false") << "\n";

    // 从C字符串构造
    StringRef hello("hello");
    outs() << "Data:" << hello.data() << ", size:" << hello.size() << "\n";

    // 从指针和长度构造
    const char buf[] = {'w', 'o', 'r', 'l', 'd'};
    StringRef world(buf, 5);
    outs() << "First char:" << world.front() << ", Last char:" << world.back() << "\n";

    // 从string构造
    std::string s = "llvm123456789";
    StringRef ref(s);
    outs() << "Substr:" << ref.substr(1, 1).data() << "\n";

    // 比较
    outs() << "Equals:" << (hello.equals(StringRef("hello")) ? "true": "false" )<< "\n";

    // 查找
    outs() << "Find 'l': " << hello.find('l') << "\n";
    return 0;
}

int main() {
    LLVMContext context;
    std::unique_ptr<Module> module = std::make_unique<Module>("my_module", context);
    IRBuilder<> builder(context);

    // 函数类型：int add(int, int)
    std::vector<Type*> paramTypes = {Type::getInt32Ty(context), Type::getInt32Ty(context)};
    FunctionType * funcType = FunctionType::get(Type::getInt32Ty(context), paramTypes, false);

    // 使用StringRef创建函数
    StringRef funcName("add");
    Function * addFunc = Function::Create(funcType, GlobalValue::LinkageTypes::ExternalLinkage, funcName, module.get());

    // 设置函数参数名称， 使用StringRef
    Function::arg_iterator args = addFunc->arg_begin();
    Value *arg1 = args++;
    arg1->setName(StringRef("a"));
    Value *arg2 = args++;
    arg2->setName(StringRef("b"));

    // 创建基本块， 使用StringRef
    StringRef bbName("entry");
    BasicBlock * bb = BasicBlock::Create(context, bbName, addFunc);
    builder.SetInsertPoint(bb);

    // 生成加法指令，使用 StringRef 命名
    StringRef sumName("sum");
    Value * sum = builder.CreateAdd(arg1, arg2, sumName);

    // 返回
    builder.CreateRet(sum);

    // 验证函数
    verifyFunction(*addFunc);

    // 输出IR
    module->print(outs(), nullptr);
    return 0;
}
