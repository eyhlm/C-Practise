/**
 * @file logger_test.c
 * @brief 日志系统测试程序
 * 
 * 提供了一个测试程序，用于测试日志系统的各项功能，包括基本日志、
 * 日志过滤和海量日志屏蔽功能
 */

 #include "logger.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <pthread.h>
 
 /* 测试线程数量 */
 #define TEST_THREAD_NUM 3
 
 /* 测试函数声明 */
 void test_basic_logging(void);
 void test_filter_mode(void);
 void test_massive_logs(void);
 void *thread_func(void *arg);
 
 int main(int argc, char *argv[]) {
     printf("=== 日志系统测试程序 ===\n\n");
     
     /* 测试基本日志功能 */
     printf("测试1: 基本日志功能\n");
     test_basic_logging();
     
     /* 测试过滤模式 */
     printf("\n测试2: 日志过滤功能\n");
     test_filter_mode();
     
     /* 测试海量日志屏蔽功能 */
     printf("\n测试3: 海量日志屏蔽功能\n");
     test_massive_logs();
     
     /* 测试多线程日志 */
     printf("\n测试4: 多线程日志功能\n");
     pthread_t threads[TEST_THREAD_NUM];
     for (int i = 0; i < TEST_THREAD_NUM; i++) {
         int *thread_id = malloc(sizeof(int));
         *thread_id = i;
         pthread_create(&threads[i], NULL, thread_func, thread_id);
     }
     
     /* 等待所有线程结束 */
     for (int i = 0; i < TEST_THREAD_NUM; i++) {
         pthread_join(threads[i], NULL);
     }
     
     printf("\n所有测试完成！\n");
     return 0;
 }
 
 /**
  * @brief 测试基本日志功能
  */
 void test_basic_logging(void) {
     /* 初始化日志系统 */
     if (log_init("test_basic.log", LOG_LEVEL_DEBUG, LOG_MODE_NORMAL) != 0) {
         printf("初始化日志系统失败\n");
         return;
     }
     
     /* 测试各个级别的日志 */
     LOG_DEBUG("这是一条调试日志");
     LOG_INFO("这是一条信息日志");
     LOG_WARN("这是一条警告日志");
     LOG_ERROR("这是一条错误日志");
     LOG_FATAL("这是一条致命错误日志");
     
     /* 测试格式化参数 */
     LOG_INFO("带参数的日志: %d, %s, %.2f", 100, "字符串", 3.14159);
     
     /* 修改日志级别 */
     printf("修改日志级别为 WARN，低于此级别的日志将不会打印\n");
     log_set_level(LOG_LEVEL_WARN);
     
     LOG_DEBUG("此调试日志不应该显示");
     LOG_INFO("此信息日志不应该显示");
     LOG_WARN("此警告日志应该显示");
     LOG_ERROR("此错误日志应该显示");
     
     /* 销毁日志系统 */
     log_destroy();
 }
 
 /**
  * @brief 测试日志过滤功能
  */
 void test_filter_mode(void) {
     /* 初始化日志系统，使用过滤模式 */
     if (log_init("test_filter.log", LOG_LEVEL_DEBUG, LOG_MODE_FILTER) != 0) {
         printf("初始化日志系统失败\n");
         return;
     }
     
     /* 测试相同日志的过滤 */
     printf("测试相同日志在1小时内只打印一次：\n");
     
     LOG_INFO("这是一条重复的日志消息");
     LOG_INFO("这是一条重复的日志消息"); /* 应该被过滤 */
     LOG_INFO("这是另一条不同的日志消息");
     LOG_INFO("这是一条重复的日志消息"); /* 应该被过滤 */
     
     LOG_WARN("不同级别的相同内容日志1");
     LOG_ERROR("不同级别的相同内容日志1"); /* 不同级别视为不同日志 */
     
     /* 测试切换回普通模式 */
     printf("切换回普通模式，相同日志应该可以打印：\n");
     log_set_mode(LOG_MODE_NORMAL);
     
     LOG_INFO("这是一条重复的日志消息"); /* 普通模式下应该可以打印 */
     LOG_INFO("这是一条重复的日志消息"); /* 普通模式下应该可以打印 */
     
     /* 销毁日志系统 */
     log_destroy();
 }
 
 /**
  * @brief 测试海量日志屏蔽功能
  */
 void test_massive_logs(void) {
     /* 初始化日志系统，使用过滤模式 */
     if (log_init("test_massive.log", LOG_LEVEL_DEBUG, LOG_MODE_NORMAL) != 0) {
         printf("初始化日志系统失败\n");
         return;
     }
     
     printf("测试海量日志屏蔽功能 (1分钟内>=60条相同日志视为海量)：\n");
     printf("在短时间内打印大量相同日志...\n");
     
     /* 模拟产生海量日志 */
     for (int i = 0; i < 70; i++) {
         LOG_INFO("这是一条海量日志测试");
         usleep(1000); /* 每条日志间隔1毫秒 */
     }
     
     printf("已触发海量日志过滤，接下来1小时内该日志应只打印一次\n");
     
     /* 再次尝试打印相同日志 */
     for (int i = 0; i < 10; i++) {
         LOG_INFO("这是一条海量日志测试");
         usleep(100000); /* 间隔100毫秒 */
     }
     
     /* 测试其他日志是否正常 */
     LOG_INFO("这是另一条不同的日志，应该正常打印");
     
     /* 销毁日志系统 */
     log_destroy();
 }
 
 /**
  * @brief 多线程测试函数
  */
 void *thread_func(void *arg) {
     int thread_id = *(int *)arg;
     free(arg);
     
     /* 初始化日志系统，使用过滤模式 */
     char filename[64];
     snprintf(filename, sizeof(filename), "test_thread_%d.log", thread_id);
     
     if (log_init(filename, LOG_LEVEL_DEBUG, LOG_MODE_FILTER) != 0) {
         printf("线程 %d: 初始化日志系统失败\n", thread_id);
         return NULL;
     }
     
     /* 打印一些测试日志 */
     for (int i = 0; i < 10; i++) {
         LOG_INFO("线程 %d: 测试日志 #%d", thread_id, i);
         usleep(50000); /* 休眠50毫秒 */
     }
     
     /* 测试重复日志过滤 */
     for (int i = 0; i < 5; i++) {
         LOG_INFO("线程 %d: 这是一条重复日志", thread_id);
     }
     
     LOG_INFO("线程 %d: 完成测试", thread_id);
     
     /* 销毁日志系统 */
     log_destroy();
     
     return NULL;
 }