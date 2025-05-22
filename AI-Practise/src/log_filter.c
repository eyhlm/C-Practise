/**
 * @file log_filter.c
 * @brief 日志过滤功能实现
 * 
 * 实现了基于哈希表的日志过滤机制，用于处理日志重复打印和海量日志过滤
 */

 #include "log_filter.h"
 #include <stdlib.h>
 #include <string.h>
 #include <stdio.h>
 
 /* 哈希表大小，选择一个合适的质数 */
 #define HASH_TABLE_SIZE 997
 
 /* 定义日志记录的结构 */
 typedef struct log_record {
     char *content;                /* 日志内容 */
     size_t content_len;           /* 日志内容长度 */
     time_t first_time;            /* 首次出现时间 */
     time_t last_time;             /* 最近出现时间 */
     unsigned int count_total;     /* 总计出现次数 */
     unsigned int count_last_min;  /* 最近一分钟出现次数 */
     time_t last_min_start;        /* 当前"一分钟"的开始时间 */
     bool is_massive;              /* 是否被标记为海量日志 */
     struct log_record *next;      /* 链表下一个节点 */
 } log_record_t;
 
 /* 过滤器状态 */
 static struct {
     log_record_t *hash_table[HASH_TABLE_SIZE]; /* 哈希表 */
     bool initialized;                           /* 初始化标志 */
 } filter_state = {
     .hash_table = {NULL},
     .initialized = false
 };
 
 /**
  * @brief 计算字符串的哈希值
  * 
  * 使用简单的BKDR哈希算法
  * 
  * @param str 字符串
  * @param len 字符串长度
  * @return 哈希值
  */
 static unsigned int hash_string(const char *str, size_t len) {
     unsigned int seed = 131; /* 31, 131, 1313, 13131... 都是不错的种子 */
     unsigned int hash = 0;
     
     for (size_t i = 0; i < len; i++) {
         hash = hash * seed + (unsigned char)str[i];
     }
     
     return hash % HASH_TABLE_SIZE;
 }
 
 /**
  * @brief 在哈希表中查找或创建日志记录
  * 
  * @param content 日志内容
  * @param content_len 日志内容长度
  * @return 日志记录指针，如果是新创建的，则需要初始化
  */
 static log_record_t *find_or_create_record(const char *content, size_t content_len) {
     unsigned int hash = hash_string(content, content_len);
     log_record_t *record = filter_state.hash_table[hash];
     
     /* 在链表中查找记录 */
     while (record) {
         if (record->content_len == content_len && 
             memcmp(record->content, content, content_len) == 0) {
             return record; /* 找到匹配的记录 */
         }
         record = record->next;
     }
     
     /* 未找到匹配记录，创建新记录 */
     record = (log_record_t *)malloc(sizeof(log_record_t));
     if (!record) {
         perror("malloc failed for log_record");
         return NULL;
     }
     
     /* 申请内存保存日志内容 */
     record->content = (char *)malloc(content_len + 1);
     if (!record->content) {
         perror("malloc failed for log content");
         free(record);
         return NULL;
     }
     
     /* 初始化新记录 */
     memcpy(record->content, content, content_len);
     record->content[content_len] = '\0';
     record->content_len = content_len;
     record->first_time = time(NULL);
     record->last_time = record->first_time;
     record->count_total = 0; /* 初始化为0，在filter_check中增加 */
     record->count_last_min = 0;
     record->last_min_start = record->first_time;
     record->is_massive = false;
     
     /* 将新记录插入链表头部 */
     record->next = filter_state.hash_table[hash];
     filter_state.hash_table[hash] = record;
     
     return record;
 }
 
 /**
  * @brief 清理日志记录，释放内存
  */
 static void clean_records(void) {
     for (int i = 0; i < HASH_TABLE_SIZE; i++) {
         log_record_t *record = filter_state.hash_table[i];
         while (record) {
             log_record_t *next = record->next;
             if (record->content) {
                 free(record->content);
             }
             free(record);
             record = next;
         }
         filter_state.hash_table[i] = NULL;
     }
 }
 
 int filter_init(void) {
     /* 已经初始化则直接返回 */
     if (filter_state.initialized) {
         return 0;
     }
     
     /* 初始化哈希表 */
     memset(filter_state.hash_table, 0, sizeof(filter_state.hash_table));
     filter_state.initialized = true;
     
     return 0;
 }
 
 void filter_destroy(void) {
     if (!filter_state.initialized) {
         return;
     }
     
     clean_records();
     filter_state.initialized = false;
 }
 
 /**
  * @brief 检查日志是否为海量日志或重复日志，并决定是否过滤
  * 
  * @param log_content 日志内容
  * @param log_len 日志内容长度
  * @param filter_mode 是否开启过滤模式
  * @return true 表示应该过滤此日志，false 表示应该打印此日志
  */
 bool filter_check_massive(const char *log_content, size_t log_len) {
     time_t now;
     log_record_t *record;
     
     if (!filter_state.initialized || !log_content || log_len == 0) {
         return false; /* 不过滤 */
     }
     
     now = time(NULL);
     
     /* 查找或创建日志记录 */
     record = find_or_create_record(log_content, log_len);
     if (!record) {
         return false; /* 创建记录失败，不过滤 */
     }
     
     /* 更新计数和时间 */
     record->count_total++;
     record->last_time = now;
     
     /* 更新最近一分钟计数器 */
     if (now - record->last_min_start >= 60) {
         /* 重置一分钟计数 */
         record->count_last_min = 1;
         record->last_min_start = now;
     } else {
         record->count_last_min++;
         
         /* 检查是否达到海量日志阈值 (1分钟内>=60条) */
         if (record->count_last_min >= 60 && !record->is_massive) {
             /* 首次检测到海量日志，标记并允许打印一条提示 */
             record->is_massive = true;
             return false; /* 打印一条提示日志 */
         }
     }
     
     /* 检查是否是已标记的海量日志 */
     if (record->is_massive) {
         /* 检查是否超过一小时需要重置 */
         if (now - record->first_time >= 3600) {
             /* 超过一小时，重置记录 */
             record->first_time = now;
             record->count_total = 1;
             record->count_last_min = 1;
             record->last_min_start = now;
             record->is_massive = false;
             return false; /* 重置后不过滤 */
         }
         
         /* 是海量日志且在一小时内，应该过滤 */
         return true;
     }
     
     return false; /* 不是海量日志，不过滤 */
 }

 bool filter_check(const char *log_content, size_t log_len) {
     time_t now;
     log_record_t *record;
     
     if (!filter_state.initialized || !log_content || log_len == 0) {
         return false; /* 不过滤 */
     }
     
     now = time(NULL);
     
     /* 查找或创建日志记录 */
     record = find_or_create_record(log_content, log_len);
     if (!record) {
         return false; /* 创建记录失败，不过滤 */
     }
     
     /* 更新计数和时间 */
     record->count_total++;
     record->last_time = now;
     
     /* 更新最近一分钟计数器 */
     if (now - record->last_min_start >= 60) {
         record->count_last_min = 1;
         record->last_min_start = now;
     } else {
         record->count_last_min++;
     }
     
     /* 检查是否是首次出现 */
     if (record->count_total == 1) {
         /* 首次出现，不过滤 */
         return false;
     }
     
     /* 检查是否是重复日志（计数>1且在一小时内） */
     if ((now - record->first_time) < 3600) {
         /* 检查是否达到海量日志阈值 */
         if (record->count_last_min >= 60 && !record->is_massive) {
             record->is_massive = true;
             /* 第一次检测到海量日志时允许打印一条警告 */
             return false;
         }
         
         /* 如果是海量日志且已经警告过，继续过滤 */
         if (record->is_massive) {
             return true;
         }
         
         /* 一小时内的重复日志，应该过滤 */
         return true;
     } else {
         /* 超过一小时，重置记录 */
         record->first_time = now;
         record->last_time = now;
         record->count_total = 1;
         record->count_last_min = 1;
         record->last_min_start = now;
         record->is_massive = false;
         return false; /* 不过滤 */
     }
 }