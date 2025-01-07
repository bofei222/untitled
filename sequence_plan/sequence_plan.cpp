#include "sequence_plan.h"

#include <iostream>
#include <arrow/acero/api.h>    // plans and nodes
#include <arrow/compute/api.h>  // field refs and exprs
#include <arrow/io/api.h>       // ReadableFile
#include <arrow/api.h>
#include <arrow/result.h>
#include <arrow/table.h>
#include <parquet/arrow/reader.h>

namespace aio = ::arrow::io;
namespace cp = ::arrow::compute;
namespace ac = ::arrow::acero;

arrow::Status sequence_plan(const std::string& path) {
  auto* pool = arrow::default_memory_pool();
  ARROW_ASSIGN_OR_RAISE(auto input, aio::ReadableFile::Open(path));

  std::unique_ptr<parquet::arrow::FileReader> arrow_reader;
  ARROW_RETURN_NOT_OK(parquet::arrow::OpenFile(input, pool, &arrow_reader));

  std::unique_ptr<arrow::RecordBatchReader> rdr;
  ARROW_RETURN_NOT_OK(arrow_reader->GetRecordBatchReader(&rdr));

  arrow::StringBuilder excl_bldr;
  ARROW_RETURN_NOT_OK(excl_bldr.Append("Skako"));
  ARROW_RETURN_NOT_OK(excl_bldr.Append("Utapau"));
  ARROW_RETURN_NOT_OK(excl_bldr.Append("Nal Hutta"));
  std::shared_ptr<arrow::StringArray> exclusions;
  ARROW_RETURN_NOT_OK(excl_bldr.Finish(&exclusions));

  auto filter_expr = cp::call("invert", {cp::call("is_in", {cp::field_ref("homeworld")},
                                                  cp::SetLookupOptions{*exclusions})});

  auto plan = ac::Declaration::Sequence(
      {{"record_batch_reader_source",
        ac::RecordBatchReaderSourceNodeOptions{std::move(rdr)}},
       {"filter", ac::FilterNodeOptions{std::move(filter_expr)}},
       {"aggregate",
        ac::AggregateNodeOptions({{{"hash_list", nullptr, "name", "name_list"},
                                   {"hash_list", nullptr, "species", "species_list"},
                                   {"hash_mean", nullptr, "height", "avg_height"}}},
                                 {"homeworld"})}});

  ARROW_ASSIGN_OR_RAISE(auto result, ac::DeclarationToTable(std::move(plan)));
  std::cout << "Results: " << result->ToString() << std::endl;
  return arrow::Status::OK();
}
