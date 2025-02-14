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
)

set(src
    ../main.cpp
)


# 创建可执行文件
add_executable(Server ${src} ${base_src})


# 创建可执行文件
add_executable(TestThreadPool ${base_src} ../test/ThreadPool.cpp)

if(0)

endif()




# 链接 Boost 线程库和系统库
target_link_libraries(Server Boost::thread Boost::system)
