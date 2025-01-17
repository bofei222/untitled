#ifndef ARROW_CALCULATION_H
#define ARROW_CALCULATION_H

#include <arrow/c/bridge.h>
#include <arrow/acero/exec_plan.h>
#include <arrow/acero/options.h>
#include <arrow/acero/table_source_node.h>
#include <arrow/compute/api.h>
#include <arrow/array.h>
#include <arrow/table.h>
#include <memory>

// 函数声明：接受 CArrowArray 和 CArrowSchema，使用 Acero 进行简单计算
extern "C" int PerformSimpleCalculation(struct ArrowArray* array, struct ArrowSchema* schema);

#endif // ARROW_CALCULATION_H
