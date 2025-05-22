/**
 * @file logger.h
 * @brief 日志系统头文件
 * 
 * 定义了日志系统的接口，包括日志级别、初始化、销毁和日志打印函数
 */

 #ifndef _LOGGER_H_
 #define _LOGGER_H_
 
 #include <stdio.h>
 #include <stdarg.h>
 #include <stdbool.h>
 
 /**
  * 日志级别枚举
  */
 typedef enum {
     LOG_LEVEL_DEBUG = 0,  /**< 调试级别 */
     LOG_LEVEL_INFO,       /**< 信息级别 */
     LOG_LEVEL_WARN,       /**< 警告级别 */
     LOG_LEVEL_ERROR,      /**< 错误级别 */
     LOG_LEVEL_FATAL       /**< 致命错误级别 */
 } log_level_t;
 
 /**
  * 日志打印模式
  */
 typedef enum {
     LOG_MODE_NORMAL = 0,  /**< 普通打印模式 */
     LOG_MODE_FILTER       /**< 过滤打印模式 */
 } log_mode_t;
 
 /**
  * @brief 初始化日志系统
  * 
  * @param filename 日志文件名，如果为NULL则只输出到标准输出
  * @param level 日志级别，低于此级别的日志不会被打印
  * @param mode 日志打印模式
  * @return 成功返回0，失败返回-1
  */
 int log_init(const char *filename, log_level_t level, log_mode_t mode);
 
 /**
  * @brief 销毁日志系统，释放资源
  */
 void log_destroy(void);
 
 /**
  * @brief 设置日志级别
  * 
  * @param level 新的日志级别
  */
 void log_set_level(log_level_t level);
 
 /**
  * @brief 设置日志模式
  * 
  * @param mode 新的日志模式
  */
 void log_set_mode(log_mode_t mode);
 
 /**
  * @brief 日志打印函数
  * 
  * @param level 日志级别
  * @param file 调用处的文件名
  * @param line 调用处的行号
  * @param func 调用处的函数名
  * @param fmt 格式化字符串
  * @param ... 参数列表
  */
 void log_print(log_level_t level, const char *file, int line, const char *func, const char *fmt, ...);
 
 /**
  * 日志打印宏，方便调用
  */
 #define LOG_DEBUG(fmt, ...) log_print(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
 #define LOG_INFO(fmt, ...)  log_print(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
 #define LOG_WARN(fmt, ...)  log_print(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
 #define LOG_ERROR(fmt, ...) log_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
 #define LOG_FATAL(fmt, ...) log_print(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
 
 #endif /* _LOGGER_H_ */