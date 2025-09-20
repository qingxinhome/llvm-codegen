### 2. `llvm::Function::Create` 使用

​	在 LLVM 中，llvm::Function::Create 是用于创建 IR（中间表示）中函数的核心 API，属于 CodeGen API 的一部分。它允许您定义一个函数，包括其签名（返回类型和参数类型）、名称和链接类型（如外部链接），并将其添加到 Module 中。

- **头文件**：`<llvm/IR/Function.h>`
- **功能**：llvm::Function::Create是Function类的静态成员函数， 用于创建 `llvm::Function` 对象。 `llvm::Function` 对象代表 IR 中的一个函数，包含函数签名、名称、参数和基本块（BasicBlock）。函数必须绑定到一个 `Module`。
- **返回类型**：`llvm::Function*`，指向创建的函数对象。

#### 函数签名
```cpp
static Function *Create(FunctionType *Ty, LinkageTypes Linkage, const Twine &Name, Module *M);
```

#### 参数说明
1. **FunctionType *Ty**：
   
   - 函数类型：`llvm::FunctionType`（定义函数签名，包括返回类型和参数类型）。
   - 创建方式：
     ```cpp
     auto int_type = llvm::Type::getInt32Ty(context);
     auto func_type = llvm::FunctionType::get(return_type, param_types, isVarArg);
     ```
     - `return_type`：返回类型（如 `Type::getInt32Ty(context)`）。
     - `param_types`：参数类型列表（如 `{int_type, int_type}`）。
     - `isVarArg`：是否为变参函数（如 `printf`），通常为 `false`。
   - 示例：`int add(int, int)` 的签名：
     ```cpp
     auto func_type = llvm::FunctionType::get(int_type, {int_type, int_type}, false);
     ```
   
2. **LinkageTypes Linkage**：
   
   - 类型：`llvm::Function::LinkageTypes` 枚举，指定函数的链接属性。
   - 常见值：
     - `ExternalLinkage`：函数对其他模块可见（如全局函数）。
     - `InternalLinkage`：函数仅在当前 Module 内部可见（类似 `static`）。
     - 示例：`ExternalLinkage` 用于数据库查询函数，允许外部调用。
     ```cpp
     llvm::Function::ExternalLinkage
     ```
   
3. **const Twine &Name**：
   - 类型：`llvm::Twine`，函数的名称（字符串类型）。
   - 用途：指定 IR 中的函数名（如 `@process_row`），便于调试和调用。
   - 示例：`"process_row"`

4. **Module *M**：
   - 类型：`llvm::Module*`，函数所属的 Module。
   - 用途：将函数添加到 Module 的符号表，Module 管理所有函数。
   - 示例：`module.get()`（假设 `module` 是 `std::unique_ptr<llvm::Module>`）。

---

### 使用步骤
以下是使用 `llvm::Function::Create` 创建函数的典型步骤，结合数据库查询场景：

1. **初始化环境**：
   - 创建 `LLVMContext` 和 `Module`：
     ```cpp
     llvm::LLVMContext context;
     auto module = std::make_unique<llvm::Module>("query_module", context);
     ```

2. **定义函数签名**：
   - 使用 `FunctionType::get` 创建签名：
     ```cpp
     auto int_type = llvm::Type::getInt32Ty(context);
     auto func_type = llvm::FunctionType::get(int_type, {int_type}, false);
     ```

3. **创建函数**：
   - 调用 `Function::Create`：
     ```cpp
     auto func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "process_row", module.get());
     ```

4. **添加基本块和指令**：
   - 创建 `BasicBlock` 并用 `IRBuilder` 添加指令：
     ```cpp
     auto entry = llvm::BasicBlock::Create(context, "entry", func);
     llvm::IRBuilder<> builder(context);
     builder.SetInsertPoint(entry);
     ```

5. **验证和输出**：
   - 验证 Module 并输出 IR：
     ```cpp
     llvm::verifyModule(*module, &llvm::errs());
     module->print(llvm::outs(), nullptr);
     ```
