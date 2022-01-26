# Introduction
Builds were provided by: https://github.com/ValYouW/tflite-dist

This is a config.cmake for the lightweight tensorflowlite_c api. 

# Example code
This sample program assumes the package has been cloned into the root of the working directory as tflite_c/
## C program
```c
#include <stdio.h>
#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/c/common.h"

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Insert directory of tflite model\n");
        return 1;
    }

    // Create the model and interpreter options.
    const char *model_path = argv[1];
    TfLiteModel* model = TfLiteModelCreateFromFile(model_path);
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreterOptionsSetNumThreads(options, 2);

    // Create the interpreter.
    TfLiteInterpreter* interpreter = TfLiteInterpreterCreate(model, options);

    // Allocate tensors and populate the input tensor data.
    TfLiteInterpreterAllocateTensors(interpreter);
    TfLiteTensor* input_tensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    size_t input_size = 1; 
    {
        for (int i = 0; i < input_tensor->dims->size; i++) {
            input_size *= input_tensor->dims->data[i];
        }
    }
    float *input = (float *)malloc(sizeof(float) * input_size);
    TfLiteTensorCopyFromBuffer(input_tensor, input, input_size * sizeof(float));

    // Execute inference.
    TfLiteInterpreterInvoke(interpreter);

    // Extract the output tensor data.
    const TfLiteTensor* output_tensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
    size_t output_size = 1; 
    {
        for (int i = 0; i < output_tensor->dims->size; i++) {
            output_size *= output_tensor->dims->data[i];
        }
    }
    float *output = (float *)malloc(sizeof(float) * output_size);

    TfLiteTensorCopyToBuffer(output_tensor, output, output_size * sizeof(float));

    // Dispose of the model and interpreter objects.
    TfLiteInterpreterDelete(interpreter);
    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);
    // free input/output buffers
    free(input);
    free(output);

    return 0;
}
```

## CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.0.0)
project(main)

set(tflitec_DIR ${CMAKE_SOURCE_DIR}/tflite_c)
find_package(tflitec CONFIG REQUIRED)

set(SRC_FILES main.c)
add_executable(main main.c)
target_link_libraries(main PRIVATE tflitec) 

# vcpkg.cmake has internal stuff that autogenerates this
# we have to do this manually
add_custom_command(TARGET main 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
    ${tflitec_DIR}/bin/tensorflowlite_c.dll
    $<TARGET_FILE_DIR:main>)
```