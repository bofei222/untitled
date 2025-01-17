#include <stdio.h>
#include "example.h"
#include "abi.h"  // 假设需要的外部头文件



int main() {
    struct ArrowArray array;
    // 调用 C++ 函数
    export_int32_data(&array);

    // 打印生成的数据长度
    printf("Array length: %ld\n", array.length);

    // 清理资源
    array.release(&array);

    return 0;
}
