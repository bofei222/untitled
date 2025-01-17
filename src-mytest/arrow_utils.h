#ifndef ARROW_UTILS_H
#define ARROW_UTILS_H

#include <arrow/c/abi.h>

#ifdef __cplusplus
extern "C" {
#endif

    // 声明用于打印 Arrow Schema 和 Array 的函数
    void PrintArrowFieldAndArray(ArrowSchema* arrowField, ArrowArray* arrowArray);

#ifdef __cplusplus
}
#endif

#endif // ARROW_UTILS_H
