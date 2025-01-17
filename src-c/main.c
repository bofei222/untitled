#include <stdio.h>
#include "sequence_plan2.h"  // 引用 C++ 接口

int main(int argc, char** argv) {
    int plan = sequence_plan(argc);  // 调用 C++ 接口函数
    // print plan
    printf("Plan: %d\n", plan);
    return 0;
}
