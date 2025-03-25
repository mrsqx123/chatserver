# 指定目标系统
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 指定交叉编译工具链的根路径
set(CMAKE_C_COMPILER /home/sqx/sqxEnv/sdk_imx6ull/toolchain/gcc-arm-10.2-x86-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /home/sqx/sqxEnv/sdk_imx6ull/toolchain/gcc-arm-10.2-x86-linux-gnueabihf/bin/arm-none-linux-gnueabihf-g++)

# 设置 sysroot 路径
set(CMAKE_SYSROOT /home/sqx/sqxEnv/sdk_imx6ull/toolchain/gcc-arm-10.2-x86-linux-gnueabihf/arm-none-linux-gnueabihf/libc)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

# 设置搜索规则
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


