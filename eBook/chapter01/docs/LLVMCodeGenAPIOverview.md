### LLVM CodeGen API 概览与使用指南

LLVM CodeGen API 是 LLVM 框架中用于生成IR 和执行代码生成的核心工具集，主要通过 C++ API 实现。以下详细介绍 LLVM CodeGen API 的主要组件、功能及其使用方法，重点聚焦于如何使用 `IRBuilder` 和 `Module` 生成 IR。内容包括 API 列表、用法说明、C++ 示例代码和实践建议，确保您理解核心 API 并能实际应用。假设您已安装 LLVM（建议版本 18+）并配置好环境（如通过 `cmake` 和 `llvm-config`）。

---

#### 1. LLVM CodeGen API 主要组件
LLVM CodeGen API 主要围绕 IR 生成和编译，核心类和工具如下。这些 API 位于 <llvm/IR/*> 头文件中，用于构建和操作 IR，最终生成机器码。

- **LLVMContext** (`<llvm/IR/LLVMContext.h>`)：
  - 功能：管理 LLVM 的全局状态（如类型系统、常量），确保线程安全。所有 IR 对象（如 Module、Function）必须绑定到一个 Context。
  - 主要方法：
    - `LLVMContext()`：构造上下文。
    - 用途：作为 IR 生成的根对象。
  - 用法：llvm::LLVMContext context; 是生成 IR 的第一步
  - 注意：Context 生命周期贯穿整个程序，需保持有效。
- **Module** (`<llvm/IR/Module.h>`)：
  - 功能：IR 的顶级容器，存储函数、全局变量、元数据等，类似一个编译单元（.c 文件）。添加函数（getOrInsertFunction）、全局变量（getOrInsertGlobal），并作为 IR 输出或编译的入口。
  - 主要方法：
    - `Module(StringRef Name, LLVMContext &C)`：创建 Module，指定名称和 Context。
    - `getOrInsertFunction(StringRef Name, FunctionType *Ty)`：添加或获取函数。
    - `getOrInsertGlobal(StringRef Name, Type *Ty)`：添加全局变量。
    - `print(raw_ostream &OS, AssemblyAnnotationWriter *AAW)`：输出 IR。
  - 用法：std::unique_ptr module = std::make_unique("my_module", context);
- **IRBuilder** (`<llvm/IR/IRBuilder.h>`)：
  - 功能：简化 IR 指令生成，维护当前插入点（BasicBlock），提供便捷方法创建指令。提供方法如 CreateAdd、CreateRet、CreateBr。
  - 主要方法（部分）：
    - `SetInsertPoint(BasicBlock *BB)`：设置指令插入的基本块。
    - `CreateAdd(Value *LHS, Value *RHS, StringRef Name)`：生成加法指令。
    - `CreateMul(Value *LHS, Value *RHS)`：生成乘法指令。
    - `CreateICmpSGT(Value *LHS, Value *RHS)`：生成有符号大于比较。
    - `CreateRet(Value *V)`：生成返回指令。
    - `CreateBr(BasicBlock *Dest)`：生成无条件分支。
    - `CreateCondBr(Value *Cond, BasicBlock *True, BasicBlock *False)`：生成条件分支。
    - `CreateLoad(Type *Ty, Value *Ptr)`：生成加载指令。
    - `CreateStore(Value *Val, Value *Ptr)`：生成存储指令。
  - 优势：自动处理指令插入顺序，减少手动管理。
  - 用法：IRBuilder<> builder(context); builder.SetInsertPoint(bb); 设置插入点后添加指令。
- **Function** (`<llvm/IR/Function.h>`)：
  - 功能：表示 IR 中的函数，包含参数、返回类型和基本块（BasicBlock）。。
  - 主要方法：
    - `Function::Create(FunctionType *Ty, LinkageTypes Linkage, StringRef Name, Module *M)`：创建函数。
    - `arg_begin()`：获取参数迭代器。
  - 用法：通过 llvm::FunctionType 定义函数类型，llvm::Function::Create 创建函数。
  - 示例：FunctionType::get(Type::getInt32Ty(context), {int_type, int_type}, false);
- **BasicBlock** (`<llvm/IR/BasicBlock.h>`)：
  - 功能：IR 的代码块，存储指令序列，是函数的基本单位，基于 SSA（静态单赋值）形式，每个值只赋值一次。
  - 主要方法：
    - `BasicBlock::Create(LLVMContext &C, StringRef Name, Function *Parent)`：创建基本块。
  - 用法：BasicBlock::Create(context, "entry", func); 创建并绑定到函数。
- **Type** (`<llvm/IR/Type.h>`)：
  - 功能：定义 IR 中的类型（如 i32、float、struct）。
  - 主要方法：
    - `Type::getInt32Ty(LLVMContext &C)`：获取 32 位整数类型。
    - `Type::getVoidTy(LLVMContext &C)`：获取 void 类型。
    - `StructType::create(LLVMContext &C, StringRef Name)`：创建结构体类型。
  - 使用场景：定义函数参数、返回值或表字段类型。
- **Value** (`<llvm/IR/Value.h>`)：
  - 功能：IR 的基本对象，表示值（如常量、变量、指令结果）。
  - 主要方法：
    - `setName(StringRef Name)`：为值命名，便于调试。
  - 使用场景：表示查询中的列值（如 `age`）或中间结果。
- **Verifier** (`<llvm/IR/Verifier.h>`)：
  - 功能：验证 IR 的合法性。
  - 主要方法：
    - `verifyModule(const Module &M, raw_ostream *OS)`：检查 Module 是否有效。
  - 使用场景：确保生成的 IR 正确。
- **ExecutionEngine** (`<llvm/ExecutionEngine/ExecutionEngine.h>`)：
  - 功能：支持 JIT 编译和执行生成的 IR。
  - 主要方法：
    - `ExecutionEngine::create(Module *M)`：创建 JIT 引擎。
    - `getFunctionAddress(StringRef Name)`：获取编译后的函数地址。
  - 使用场景：动态执行数据库查询代码。

#### 2. 使用方法
以下是使用 LLVM CodeGen API 生成 IR 的标准流程，结合数据库查询场景（如 `SELECT age + 10 FROM users WHERE age > 30`）。

1. **初始化环境**：
   - 创建 `LLVMContext`：`llvm::LLVMContext context;`
   - 创建 `Module`：`std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("query_module", context);`
   - 初始化 `IRBuilder`：`llvm::IRBuilder<> builder(context);`

2. **定义函数**：
   - 定义函数签名：`auto func_type = llvm::FunctionType::get(int_type, {int_type}, false);`
   - 创建函数：`auto func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "process_row", module.get());`

3. **创建基本块和指令**：
   - 创建 BasicBlock：`auto entry = llvm::BasicBlock::Create(context, "entry", func);`
   - 设置插入点：`builder.SetInsertPoint(entry);`
   - 生成指令：
     - 获取参数：`auto arg = func->arg_begin();`
     - 比较：`auto cmp = builder.CreateICmpSGT(arg, builder.getInt32(30));`
     - 分支：`builder.CreateCondBr(cmp, then_bb, else_bb);`
     - 计算：`auto result = builder.CreateAdd(arg, builder.getInt32(10));`
     - 返回：`builder.CreateRet(result);`

4. **验证和输出**：
   - 验证：`llvm::verifyModule(*module, &llvm::errs());`
   - 输出 IR：`module->print(llvm::outs(), nullptr);`
   - 可选：用 `llc` 编译 IR 为对象文件，或用 `ExecutionEngine` JIT 执行。

#### 3. 示例代码
以下是一个完整的 C++ 示例，生成一个数据库查询函数 `int process_row(int age)`，模拟 `SELECT age + 10 FROM users WHERE age > 30`。如果 `age > 30`，返回 `age + 10`，否则返回 0。

<xaiArtifact artifact_id="db73e924-6ea3-4234-a770-a2208e523063" artifact_version_id="e435e313-46f2-4ba8-b160-e84a4cfc4d73" title="llvm_ir_query.cpp" contentType="text/x-c++src">
```cpp
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

int main() {
    // 初始化 Context 和 Module
    llvm::LLVMContext context;
    auto module = std::make_unique<llvm::Module>("query_module", context);
    llvm::IRBuilder<> builder(context);

    // 定义函数: int process_row(int age)
    auto int_type = llvm::Type::getInt32Ty(context);
    auto func_type = llvm::FunctionType::get(int_type, {int_type}, false);
    auto func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "process_row", module.get());

    // 创建基本块
    auto entry = llvm::BasicBlock::Create(context, "entry", func);
    auto then_bb = llvm::BasicBlock::Create(context, "then", func);
    auto else_bb = llvm::BasicBlock::Create(context, "else", func);

    // 设置插入点
    builder.SetInsertPoint(entry);

    // 获取参数 age
    auto arg_age = func->arg_begin();
    arg_age->setName("age");

    // 过滤: age > 30
    auto cmp = builder.CreateICmpSGT(arg_age, builder.getInt32(30), "cmp");
    builder.CreateCondBr(cmp, then_bb, else_bb);

    // Then 分支: 返回 age + 10
    builder.SetInsertPoint(then_bb);
    auto result = builder.CreateAdd(arg_age, builder.getInt32(10), "result");
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
```
</xaiArtifact>

- **编译运行**：`clang++ llvm_ir_query.cpp `llvm-config --cxxflags --ldflags --libs core -o query_gen``
- **输出 IR**：
  ```
  define i32 @process_row(i32 %age) {
  entry:
    %cmp = icmp sgt i32 %age, 30
    br i1 %cmp, label %then, label %else
  
  then:
    %result = add i32 %age, 10
    ret i32 %result
  
  else:
    ret i32 0
  }
  ```
- **说明**：代码使用 `IRBuilder::CreateICmpSGT` 生成比较，`CreateCondBr` 处理分支，`CreateAdd` 计算 `age + 10`，模拟数据库查询逻辑。

#### 4. 其他常用 API
以下是 CodeGen 中常用的附加 API，适用于数据库场景：
- **GlobalVariable** (`<llvm/IR/GlobalVariable.h>`)：
  - `GlobalVariable(Module *M, Type *Ty, bool isConstant, LinkageTypes Linkage, Constant *Initializer)`：创建全局变量，模拟数据库表数据。
- **PHINode** (`<llvm/IR/Instructions.h>`)：
  - `CreatePHI(Type *Ty, unsigned NumReservedValues)`：生成 PHI 节点，用于循环或分支合并。
- **CallInst** (`<llvm/IR/Instructions.h>`)：
  - `CreateCall(FunctionCallee Callee, ArrayRef<Value*> Args)`：调用函数，适用于调用外部函数（如 `printf`）。



---------------------------------------------------------