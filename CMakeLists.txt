cmake_minimum_required(VERSION 2.6)

project(Server)

# 添加编译标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -ggdb -Wall -O0 -Wno-unused-variable -pthread")

# 查找 Boost 库，并确保包含 thread 和 system 库
find_package(Boost REQUIRED COMPONENTS thread system)



# 添加库文件搜索路径
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib/
)

# 源文件设置
set(base_src
    ../base/ThreadPool.cpp
    ../base/Buffer.cpp
    ../base/util.cpp
)


# 源文件设置
set(net_src
    ../net/TcpSocket.cpp
)


set(src
    ../main.cpp
)


# 创建可执行文件
add_executable(Server ${src} ${base_src} ${net_src})


# 创建可执行文件
add_executable(TcpSocketTest ${base_src}  ${net_src} ../test/TcpSocketTest.cpp)

if(0)
# 创建可执行文件
add_executable(TestThreadPool ${base_src} ../test/ThreadPool.cpp)

# 创建可执行文件
add_executable(PoolTest ${base_src} ../test/PoolTest.cpp)

# 创建可执行文件
add_executable(BufferTest ${base_src} ../test/BufferTest.cpp)
endif()




# 链接 Boost 线程库和系统库
target_link_libraries(Server Boost::thread Boost::system)
