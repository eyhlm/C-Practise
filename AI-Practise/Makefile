# 编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -pthread -Iinclude
LDFLAGS = -pthread

# 目录定义
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# 目标文件（路径在 build 目录）
OBJS = $(BUILD_DIR)/logger.o $(BUILD_DIR)/log_filter.o $(BUILD_DIR)/logger_test.o

# 默认目标
all: $(BUILD_DIR) logger_test

# 创建 build 目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 编译规则：从 src 编译到 build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 显式声明依赖关系（解决头文件修改触发重新编译）
$(BUILD_DIR)/logger.o: $(SRC_DIR)/logger.c $(INCLUDE_DIR)/logger.h $(INCLUDE_DIR)/log_filter.h
$(BUILD_DIR)/log_filter.o: $(SRC_DIR)/log_filter.c $(INCLUDE_DIR)/log_filter.h
$(BUILD_DIR)/logger_test.o: $(SRC_DIR)/logger_test.c $(INCLUDE_DIR)/logger.h

# 链接测试程序
logger_test: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# 清理目标
clean:
	rm -rf $(BUILD_DIR) logger_test test_*.log

# 运行测试
test: logger_test
	./logger_test

# 创建静态库
liblogger.a: $(BUILD_DIR)/logger.o $(BUILD_DIR)/log_filter.o
	ar rcs $@ $^

# 创建共享库
liblogger.so: $(BUILD_DIR)/logger.o $(BUILD_DIR)/log_filter.o
	$(CC) -shared -o $@ $^

# 安装（需要sudo权限）
install: liblogger.a liblogger.so
	mkdir -p /usr/local/include/logger
	cp $(INCLUDE_DIR)/*.h /usr/local/include/logger/
	cp liblogger.a liblogger.so /usr/local/lib/
	ldconfig

# 卸载
uninstall:
	rm -rf /usr/local/include/logger
	rm -f /usr/local/lib/liblogger.a /usr/local/lib/liblogger.so
	ldconfig

.PHONY: all clean test install uninstall