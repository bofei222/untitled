%module sequence_plan  // 生成的模块名

%{
#include "sequence_plan.h"  // 包含要包装的头文件
#include <iostream>
#include <arrow/acero/api.h>    // plans and nodes
#include <arrow/compute/api.h>  // field refs and exprs
#include <arrow/io/api.h>       // ReadableFile
#include <arrow/api.h>
#include <arrow/result.h>
#include <arrow/table.h>
#include <parquet/arrow/reader.h>

%}

%include <std_string.i>  // 包含对 std::string 类型的支持

// SWIG 将 C++ 函数包装成可供 Go 等语言调用的接口
// 这里我们声明 sequence_plan 函数，这将自动使其可以在目标语言中使用
%inline %{
int sequence_plan(const std::string& path);
%}


