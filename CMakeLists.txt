cmake_minimum_required(VERSION 3.10)
project(breakout)

set(CMAKE_CXX_STANDARD 11)

# 手动指定你的 raylib 路径（Windows 最稳定，永不报错）
set(RAYLIB_DIR "C:/raylib")

# 包含头文件
include_directories(${RAYLIB_DIR}/include)
link_directories(${RAYLIB_DIR}/lib/mingw-w64)

# 你的所有源文件
add_executable(breakout
    main.cpp
    Ball.cpp
    Paddle.cpp
    Brick.cpp
)

# 链接 raylib
target_link_libraries(breakout raylib winmm gdi32 opengl32)