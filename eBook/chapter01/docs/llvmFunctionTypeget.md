### 1.`llvm::FunctionType::get` 方法介绍

`llvm::FunctionType::get` 是 LLVM C++ API 中用于创建函数类型的核心方法，位于 `llvm::FunctionType` 类中。它用于定义函数的返回类型和参数类型，是生成 LLVM IR（中间表示）中函数定义的关键步骤。

#### **说明**：

- `llvm::FunctionType::get` 方法定义在 **`<llvm/IR/Function.h>`** 头文件中。
- `FunctionType` 是 LLVM IR 中用于表示函数类型的类，`get` 是FunctionType类的静态成员方法，用于创建函数类型。
- 该头文件包含了与函数相关的核心类和方法（如 `llvm::Function` 和 `llvm::FunctionType`），广泛用于生成 LLVM IR 的函数签名。

#### 功能
`llvm::FunctionType::get` 创建一个 `FunctionType` 对象，表示函数的类型签名，包括：

- 返回值的类型（如 `i32`、`float`、指针类型等）。
- 参数列表的类型。
- 是否为变长参数函数（varargs）。

该方法返回的 `FunctionType` 指针可用于创建 `llvm::Function` 对象，进而定义函数的 IR 结构。

#### **`FunctionType::get` 有两种主要重载形式：**

1. **static FunctionType \*get(Type \*Result, ArrayRef<Type\*> Params, bool isVarArg)**

- **方法签名：**

```cpp
static FunctionType *get(Type *Result, ArrayRef<Type*> Params, bool isVarArg);
```

- **参数**：

  - `Result`：返回类型，类型为 `llvm::Type*`。可以是基本类型（如 `IntegerType::getInt32Ty`）、指针类型或结构体类型。
  - `Params`：参数类型的列表，通常为 `ArrayRef<Type*>`（或 `std::vector<Type*>`），每个元素表示一个参数的类型。
  - `isVarArg`：布尔值，指定函数是否支持变长参数（如 C 风格的 `printf`）。通常在数据库 CodeGen 中设为 `false`。

-  **返回值**

  - 返回 `FunctionType*`，表示函数的类型签名。

  - 该对象归 `LLVMContext` 管理，无需手动释放，但需确保 `LLVMContext` 的生命周期覆盖整个 IR 生成过程。

示例：`int add(int, int)` 的签名：

```
  // 定义返回类型和参数类型
  llvm::Type* Int32Ty = llvm::Type::getInt32Ty(Context); // i32 类型
  std::vector<llvm::Type*> ParamTypes = {Int32Ty, Int32Ty}; // 两个 i32 参数

  // 创建函数类型：i32 (i32, i32)
  llvm::FunctionType* FuncTy = llvm::FunctionType::get(
      Int32Ty,           // 返回类型
      ParamTypes,        // 参数类型列表
      false              // 非变长参数
  );
```

2. **static FunctionType \*get(Type \*Result, bool isVarArg)**

- **方法签名：**

```cpp
static FunctionType *get(Type *Result, bool isVarArg);
```

- **参数**：

  - 用于无参数函数，`Params` 为空。

  - 其他参数同上。

-  **返回值**

  - 返回 `FunctionType*`，表示函数的类型签名。

  - 该对象归 `LLVMContext` 管理，无需手动释放，但需确保 `LLVMContext` 的生命周期覆盖整个 IR 生成过程。

示例：定义无参数函数类型 void ()

```
// 定义返回类型
llvm::Type* VoidTy = llvm::Type::getVoidTy(Context); // void 类型

// 创建函数类型：void ()
llvm::FunctionType* FuncTy = llvm::FunctionType::get(
    VoidTy,            // 返回类型
    false              // 非变长参数
);
```