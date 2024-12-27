#include <iostream>
#include <string>
#include <arrow/acero/api.h>    // plans and nodes
#include <arrow/compute/api.h>  // field refs and exprs
#include <arrow/io/api.h>       // ReadableFile
#include <arrow/api.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>
#include <parquet/arrow/reader.h>
#include "sequence_plan.h"

namespace aio = ::arrow::io;
namespace ac = ::arrow::acero;

extern "C" const char* sequence_plan(const char* c_path) {
  static std::string result_str;  // 用于返回结果
  std::string path(c_path);       // 将 C 风格字符串转为 C++ 字符串

  auto* pool = arrow::default_memory_pool();

  // 打开文件
  auto result = aio::ReadableFile::Open(path);
  if (!result.ok()) {
    result_str = "Error opening file: " + result.status().ToString();
    return result_str.c_str();
  }
  auto input = *result;

  // 初始化 Parquet 文件读取器
  std::unique_ptr<parquet::arrow::FileReader> arrow_reader;
  auto status = parquet::arrow::OpenFile(input, pool, &arrow_reader);
  if (!status.ok()) {
    result_str = "Error initializing Arrow reader: " + status.ToString();
    return result_str.c_str();
  }

  // 创建 RecordBatchReader
  std::unique_ptr<arrow::RecordBatchReader> rdr;
  status = arrow_reader->GetRecordBatchReader(&rdr);
  if (!status.ok()) {
    result_str = "Error creating RecordBatchReader: " + status.ToString();
    return result_str.c_str();
  }

  // 定义执行计划（无过滤表达式）
  auto plan = ac::Declaration::Sequence({
      {"record_batch_reader_source",
       ac::RecordBatchReaderSourceNodeOptions{std::move(rdr)}},
      {"aggregate",
       ac::AggregateNodeOptions({{{"hash_list", nullptr, "name", "name_list"},
                                  {"hash_list", nullptr, "species", "species_list"},
                                  {"hash_mean", nullptr, "height", "avg_height"}}},
                                {"homeworld"})}});

  // 执行计划并返回结果
  auto table_result = ac::DeclarationToTable(std::move(plan));
  if (!table_result.ok()) {
    result_str = "Error executing plan: " + table_result.status().ToString();
    return result_str.c_str();
  }

  auto table = *table_result;
  result_str = table->ToString();
  return result_str.c_str();
}
