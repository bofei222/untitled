#include <iostream>
#include <vector>
#include <arrow/c/abi.h>
#include "arrow_utils.h"

int main() {
    // 示例：创建一个 ArrowSchema 和 ArrowArray 类型的对象
    // 创建一个字段
    ArrowSchema schema;
    schema.type = ARROW_INT32;
    schema.name = "example_field";

    // 创建一个 ArrowArray
    ArrowArray array;
    array.length = 5;
    array.null_count = 0;

    // 假设这里的数据是 [1, 2, 3, 4, 5]
    std::vector<int32_t> values = {1, 2, 3, 4, 5};
    array.buffers = new const uint8_t*[2];
    array.buffers[1] = reinterpret_cast<const uint8_t*>(values.data());

    // 调用打印函数
    PrintArrowFieldAndArray(&schema, &array);

    // 释放资源（仅为示例，实际使用时注意内存管理）
    delete[] array.buffers;

    return 0;
}
