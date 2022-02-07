#include <stdio.h>
#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/c/common.h"

void PrintTfLiteModelSummary(TfLiteInterpreter *interpreter);
void PrintTfLiteTensorSummary(const TfLiteTensor *tensor);

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

    // print the info before invoking
    PrintTfLiteModelSummary(interpreter);

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

    printf("Successfully invoked model\n");

    int a = 10;
    int *c = &a;
    c[1] = 2;
    printf("Value of c=%d\n", a);

    return 0;
}


void PrintTfLiteModelSummary(TfLiteInterpreter *interpreter) {
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

void PrintTfLiteTensorSummary(const TfLiteTensor *tensor) {
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