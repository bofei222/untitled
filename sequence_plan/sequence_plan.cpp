#include <iostream>
#include <arrow/acero/api.h>    // plans and nodes
#include <arrow/compute/api.h>  // field refs and exprs
#include <arrow/io/api.h>       // ReadableFile
#include <arrow/api.h>
#include <arrow/result.h>
#include <arrow/table.h>
#include <parquet/arrow/reader.h>

int sequence_plan(const std::string& path) {

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

  auto result = std::move(_error_or_value255549294).ValueUnsafe();;;


  // Step 7: Execute the query plan and retrieve results

  // Step 8: Print results
  std::cout << "Results: " << result->ToString() << std::endl;

  return 0; // Success
}