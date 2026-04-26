# Raylib 打砖块游戏

## 项目简介
这是一个使用 C++ 和 Raylib 库开发的经典打砖块游戏。

## 功能
- 球体与墙壁、挡板、砖块的碰撞检测。
- 生命值系统 (Lives)。
- 计分系统。
- 游戏结束与重新开始。

## 编译运行
mkdir build && cd build
cmake ..
make
./zhuan2

## 运行测试
g++ tests/test_collision.cpp -o tests/run_tests
./tests/run_tests

## 团队分工
- 成员A: 核心逻辑开发、Git管理
- 成员B: 单元测试编写、文档撰写
