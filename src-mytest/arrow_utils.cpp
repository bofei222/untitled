#include <iostream>
#include <arrow/api.h>
#include <arrow/array.h>

#include <arrow/c/abi.h>

void PrintArrowFieldAndArray(ArrowSchema* arrowField, ArrowArray* arrowArray) {
    if (!arrowField || !arrowArray) {
        std::cerr << "Invalid schema or array pointer!" << std::endl;
        return;
    }

    // 1. Print ArrowSchema field details
    std::cout << "ArrowSchema fields:" << std::endl;
    for (int i = 0; i < arrowField->n_children; ++i) {
        ArrowSchema* childSchema = arrowField->children[i];
        std::cout << "Field " << i << " name: " << childSchema->name << std::endl;
        std::cout << "Field " << i << " type: " << childSchema->format << std::endl;
    }

    // 2. Print ArrowArray data
    std::cout << "\nArrowArray values:" << std::endl;
    for (int64_t i = 0; i < arrowArray->length; ++i) {
        for (int j = 0; j < arrowField->n_children; ++j) {
            ArrowArray* childArray = arrowArray->children[j];

            // Assuming the childArray contains a specific type, for example, integers
            if (childArray->format[0] == 'i') {  // Integer type
                int* data = reinterpret_cast<int*>(childArray->buffers[1]);
                std::cout << "Value at index " << i << " in field " << j << ": " << data[i] << std::endl;
            }
            // You can add more type checks for other Arrow types like float, string, etc.
            else {
                std::cerr << "Unsupported data type: " << childArray->format << std::endl;
            }
        }
    }
}


int main() {
    // Sample ArrowSchema and ArrowArray creation for testing (normally you'd load these from a file or other sources)
    std::shared_ptr<arrow::Field> field = arrow::field("sampleField", arrow::int32());
    std::shared_ptr<arrow::Schema> schema = arrow::schema({field});

    // Create a sample array with integers
    arrow::Int32Builder builder;
    builder.Append(1);
    builder.Append(2);
    builder.Append(3);
    std::shared_ptr<arrow::Array> array;
    builder.Finish(&array);

    // Convert to Arrow C structs
    ArrowSchema arrowField;
    arrowField.name = const_cast<char*>(field->name().c_str());
    arrowField.format = const_cast<char*>(field->type()->ToString().c_str());
    arrowField.n_children = 0;  // No children for simple field in this example

    ArrowArray arrowArray;
    arrowArray.length = array->length();
    arrowArray.buffers = new uint8_t*[2];  // For simplicity, assume we have a buffer for data
    arrowArray.buffers[1] = reinterpret_cast<uint8_t*>(array->data()->buffers()[1]->data());
    arrowArray.n_children = 0;

    // Print the field and array details
    PrintArrowFieldAndArray(&arrowField, &arrowArray);

    return 0;
}
