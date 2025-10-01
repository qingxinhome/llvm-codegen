### LLVM 的 lli 命令介绍

`lli` 是 LLVM 项目中的一个核心命令行工具，全称为 LLVM Interpreter（LLVM 解释器），用于直接执行 LLVM 位码（bitcode）文件。它结合了解释器和即时编译（JIT）功能，能够在运行时将 LLVM IR（中间表示）编译为本地机器码并执行，非常适合测试和调试 LLVM 生成的代码。在你的测试环境（RHEL 9.5，LLVM 18.1.8）中，`lli` 可以通过 `llvm-config --bindir` 找到路径，通常安装在 `/usr/bin/lli` 或类似位置（可通过 `dnf install llvm-18` 安装）。

#### 1. 概述
- **主要功能**：`lli` 读取 LLVM 位码文件（`.bc` 或 `.ll`），使用 ORC JIT（LLVM 的现代 JIT 框架）或简单解释器执行它，支持动态加载和运行。
- **适用场景**：
  - 测试生成的 LLVM IR：快速验证 IR 代码是否正确执行。
  - JIT 开发：模拟运行时编译场景。
  - 脚本执行：结合 Clang 生成位码后直接运行。
- **版本兼容**：在 LLVM 18.1.8 中，`lli` 支持不透明指针（Opaque Pointers）和现代 ORC JIT 改进，性能优于早期版本。
- **安装检查**（RHEL 9.5）：
  ```bash
  lli --version  # 输出 LLVM 18.1.8
  ```
  如果未安装，运行 `sudo dnf install llvm-18-runtime`。

#### 2. 基本用法
`lli` 的基本语法：
```bash
lli [选项] <输入位码文件>
```
- **输入文件**：
  - `.bc`：LLVM 位码文件（二进制）。
  - `.ll`：LLVM 汇编文件（文本 IR），`lli` 会自动编译为位码。
- **执行流程**：
  1. 加载位码。
  2. 初始化 JIT（可选优化）。
  3. 执行主函数（默认 `main`）。
  4. 输出结果或错误。

示例：创建一个简单 C 文件 `hello.c`：
```c
#include <stdio.h>
int main() { printf("Hello, LLVM!\n"); return 0; }
```
生成位码：
```bash
clang-18 -emit-llvm -c hello.c -o hello.bc
```
执行：
```bash
lli hello.bc
```
输出：
```
Hello, LLVM!
```

#### 3. 常用选项
基于 LLVM 18.1.8 的 man page 和官方文档，以下是关键选项（运行 `lli --help` 查看完整列表）：

| 选项                               | 描述                               | 示例                                   |
| ---------------------------------- | ---------------------------------- | -------------------------------------- |
| `-entry-function <name>`           | 指定入口函数（默认 `main`）。      | `lli -entry-function my_main hello.bc` |
| `-force-interpreter`               | 强制使用解释器模式（不使用 JIT）。 | `lli -force-interpreter hello.bc`      |
| `-jit-kind <lljit|orc-lazy|mcjit>` | 指定 JIT 引擎（默认 `lljit`）。    | `lli -jit-kind orc-lazy hello.bc`      |
| `-O0` / `-O1` / `-O2` / `-O3`      | 优化级别（`-O0` 无优化）。         | `lli -O3 hello.bc`                     |
| `-debug`                           | 启用调试输出（打印 JIT 过程）。    | `lli -debug hello.bc`                  |
| `-load <library>`                  | 动态加载共享库。                   | `lli -load libm.so hello.bc`           |
| `-extra-module <file>`             | 加载额外位码模块。                 | `lli -extra-module extra.bc hello.bc`  |
| `-stats`                           | 输出性能统计（如执行时间）。       | `lli -stats hello.bc`                  |
| `-disable-opt`                     | 禁用所有优化。                     | `lli -disable-opt hello.bc`            |

- **RHEL 9.5 注意**：在 RHEL 9.5 上，命令为 `lli`，需使用 `llvm-config-18 --bindir` 确认路径。JIT 模式下，确保系统支持本地架构（x86_64）。
