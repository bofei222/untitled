#include "sequence_plan.h"
#include <iostream>

int main() {
    std::string path = "path/to/your/file.parquet";
    arrow::Status status = sequence_plan(path);
    if (!status.ok()) {
        std::cerr << "Error: " << status.ToString() << std::endl;
        return -1;
    }
    return 0;
}
