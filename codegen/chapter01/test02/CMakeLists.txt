cmake_minimum_required(VERSION 3.10)
project(LLVMIRQuery)

# 查找 LLVM 包
find_package(LLVM REQUIRED CONFIG)

# 打印 LLVM 配置以调试
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# 添加 LLVM 包含路径和定义
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# 创建可执行文件
add_executable(main main.cpp)

# 链接 LLVM 库和 tinfo（解决之前链接错误）
target_link_libraries(main PRIVATE LLVMCore LLVMIRReader LLVMSupport tinfo)