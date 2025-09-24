### `llvm::IRBuilder::SetInsertPoint` 方法介绍

`llvm::IRBuilder::SetInsertPoint` 是 LLVM C++ API 中 `llvm::IRBuilder` 类的核心成员方法，用于设置 IR 指令的插入点。它指定指令将插入到哪个基本块（`BasicBlock`）或基本块中的具体位置（在某条指令之前）。

#### 1. 功能
`SetInsertPoint` 设置 `IRBuilder` 的当前插入点，决定后续通过 `IRBuilder` 创建的指令（如 `CreateAdd`、`CreateRet`）插入到哪个基本块或指令位置。它的主要作用包括：
- 指定指令插入到某个基本块的末尾（默认）。
- 指定指令插入到基本块中的某条指令之前（精确控制）。
- 支持动态构造控制流（如分支、循环），在数据库 CodeGen 中用于生成查询执行的 IR 结构。

#### 2. 方法签名
`IRBuilder::SetInsertPoint` 有以下主要重载形式（基于 LLVM 18+）：

```cpp
void SetInsertPoint(BasicBlock *BB);
```

- **参数**：
  - `BB`：`llvm::BasicBlock*`，指定插入指令的基本块。后续指令将追加到该基本块的末尾。
- **返回**：`void`，直接修改 `IRBuilder` 的内部状态。

```cpp
void SetInsertPoint(Instruction *I);
```

- **参数**：
  - `I`：`llvm::Instruction*`，指定插入点为某条指令之前。后续指令将插入到 `I` 之前。
- **返回**：`void`。

#### 3. 使用示例
以下是一个简单的 C++ 示例，展示如何使用 `SetInsertPoint` 在一个函数中创建基本块并插入指令，生成 IR 表示 `i32 @add(i32, i32)`，实现两个整数相加。

```cpp
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>

int main() {
  llvm::LLVMContext Context;
  llvm::Module Mod("my_module", Context);

  // 定义函数类型：i32 (i32, i32)
  llvm::Type* Int32Ty = llvm::Type::getInt32Ty(Context);
  std::vector<llvm::Type*> ParamTypes = {Int32Ty, Int32Ty};
  llvm::FunctionType* FuncTy = llvm::FunctionType::get(Int32Ty, ParamTypes, false);

  // 创建函数
  llvm::Function* AddFunc = llvm::Function::Create(
      FuncTy, llvm::Function::ExternalLinkage, "add", &Mod);

  // 创建基本块
  llvm::BasicBlock* Entry = llvm::BasicBlock::Create(Context, "entry", AddFunc);

  // 创建 IRBuilder 并设置插入点
  llvm::IRBuilder<> Builder(Context);
  Builder.SetInsertPoint(Entry);

  // 获取函数参数
  auto ArgIt = AddFunc->arg_begin();
  llvm::Value* Arg1 = ArgIt++;
  llvm::Value* Arg2 = ArgIt;
  Arg1->setName("a");
  Arg2->setName("b");

  // 生成加法指令
  llvm::Value* Sum = Builder.CreateAdd(Arg1, Arg2, "sum");

  // 生成返回指令
  Builder.CreateRet(Sum);

  // 输出 IR
  Mod.print(llvm::outs(), nullptr);
  return 0;
}
```

**预期 IR 输出**：
```
; ModuleID = 'my_module'
source_filename = "my_module"

define i32 @add(i32 %a, i32 %b) {
entry:
  %sum = add i32 %a, %b
  ret i32 %sum
}
```
