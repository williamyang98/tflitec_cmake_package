#include <stdio.h>
#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/c/common.h"

void PrintTfLiteModelSummary(TfLiteInterpreter* interpreter);
void PrintTfLiteTensorSummary(const TfLiteTensor* tensor);

// Refer to tensorflow/lite/c_api_types.h for each TfLiteType
const size_t TfLiteTypeSizeTable[17] = {
    0, // kTfLiteNoType = 0,
    4, // kTfLiteFloat32 = 1,
    4, // kTfLiteInt32 = 2,
    1, // kTfLiteUInt8 = 3,
    8, // kTfLiteInt64 = 4,
    1, // kTfLiteString = 5,            TODO: What is the format of variable length byte string? 
    1, // kTfLiteBool = 6,
    2, // kTfLiteInt16 = 7,
    8, // kTfLiteComplex64 = 8,
    1, // kTfLiteInt8 = 9,
    2, // kTfLiteFloat16 = 10,
    8, // kTfLiteFloat64 = 11,
    16,// kTfLiteComplex128 = 12,
    8, // kTfLiteUInt64 = 13,
    1, // kTfLiteResource = 14,         TODO: What is the format of this? 
    1, // kTfLiteVariant = 15,          TODO: What is this type at runtime?
    4, // kTfLiteUInt32 = 16,
};

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("Usage: main [model_filepath]\n");
        return 1;
    }

    // Create the model and interpreter options.
    const char* model_path = argv[1];
    const int total_threads = 1;
    TfLiteModel* model = TfLiteModelCreateFromFile(model_path);
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreterOptionsSetNumThreads(options, total_threads);

    // Create the interpreter.
    TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model, options);
    TfLiteInterpreterAllocateTensors(interpreter);

    // Show summary of model
    PrintTfLiteModelSummary(interpreter);

    // Allocate and copy input data
    const int input_tensor_count = TfLiteInterpreterGetInputTensorCount(interpreter);
    for (int i = 0; i < input_tensor_count; i++) {
        TfLiteTensor* tensor = TfLiteInterpreterGetInputTensor(interpreter, i);
        size_t total_elements = 1;
        for (int j = 0; j < tensor->dims->size; j++) {
            total_elements *= tensor->dims->data[j];
        }
        const TfLiteType tensor_type = TfLiteTensorType(tensor);
        const size_t type_size = TfLiteTypeSizeTable[tensor_type];
        const size_t total_bytes = total_elements*type_size;
        uint8_t* buf = (uint8_t*)malloc(total_bytes);
        TfLiteTensorCopyFromBuffer(tensor, (void*)buf, total_bytes);
        free(buf);
    }
    printf("Copied data into input tensors\n");

    // Forward pass
    TfLiteInterpreterInvoke(interpreter);
    printf("Successfully invoked interpreter for forward pass\n");

    // Allocate and copy output data
    const int output_tensor_count = TfLiteInterpreterGetOutputTensorCount(interpreter);
    for (int i = 0; i < output_tensor_count; i++) {
        const TfLiteTensor* tensor = TfLiteInterpreterGetOutputTensor(interpreter, i);
        size_t total_elements = 1;
        for (int j = 0; j < tensor->dims->size; j++) {
            total_elements *= tensor->dims->data[j];
        }
        const TfLiteType tensor_type = TfLiteTensorType(tensor);
        const size_t type_size = TfLiteTypeSizeTable[tensor_type];
        const size_t total_bytes = total_elements*type_size;
        uint8_t* buf = (uint8_t*)malloc(total_bytes);
        TfLiteTensorCopyToBuffer(tensor, buf, total_bytes);
        free(buf);
    }
    printf("Copied data from output tensors\n");

    // Dispose of the model and interpreter objects.
    TfLiteInterpreterDelete(interpreter);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);
    printf("Freed model data\n");

    return 0;
}


void PrintTfLiteModelSummary(TfLiteInterpreter* interpreter) {
    int input_tensor_count = TfLiteInterpreterGetInputTensorCount(interpreter);
    int output_tensor_count = TfLiteInterpreterGetOutputTensorCount(interpreter);
    printf("inputs=%d, output=%d\n", input_tensor_count, output_tensor_count);

    for (int i = 0; i < input_tensor_count; i++) {
        const TfLiteTensor* input_tensor = TfLiteInterpreterGetInputTensor(interpreter, i);
        printf("inp_tensor[%d]: ", i);
        PrintTfLiteTensorSummary(input_tensor);
    }

    for (int i = 0; i < output_tensor_count; i++) {
        const TfLiteTensor* output_tensor = TfLiteInterpreterGetOutputTensor(interpreter, i);
        printf("out_tensor[%d]: ", i);
        PrintTfLiteTensorSummary(output_tensor);
    }
}

void PrintTfLiteTensorSummary(const TfLiteTensor* tensor) {
    printf("%s (", tensor->name);
    // size of tensor
    for (int j = 0; j < tensor->dims->size; j++) {
        int dim = tensor->dims->data[j];
        if (j != tensor->dims->size-1) {
            printf("%d,", dim);
        } else {
            printf("%d) ", dim);
        }
    }
    // type of tensor
    TfLiteType t = TfLiteTensorType(tensor);
    printf("%s ", TfLiteTypeGetName(t));
    // quantisation?
    if (t == kTfLiteUInt8) {
        TfLiteQuantizationParams qparams = TfLiteTensorQuantizationParams(tensor);
        printf("[scale=%.2f, zero_point=%d]\n", qparams.scale, qparams.zero_point);
    } else {
        printf("\n");
    }
}
