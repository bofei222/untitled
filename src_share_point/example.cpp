#include <iostream>
#include <memory>

// C 风格的接口声明，确保不涉及智能指针
extern "C" {
    // 创建数据
    int* create_data();

    // 删除数据
    void delete_data(int* data);

    // 处理数据
    int process_data(int* data);
}

// C++ 实现部分
std::unique_ptr<int> create_data_impl() {
    // 使用 unique_ptr 动态分配内存
    return std::make_unique<int>(42);  // 创建一个值为 42 的整数
}

extern "C" int* create_data() {
    // 使用 unique_ptr 管理内存，但返回裸指针
    std::unique_ptr<int> ptr = create_data_impl();
    return ptr.release();  // release 返回裸指针，释放智能指针的控制
}

extern "C" void delete_data(int* data) {
    // 手动释放裸指针
    delete data;
}

extern "C" int process_data(int* data) {
    // 使用裸指针处理数据
    return *data * 2;  // 示例：简单的数值处理
}

