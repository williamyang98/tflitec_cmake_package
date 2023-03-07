## Instructions
1. Configure cmake

<code>cmake . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release</code>

2. Build program 

<code>ninja -C build</code>

3. Run program

<code>build/main.exe models/cnn_227_160_quantized.tflite</code>
