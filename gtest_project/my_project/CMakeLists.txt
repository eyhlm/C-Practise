cmake_minimum_required(VERSION 3.10)
project(LoggerProject)

# 设置C和C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")

# 指定Google Test的路径
# 注意：替换为你实际的Google Test编译路径
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../googletest)
set(GTEST_BUILD_DIR ${GTEST_DIR}/build)

# 添加包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../AI-Practise/include)  # 原项目的头文件目录
include_directories(${GTEST_DIR}/googletest/include)  # Google Test的头文件

# 找到原项目源文件
set(LOGGER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../AI-Practise/src/logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../AI-Practise/src/log_filter.c
)

# 将源文件编译为库
add_library(logger_lib ${LOGGER_SOURCES})

# 添加测试可执行文件
add_executable(log_filter_test test/log_filter_test.cpp)
add_executable(logger_test test/logger_test.cpp)

# 链接测试可执行文件与库和Google Test
target_link_libraries(log_filter_test
  logger_lib
  ${GTEST_BUILD_DIR}/lib/libgtest.a
  ${GTEST_BUILD_DIR}/lib/libgtest_main.a
  pthread
)

target_link_libraries(logger_test
  logger_lib
  ${GTEST_BUILD_DIR}/lib/libgtest.a
  ${GTEST_BUILD_DIR}/lib/libgtest_main.a
  pthread
)

# 启用测试
enable_testing()
add_test(NAME LogFilterTest COMMAND log_filter_test)
add_test(NAME LoggerTest COMMAND logger_test)