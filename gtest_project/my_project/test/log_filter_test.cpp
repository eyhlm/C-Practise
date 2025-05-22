/**
 * @file log_filter_test.cpp
 * @brief 日志过滤器的专项单元测试
 */

 #include <gtest/gtest.h>
 #include <gmock/gmock.h>
 #include <cstdio>
 #include <string>
 #include <thread>
 #include <chrono>
 #include <vector>
 #include <unistd.h>
 
 // 包含被测试的头文件
 extern "C" {
     #include "log_filter.h"
     // 测试内部函数需要访问静态函数，在这里声明
     unsigned int hash_string(const char *str, size_t len);
 }
 
 class LogFilterTest : public ::testing::Test {
 protected:
     void SetUp() override {
         // 确保每次测试前过滤器未初始化
         filter_destroy();
     }
     
     void TearDown() override {
         filter_destroy();
     }
 };
 
 // 基本初始化和销毁功能测试
 TEST_F(LogFilterTest, InitAndDestroy) {
     // 测试初始化
     ASSERT_EQ(0, filter_init());
     
     // 测试重复初始化
     ASSERT_EQ(0, filter_init());
     
     // 测试销毁
     filter_destroy();
     
     // 验证销毁后能重新初始化
     ASSERT_EQ(0, filter_init());
 }
 
 // 测试基本过滤功能
 TEST_F(LogFilterTest, BasicFilterCheck) {
     ASSERT_EQ(0, filter_init());
     
     const char* test_log = "Test log message";
     size_t len = strlen(test_log);
     
     // 首次检查不应过滤
     ASSERT_FALSE(filter_check(test_log, len));
     
     // 短时间内重复消息应该被过滤
     ASSERT_TRUE(filter_check(test_log, len));
     
     // 不同内容不应被过滤
     const char* different_log = "Different log message";
     ASSERT_FALSE(filter_check(different_log, strlen(different_log)));
 }
 
 // 测试专门的海量日志检测功能
 TEST_F(LogFilterTest, MassiveLogDetection) {
     ASSERT_EQ(0, filter_init());
     
     const char* test_log = "Test massive log detection";
     size_t len = strlen(test_log);
     
     // 首次检查不应过滤
     ASSERT_FALSE(filter_check_massive(test_log, len));
     
     // 重复少于60次不应标记为海量日志
     for (int i = 0; i < 58; i++) {  // 已经有1次了，再加58次，总共59次
         filter_check_massive(test_log, len);
     }
     
     // 第60次应标记为海量日志，但允许打印一次提示
     ASSERT_FALSE(filter_check_massive(test_log, len));
     
     // 之后应该被过滤
     ASSERT_TRUE(filter_check_massive(test_log, len));
 }
 
 // 测试多个不同日志的过滤情况
 TEST_F(LogFilterTest, MultipleLogMessages) {
     ASSERT_EQ(0, filter_init());
     
     std::vector<const char*> logs = {
         "Log message 1",
         "Log message 2",
         "Log message 3",
         "Log message 4",
         "Log message 5"
     };
     
     // 第一轮：所有日志都是首次出现，不应过滤
     for (const auto& log : logs) {
         ASSERT_FALSE(filter_check(log, strlen(log)));
     }
     
     // 第二轮：所有日志都是重复出现，应该过滤
     for (const auto& log : logs) {
         ASSERT_TRUE(filter_check(log, strlen(log)));
     }
 }
 
 // 模拟时间流逝测试过滤器重置功能（需要修改代码或使用Mock）
 // 由于无法直接修改time()函数返回值，此测试仅为示例
 TEST_F(LogFilterTest, DISABLED_FilterResetAfterOneHour) {
     ASSERT_EQ(0, filter_init());
     
     const char* test_log = "Test log reset";
     size_t len = strlen(test_log);
     
     // 首次检查不应过滤
     ASSERT_FALSE(filter_check(test_log, len));
     
     // 短时间内重复消息应该被过滤
     ASSERT_TRUE(filter_check(test_log, len));
     
     // 假设这里能修改time()返回值使其增加3601秒（1小时+1秒）
     // time_mock_add_seconds(3601);
     
     // 超过一小时后应重置过滤器，不应过滤
     // ASSERT_FALSE(filter_check(test_log, len));
 }
 
 // 测试空日志内容处理
 TEST_F(LogFilterTest, EmptyLogContent) {
     ASSERT_EQ(0, filter_init());
     
     // 空内容不应导致崩溃，且不应过滤
     ASSERT_FALSE(filter_check("", 0));
     ASSERT_FALSE(filter_check(nullptr, 0));
     
     // 空指针不应导致崩溃
     ASSERT_FALSE(filter_check(nullptr, 5));
 }
 
 // 测试未初始化情况
 TEST_F(LogFilterTest, UninitializedFilter) {
     // 确保过滤器未初始化
     filter_destroy();
     
     // 未初始化时不应过滤
     ASSERT_FALSE(filter_check("Test", 4));
     ASSERT_FALSE(filter_check_massive("Test", 4));
 }
 
 // 测试多线程并发访问
 TEST_F(LogFilterTest, ThreadSafetyTest) {
     ASSERT_EQ(0, filter_init());
     
     const int THREAD_COUNT = 10;
     const int LOGS_PER_THREAD = 100;
     
     auto thread_func = [LOGS_PER_THREAD](int thread_id) {
         for (int i = 0; i < LOGS_PER_THREAD; i++) {
             std::string log = "Thread " + std::to_string(thread_id) + " log " + std::to_string(i);
             filter_check(log.c_str(), log.length());
         }
     };
     
     std::vector<std::thread> threads;
     for (int i = 0; i < THREAD_COUNT; i++) {
         threads.emplace_back(thread_func, i);
     }
     
     for (auto& t : threads) {
         t.join();
     }
     
     // 这个测试主要是确保在多线程环境下不会崩溃
     // 如果代码线程安全，则测试将顺利完成
 }
 
 // 测试非常长的日志内容
 TEST_F(LogFilterTest, LongLogContent) {
     ASSERT_EQ(0, filter_init());
     
     // 创建一个很长的日志内容（约1KB）
     std::string long_log;
     for (int i = 0; i < 1000; i++) {
         long_log += static_cast<char>('A' + (i % 26));
     }
     
     // 首次检查不应过滤
     ASSERT_FALSE(filter_check(long_log.c_str(), long_log.length()));
     
     // 重复消息应该被过滤
     ASSERT_TRUE(filter_check(long_log.c_str(), long_log.length()));
 }
 
 // 主函数
 int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
 }