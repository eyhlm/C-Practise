/**
 * @file log_filter.h
 * @brief 日志过滤功能头文件
 * 
 * 定义了日志过滤系统的接口，用于处理日志重复打印和海量日志过滤
 */

 #ifndef _LOG_FILTER_H_
 #define _LOG_FILTER_H_
 
 #include <stdbool.h>
 #include <time.h>
 
 /**
  * @brief 初始化日志过滤器
  * 
  * @return 成功返回0，失败返回-1
  */
 int filter_init(void);
 
 /**
  * @brief 销毁日志过滤器，释放资源
  */
 void filter_destroy(void);
 
 /**
  * @brief 检查日志是否应该被过滤
  * 
  * 实现以下过滤逻辑：
  * 1. 如果1小时内有相同日志，则过滤（仅保留第一条）
  * 2. 如果1分钟内同一日志出现>=60次，视为海量日志，接下来1小时内只打印一次
  * 
  * @param log_content 日志内容
  * @param log_len 日志内容长度
  * @return true 表示应该过滤此日志，false 表示应该打印此日志
  */
 bool filter_check(const char *log_content, size_t log_len);
 
 /**
  * @brief 检查日志是否为海量日志
  * 
  * 仅实现海量日志过滤逻辑：
  * 1. 如果1分钟内同一日志出现>=60次，视为海量日志，接下来1小时内只打印一次
  * 
  * @param log_content 日志内容
  * @param log_len 日志内容长度
  * @return true 表示应该过滤此日志，false 表示应该打印此日志
  */
 bool filter_check_massive(const char *log_content, size_t log_len);
 
 #endif /* _LOG_FILTER_H_ */