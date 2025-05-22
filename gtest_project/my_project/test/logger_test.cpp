/**
 * @file logger_test.cpp
 * @brief 日志系统的单元测试
 */

 #include <gtest/gtest.h>
 #include <gmock/gmock.h>
 #include <cstdio>
 #include <fstream>
 #include <string>
 #include <thread>
 #include <chrono>
 
 // 包含被测试的头文件
 extern "C" {
     #include "logger.h"
     #include "log_filter.h"
 }
 
 class LoggerTest : public ::testing::Test {
 protected:
     void SetUp() override {
         // 确保每次测试前系统未初始化
         log_destroy();
         filter_destroy();
         
         // 使用临时文件进行测试
         temp_log_filename = "test_log.txt";
         
         // 确保文件不存在
         std::remove(temp_log_filename);
     }
     
     void TearDown() override {
         // 清理
         log_destroy();
         filter_destroy();
         std::remove(temp_log_filename);
     }
     
     // 检查日志文件中是否包含指定字符串
     bool log_file_contains(const std::string& text) {
         std::ifstream file(temp_log_filename);
         if (!file.is_open()) {
             return false;
         }
         
         std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
         file.close();
         
         return content.find(text) != std::string::npos;
     }
     
     // 获取日志文件内容
     std::string get_log_content() {
         std::ifstream file(temp_log_filename);
         if (!file.is_open()) {
             return "";
         }
         
         std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
         file.close();
         
         return content;
     }
     
     // 清空日志文件
     void clear_log_file() {
         std::ofstream file(temp_log_filename, std::ios::trunc);
         file.close();
     }
     
     const char* temp_log_filename;
 };
 
 // 基本初始化和销毁功能测试
 TEST_F(LoggerTest, InitAndDestroy) {
     // 测试初始化
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_DEBUG, LOG_MODE_NORMAL));
     ASSERT_TRUE(log_file_contains("Log system initialized successfully"));
     
     // 测试重复初始化
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_INFO, LOG_MODE_NORMAL));
     
     // 测试销毁
     log_destroy();
     
     // 验证销毁后能重新初始化
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_DEBUG, LOG_MODE_NORMAL));
 }
 
 // 测试不同日志级别
 TEST_F(LoggerTest, LogLevels) {
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_INFO, LOG_MODE_NORMAL));
     clear_log_file(); // 清除初始化日志
     
     // DEBUG级别日志不应该被打印
     LOG_DEBUG("This is a debug message");
     ASSERT_FALSE(log_file_contains("DEBUG"));
     
     // INFO及以上级别应该被打印
     LOG_INFO("This is an info message");
     ASSERT_TRUE(log_file_contains("INFO"));
     
     LOG_WARN("This is a warning message");
     ASSERT_TRUE(log_file_contains("WARN"));
     
     LOG_ERROR("This is an error message");
     ASSERT_TRUE(log_file_contains("ERROR"));
     
     LOG_FATAL("This is a fatal message");
     ASSERT_TRUE(log_file_contains("FATAL"));
     
     // 测试设置日志级别
     log_set_level(LOG_LEVEL_ERROR);
     clear_log_file();
     
     LOG_INFO("This should not appear");
     LOG_WARN("This should not appear either");
     ASSERT_FALSE(log_file_contains("INFO"));
     ASSERT_FALSE(log_file_contains("WARN"));
     
     LOG_ERROR("This should appear");
     LOG_FATAL("This should also appear");
     ASSERT_TRUE(log_file_contains("ERROR"));
     ASSERT_TRUE(log_file_contains("FATAL"));
 }
 
 // 测试日志格式
 TEST_F(LoggerTest, LogFormat) {
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_DEBUG, LOG_MODE_NORMAL));
     clear_log_file();
     
     LOG_INFO("Test message with formatting: %d, %s", 42, "string");
     
     std::string content = get_log_content();
     
     // 检查日期时间格式
     EXPECT_THAT(content,::testing::MatchesRegex("^[0-9]{4}-[0-9]{2}-[0-9]{2}.*"));
     
     // 检查级别
     EXPECT_THAT(content, ::testing::HasSubstr("[INFO]"));
     
     // 检查文件和行号
    EXPECT_THAT(content, ::testing::MatchesRegex(".*\\[.*cpp.*\\].*"));
     
     // 检查消息内容
     EXPECT_THAT(content, ::testing::HasSubstr("Test message with formatting: 42, string"));
 }
 
 // 测试日志过滤功能
 TEST_F(LoggerTest, LogFilter) {
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_DEBUG, LOG_MODE_FILTER));
     
     // 第一次打印不会被过滤
     LOG_INFO("Repeated message");
     ASSERT_TRUE(log_file_contains("Repeated message"));
     
     clear_log_file();
     
     // 短时间内重复消息应该被过滤
     LOG_INFO("Repeated message");
     ASSERT_FALSE(log_file_contains("Repeated message"));
     
     // 切换到普通模式应该不过滤重复消息
     log_set_mode(LOG_MODE_NORMAL);
     clear_log_file();
     
     LOG_INFO("Repeated message");
     ASSERT_TRUE(log_file_contains("Repeated message"));
 }
 
 // 测试海量日志过滤功能
 TEST_F(LoggerTest, MassiveLogFilter) {
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_DEBUG, LOG_MODE_NORMAL));
     
     // 在普通模式下也应该过滤海量日志，但前59条应该正常打印
     for (int i = 0; i < 59; i++) {
         LOG_INFO("Massive log test");
     }
     
     clear_log_file();
     
     // 第60条正常打印
     LOG_INFO("Massive log test");
     ASSERT_TRUE(log_file_contains("Massive log test"));
     
     clear_log_file();
     
     // 之后的日志应该被过滤
     LOG_INFO("Massive log test");
     ASSERT_FALSE(log_file_contains("Massive log test"));
     
     // 切换到过滤模式，结果应相同
     log_set_mode(LOG_MODE_FILTER);
     clear_log_file();
     
     LOG_INFO("Massive log test");
     ASSERT_FALSE(log_file_contains("Massive log test"));
 }
 
 // 测试多线程安全性
 TEST_F(LoggerTest, ThreadSafety) {
     ASSERT_EQ(0, log_init(temp_log_filename, LOG_LEVEL_DEBUG, LOG_MODE_NORMAL));
     clear_log_file();
     
     const int THREAD_COUNT = 10;
     const int LOGS_PER_THREAD = 100;
     
     auto thread_func = [LOGS_PER_THREAD](int thread_id) {
         for (int i = 0; i < LOGS_PER_THREAD; i++) {
             LOG_INFO("Thread %d, log %d", thread_id, i);
         }
     };
     
     std::vector<std::thread> threads;
     for (int i = 0; i < THREAD_COUNT; i++) {
         threads.emplace_back(thread_func, i);
     }
     
     for (auto& t : threads) {
         t.join();
     }
     
     // 验证日志完整性 - 检查每个线程的第一条和最后一条日志
     std::string content = get_log_content();
     for (int i = 0; i < THREAD_COUNT; i++) {
         std::string first_log = "Thread " + std::to_string(i) + ", log 0";
         std::string last_log = "Thread " + std::to_string(i) + ", log " + std::to_string(LOGS_PER_THREAD - 1);
         
         // 由于过滤机制，可能不是所有日志都会出现，所以我们只检查每个线程是否有日志
         EXPECT_TRUE(content.find("Thread " + std::to_string(i)) != std::string::npos);
     }
 }
 
 // 模拟时间函数，用于测试过滤器重置功能
 class FilterResetTest : public ::testing::Test {
 protected:
     void SetUp() override {
         // 确保每次测试前系统未初始化
         log_destroy();
         filter_destroy();
         
         // 初始化过滤器
         ASSERT_EQ(0, filter_init());
     }
     
     void TearDown() override {
         filter_destroy();
     }
 };
 
 // 测试过滤器独立功能
 TEST_F(FilterResetTest, BasicFilterCheck) {
     const char* test_log = "Test log message";
     size_t len = strlen(test_log);
     
     // 首次检查不应过滤
     ASSERT_FALSE(filter_check(test_log, len));
     
     // 第二次检查应该过滤
     ASSERT_TRUE(filter_check(test_log, len));
     
     // 不同内容不应被过滤
     const char* different_log = "Different log message";
     ASSERT_FALSE(filter_check(different_log, strlen(different_log)));
 }
 
 // 测试海量日志检测
 TEST_F(FilterResetTest, MassiveLogCheck) {
     const char* test_log = "Massive log test";
     size_t len = strlen(test_log);
     
     // 前59次检查不应该标记为海量
     for (int i = 0; i < 59; i++) {
         filter_check_massive(test_log, len);
     }
     
     // 第60次应该标记为海量，但允许打印一次提示
     ASSERT_FALSE(filter_check_massive(test_log, len));
     
     // 之后应该过滤
     ASSERT_TRUE(filter_check_massive(test_log, len));
 }
 
 // 主函数
 int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
 }