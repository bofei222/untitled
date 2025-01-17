// example.cpp

#include "example.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <arrow/api.h>
#include "arrow/testing/gtest_util.h"
#include "arrow/type_fwd.h"

std::vector<int32_t> generate_data(size_t size) {
    static std::uniform_int_distribution<int32_t> dist(std::numeric_limits<int32_t>::min(),
                                                       std::numeric_limits<int32_t>::max());
    static std::random_device rnd_device;
    std::default_random_engine generator(rnd_device());
    std::vector<int32_t> data(size);
    std::generate(data.begin(), data.end(), [&]() { return dist(generator); });
    return data;
}

void export_int32_data(struct ArrowArray* array) {



    // auto dateArray = arrow::ArrayFromJSON(arrow::date32(), "[17471000]");
    const int64_t length = 1000;
    std::unique_ptr<std::vector<int32_t>> data =
        std::make_unique<std::vector<int32_t>>(generate_data(length));

    *array = ArrowArray{
        length,
        0,                                                                   // null_count
        0,                                                                   // offset
        2,                                                                   // n_buffers
        0,                                                                   // n_children
        new const void*[2]{nullptr, reinterpret_cast<void*>(data->data())},  // buffers
        nullptr,                                                             // children
        nullptr,                                                             // dictionary
        [](struct ArrowArray* arr) {  // release callback
            delete[] arr->buffers;
            delete reinterpret_cast<std::vector<int32_t>*>(arr->private_data);
            arr->release = nullptr;
        },
        reinterpret_cast<void*>(data.release()),  // private_data
    };
}


