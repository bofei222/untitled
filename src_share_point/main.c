#include <stdio.h>

// 声明 C++ 提供的函数
extern int* create_data();  // 创建数据
extern void delete_data(int* data);  // 删除数据
extern int process_data(int* data);  // 处理数据

int main() {
    // 使用 C 风格接口与 C++ 代码交互

    // 创建数据
    int* data = create_data();

    // 处理数据
    int result = process_data(data);
    printf("Processed result222: %d\n", result);  // 输出：Processed result: 84

    // 删除数据
    delete_data(data);

    return 0;
}
