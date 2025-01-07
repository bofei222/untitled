#ifndef SEQUENCE_PLAN_H
#define SEQUENCE_PLAN_H

#include <string>
#include <arrow/status.h>

arrow::Status sequence_plan(const std::string& path);

#endif  // SEQUENCE_PLAN_H
