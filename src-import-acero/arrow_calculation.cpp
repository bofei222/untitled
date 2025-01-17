#include "arrow_calculation.h"
#include <iostream>

extern "C" int PerformSimpleCalculation(struct ArrowArray* array, struct ArrowSchema* schema) {
    // Step 1: 将 CArrowArray 和 CArrowSchema 转换为 RecordBatch
    std::shared_ptr<arrow::RecordBatch> record_batch;
    auto status  = arrow::ImportRecordBatch(array, schema);



    if (!status.ok()) {
        std::cerr << "Failed to import RecordBatch: " << status.status() << std::endl;
        return -1;
    }
    record_batch = std::move(status).ValueOrDie();



    // 打印导入的 RecordBatch
    std::cout << "Imported RecordBatch:" << std::endl;
    std::cout << record_batch->ToString() << std::endl;

    // Step 2: 创建 Acero ExecPlan，执行简单计算（例如：将第一列加 10）
    arrow::acero::ExecContext exec_context;
    auto plan_result = arrow::acero::ExecPlan::Make(&exec_context);
    if (!plan_result.ok()) {
        std::cerr << "Failed to create ExecPlan: " << plan_result.status().ToString() << std::endl;
        return -1;
    }
    auto plan = plan_result.ValueUnsafe();

    // 构建一个 TableSourceNode，用于从 RecordBatch 读取数据
    auto source_node_options = arrow::acero::TableSourceNodeOptions{record_batch, {}};
    auto source_node_result = arrow::acero::MakeExecNode("table_source", plan.get(), {}, source_node_options);
    if (!source_node_result.ok()) {
        std::cerr << "Failed to create TableSourceNode: " << source_node_result.status().ToString() << std::endl;
        return -1;
    }
    auto source_node = source_node_result.ValueUnsafe();

    // 构建一个计算节点，用于对第一列加 10
    arrow::compute::Expression expr = arrow::compute::call("add", {arrow::compute::field_ref(0), arrow::compute::literal(10)});
    auto project_node_options = arrow::acero::ProjectNodeOptions{{expr}, {"new_col"}};
    auto project_node_result = arrow::acero::MakeExecNode("project", plan.get(), {source_node}, project_node_options);
    if (!project_node_result.ok()) {
        std::cerr << "Failed to create ProjectNode: " << project_node_result.status().ToString() << std::endl;
        return -1;
    }
    auto project_node = project_node_result.ValueUnsafe();

    // 设置 Plan 的 Sink（用于输出）
    auto sink_node_options = arrow::acero::SinkNodeConsumer();
    auto sink_node_result = arrow::acero::MakeExecNode("sink", plan.get(), {project_node}, sink_node_options);
    if (!sink_node_result.ok()) {
        std::cerr << "Failed to create SinkNode: " << sink_node_result.status().ToString() << std::endl;
        return -1;
    }

    // 执行 ExecPlan
    plan->StartProducing();
    plan->finished().Wait();

    // 成功执行计算
    std::cout << "Calculation completed successfully!" << std::endl;
    return 0;
}
