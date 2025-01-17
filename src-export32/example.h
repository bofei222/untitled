#ifndef SEQUENCE_PLAN_H
#define SEQUENCE_PLAN_H
#include "abi.h"  // 假设需要的外部头文件
#ifdef __cplusplus

extern "C" {
#endif

    // 修改返回类型为 int
    void export_int32_data(struct ArrowArray*);

#ifdef __cplusplus
}
#endif

#endif // SEQUENCE_PLAN_H
