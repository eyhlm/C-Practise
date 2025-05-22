#include <gtest/gtest.h>
#include "calculator.h"

// 测试加法函数
TEST(CalculatorTest, AddTest) {
    EXPECT_EQ(5, add(2, 3));
    EXPECT_EQ(-1, add(-4, 3));
    EXPECT_EQ(0, add(0, 0));
}

// 测试减法函数
TEST(CalculatorTest, SubtractTest) {
    EXPECT_EQ(-1, subtract(2, 3));
    EXPECT_EQ(-7, subtract(-4, 3));
    EXPECT_EQ(0, subtract(0, 0));
}

// 测试乘法函数
TEST(CalculatorTest, MultiplyTest) {
    EXPECT_EQ(6, multiply(2, 3));
    EXPECT_EQ(-12, multiply(-4, 3));
    EXPECT_EQ(0, multiply(0, 5));
}

// 测试除法函数
TEST(CalculatorTest, DivideTest) {
    EXPECT_EQ(2, divide(6, 3));
    EXPECT_EQ(-2, divide(-6, 3));
    EXPECT_EQ(0, divide(5, 0)); // 测试除零情况
}

// 主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
