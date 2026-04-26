cmake_minimum_required(VERSION 3.14) # 提升版本以支持 FetchContent
project(zhuan2)
set(CMAKE_CXX_STANDARD 17)

# 自动下载 nlohmann/json 库用于解析配置
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# 寻找 Raylib
find_package(raylib QUIET)
find_package(ENet REQUIRED)
target_link_libraries(BrickBreaker ENet::ENet)
# 主游戏程序
add_executable(zhuan2 main.cpp Ball.cpp Paddle.cpp Brick.cpp Game.cpp  PowerUpEffects.cpp )
target_include_directories(zhuan2 PRIVATE ${json_SOURCE_DIR}/include) # 引入 json 头文件

if (raylib_FOUND)
    target_link_libraries(zhuan2 raylib)
else()
    include_directories(/usr/local/include)
    target_link_libraries(zhuan2 raylib m pthread dl)
endif()

# ================= 单元测试配置 =================
enable_testing()
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# 创建测试可执行文件（注意：这里不需要链接 raylib，实现了真正的逻辑解耦！）
add_executable(unit_tests
    tests/TestCollision.cpp
)
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unit_tests gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(unit_tests)
