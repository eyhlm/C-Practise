# 项目名称
Gtest
## 项目简介
基于claude3.7与DeepSeek开发的gtest程序

（claude3.7代码能力强，但是目前免费版有字数限制且没有深度思考，在某些问题分析上逊于DeepSeek）

## 安装指南
```bash
#在此文件夹下克隆googletest项目 或 把CMakeLists中的GTEST_DIR指定为googletest路径
mkdir build
cd build
cmake ../
make
./log_filter_test
./logger_test