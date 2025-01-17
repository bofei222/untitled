#include <iostream>
#include <arrow/acero/api.h>    // plans and nodes
#include <arrow/compute/api.h>  // field refs and exprs
#include <arrow/io/api.h>       // ReadableFile
#include <arrow/api.h>
#include <arrow/result.h>
#include <arrow/table.h>
#include <parquet/arrow/reader.h>
#include <sequence_plan2.h>  // 包含要包装的头文件

// 修改返回类型为 int
int sequence_plan(int argc) {
  auto* pool = arrow::default_memory_pool();
  // print argc
  std::cout << "argc: " << argc << std::endl;
  std::string path = "/home/bf/CLionProjects/sample_data/starwars.parquet";

  // Step 1: Open the file
  auto input_status = arrow::io::ReadableFile::Open(path);
  if (!input_status.ok()) {
    return -1; // Error code -1: Failed to open file
  }
  auto input = std::move(input_status).ValueOrDie();  // Get the file handle

  // Step 2: Open the Parquet file
  std::unique_ptr<parquet::arrow::FileReader> arrow_reader;
  auto open_file_status = parquet::arrow::OpenFile(input, pool, &arrow_reader);
  if (!open_file_status.ok()) {
    return -2; // Error code -2: Failed to open Parquet file
  }

  // Step 3: Get RecordBatchReader
  std::unique_ptr<arrow::RecordBatchReader> rdr;
  auto record_batch_status = arrow_reader->GetRecordBatchReader(&rdr);
  if (!record_batch_status.ok()) {
    return -3; // Error code -3: Failed to get RecordBatchReader
  }

  // Step 4: Build exclusions
  arrow::StringBuilder excl_bldr;
  auto append_status = excl_bldr.Append("Skako");
  if (!append_status.ok()) {
    return -4; // Error code -4: Failed to append to StringBuilder
  }

  append_status = excl_bldr.Append("Utapau");
  if (!append_status.ok()) {
    return -5; // Error code -5: Failed to append to StringBuilder
  }

  append_status = excl_bldr.Append("Nal Hutta");
  if (!append_status.ok()) {
    return -6; // Error code -6: Failed to append to StringBuilder
  }

  std::shared_ptr<arrow::StringArray> exclusions;
  auto finish_status = excl_bldr.Finish(&exclusions);
  if (!finish_status.ok()) {
    return -7; // Error code -7: Failed to finish StringBuilder
  }

  // Step 5: Build filter expression
  auto filter_expr = arrow::compute::call("invert", {
    arrow::compute::call("is_in", {arrow::compute::field_ref("homeworld")},
    arrow::compute::SetLookupOptions{*exclusions})
  });

  // Step 6: Build the query plan
  auto plan = arrow::acero::Declaration::Sequence(
      {{"record_batch_reader_source",
        arrow::acero::RecordBatchReaderSourceNodeOptions{std::move(rdr)}},
       {"filter", arrow::acero::FilterNodeOptions{std::move(filter_expr)}},
       {"aggregate",
        arrow::acero::AggregateNodeOptions({{{"hash_list", nullptr, "name", "name_list"},
                                             {"hash_list", nullptr, "species", "species_list"},
                                             {"hash_mean", nullptr, "height", "avg_height"}}},
                                           {"homeworld"})}});

  auto &&_error_or_value255549294 = (DeclarationToTable(std::move(plan)));

  auto result = std::move(_error_or_value255549294).ValueUnsafe();

  // Step 8: Print results
  std::cout << "Results: " << result->ToString() << std::endl;

  return 0; // Success
}
