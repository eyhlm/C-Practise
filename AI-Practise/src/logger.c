/**
 * @file logger.c
 * @brief 日志系统实现
 * 
 * 实现了日志系统的各项功能，包括初始化、销毁、设置日志级别和日志打印
 */

 #include "logger.h"
 #include "log_filter.h"
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <pthread.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/time.h>
 #include <stdarg.h>
 
 /* 日志缓冲区大小 */
 #define LOG_BUFFER_SIZE 4096
 /* 用户消息缓冲区大小 */
 #define USER_MSG_BUFFER_SIZE 2048
 
 /* 日志系统状态 */
 static struct {
     FILE *log_file;              /* 日志文件句柄 */
     log_level_t log_level;       /* 当前日志级别 */
     log_mode_t log_mode;         /* 当前日志模式 */
     bool initialized;            /* 初始化标志 */
     pthread_mutex_t mutex;       /* 互斥锁，保证多线程安全 */
     char buffer[LOG_BUFFER_SIZE]; /* 日志缓冲区 */
     char user_msg[USER_MSG_BUFFER_SIZE]; /* 用户消息缓冲区，用于过滤 */
 } logger_state = {
     .log_file = NULL,
     .log_level = LOG_LEVEL_INFO,
     .log_mode = LOG_MODE_NORMAL,
     .initialized = false
 };
 
 /* 日志级别对应的字符串表示 */
 static const char *level_strings[] = {
     "DEBUG",
     "INFO",
     "WARN",
     "ERROR",
     "FATAL"
 };
 
 /* 日志级别对应的颜色代码 */
 static const char *level_colors[] = {
     "\033[36m", /* 青色 - DEBUG */
     "\033[32m", /* 绿色 - INFO */
     "\033[33m", /* 黄色 - WARN */
     "\033[31m", /* 红色 - ERROR */
     "\033[35m"  /* 紫色 - FATAL */
 };
 
 /* 重置颜色的ANSI转义序列 */
 static const char *color_reset = "\033[0m";
 
 int log_init(const char *filename, log_level_t level, log_mode_t mode) {
     /* 已经初始化则先销毁 */
     if (logger_state.initialized) {
         log_destroy();
     }
     
     /* 初始化互斥锁 */
     if (pthread_mutex_init(&logger_state.mutex, NULL) != 0) {
         perror("pthread_mutex_init failed");
         return -1;
     }
     
     /* 设置日志级别和模式 */
     logger_state.log_level = level;
     logger_state.log_mode = mode;
     
     /* 如果提供了文件名，打开日志文件 */
     if (filename) {
         logger_state.log_file = fopen(filename, "a");
         if (!logger_state.log_file) {
             perror("Failed to open log file");
             pthread_mutex_destroy(&logger_state.mutex);
             return -1;
         }
         
         /* 设置文件缓冲区为行缓冲，确保日志及时写入 */
         setvbuf(logger_state.log_file, NULL, _IOLBF, 0);
     }
     
     /* 初始化日志过滤器 */
     if (filter_init() != 0) {
         if (logger_state.log_file) {
             fclose(logger_state.log_file);
             logger_state.log_file = NULL;
         }
         pthread_mutex_destroy(&logger_state.mutex);
         return -1;
     }
     
     logger_state.initialized = true;
     
     /* 打印初始化成功的日志 */
     log_print(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, 
             "Log system initialized successfully (level=%s, mode=%s, file=%s)",
             level_strings[level], mode == LOG_MODE_NORMAL ? "normal" : "filter",
             filename ? filename : "stdout");
     
     return 0;
 }
 
 void log_destroy(void) {
     if (!logger_state.initialized) {
         return;
     }
     
     pthread_mutex_lock(&logger_state.mutex);
     
     /* 关闭日志文件 */
     if (logger_state.log_file) {
         fclose(logger_state.log_file);
         logger_state.log_file = NULL;
     }
     
     logger_state.initialized = false;
     
     pthread_mutex_unlock(&logger_state.mutex);
     pthread_mutex_destroy(&logger_state.mutex);
     
     /* 销毁日志过滤器 */
     filter_destroy();
 }
 
 void log_set_level(log_level_t level) {
     if (level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_FATAL) {
         pthread_mutex_lock(&logger_state.mutex);
         logger_state.log_level = level;
         pthread_mutex_unlock(&logger_state.mutex);
     }
 }
 
 void log_set_mode(log_mode_t mode) {
     pthread_mutex_lock(&logger_state.mutex);
     logger_state.log_mode = mode;
     pthread_mutex_unlock(&logger_state.mutex);
 }
 
 void log_print(log_level_t level, const char *file, int line, const char *func, const char *fmt, ...) {
     struct timeval tv;
     struct tm *tm_info;
     time_t timer;
     char time_str[32]; /* 时间字符串缓冲区 */
     va_list args;
     bool should_filter = false;
     int log_len;
     int user_msg_len;
     
     /* 检查日志级别 */
     if (level < logger_state.log_level || !logger_state.initialized) {
         return;
     }
     
     /* 获取当前时间 */
     gettimeofday(&tv, NULL);
     timer = tv.tv_sec;
     tm_info = localtime(&timer);
     
     /* 格式化时间字符串 */
     strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
     
     pthread_mutex_lock(&logger_state.mutex);
     
     /* 首先格式化用户消息部分，仅用于过滤比较 */
     va_start(args, fmt);
     user_msg_len = vsnprintf(logger_state.user_msg, USER_MSG_BUFFER_SIZE, fmt, args);
     va_end(args);
     
     /* 确保用户消息以换行符结束 */
     if (user_msg_len > 0 && user_msg_len < USER_MSG_BUFFER_SIZE - 1 && 
         logger_state.user_msg[user_msg_len - 1] != '\n') {
         logger_state.user_msg[user_msg_len++] = '\n';
         logger_state.user_msg[user_msg_len] = '\0';
     }
     
     /* 创建过滤键，格式: "LEVEL:MESSAGE" */
     char filter_key[USER_MSG_BUFFER_SIZE + 10];
     snprintf(filter_key, sizeof(filter_key), "%s:%s", level_strings[level], logger_state.user_msg);
     
     /* 检查是否需要过滤 */
     if (logger_state.log_mode == LOG_MODE_FILTER) {
         /* 在过滤模式下，检查普通过滤和海量日志过滤 */
         should_filter = filter_check(filter_key, strlen(filter_key));
     } else {
         /* 在普通模式下，仅检查海量日志过滤 */
         should_filter = filter_check_massive(filter_key, strlen(filter_key));
     }
     
     /* 如果不过滤，则格式化完整日志并打印 */
     if (!should_filter) {
         /* 格式化日志前缀 */
         log_len = snprintf(logger_state.buffer, LOG_BUFFER_SIZE,
                         "%s.%03ld [%s] [%s:%d %s] ",
                         time_str, tv.tv_usec / 1000, level_strings[level], 
                         file, line, func);
         
         /* 添加用户日志内容 */
         va_start(args, fmt);
         log_len += vsnprintf(logger_state.buffer + log_len, LOG_BUFFER_SIZE - log_len, fmt, args);
         va_end(args);
         
         /* 确保字符串以换行符结束 */
         if (log_len > 0 && log_len < LOG_BUFFER_SIZE - 1 && logger_state.buffer[log_len - 1] != '\n') {
             logger_state.buffer[log_len++] = '\n';
             logger_state.buffer[log_len] = '\0';
         }
         
         /* 输出到标准输出 */
         fprintf(stdout, "%s%s%s", level_colors[level], logger_state.buffer, color_reset);
         fflush(stdout);
         
         /* 输出到日志文件 */
         if (logger_state.log_file) {
             fprintf(logger_state.log_file, "%s", logger_state.buffer);
             fflush(logger_state.log_file);
         }
     }
     
     pthread_mutex_unlock(&logger_state.mutex);
 }