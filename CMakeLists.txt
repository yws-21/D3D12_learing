cmake_minimum_required(VERSION 3.20)

# 项目名称，可以随意改
project(D3D12_learning LANGUAGES CXX)

# 设置 C++ 标准 (D3D12 教程通常用 C++17 或 C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 自动收集所有的源文件 (*.cpp)
# 这里使用了 GLOB_RECURSE，意味着它会自动去 D3D、Support、Debug 甚至根目录找所有的 cpp 文件。
# 以后你加新文件不用改这里。
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 2. 生成可执行文件
add_executable(D3D12_learning ${SOURCES})

# 3. 设置头文件包含目录
# 这样你的代码里写 #include <Support/WinInclude.h> 或 #include "D3D/DXContext.h" 都能找到
target_include_directories(D3D12_learning PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Support
    ${CMAKE_CURRENT_SOURCE_DIR}/D3D
    ${CMAKE_CURRENT_SOURCE_DIR}/Debug
)

# 4. 链接 Windows 和 DirectX 库
target_link_libraries(D3D12_learning PRIVATE 
    d3d12
    dxgi
    d3dcompiler
    dxguid
)

# 5. 设置字符集为 Unicode (VS 默认)
if(MSVC)
    target_compile_options(D3D12_learning PRIVATE /utf-8)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# 6. 如果你是 Windows 窗口程序，可能需要设置子系统（看你的 main 入口）
# 教程里用的是 int main()，所以默认是控制台，会弹黑框，这个不用改。