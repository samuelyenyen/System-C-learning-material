#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cfloat> // 对于 FLT_MAX
#include <algorithm> // For std::max_elemen
#include <systemc> // include the systemC header file
using namespace sc_core; // use namespace
using namespace std;
using tensor_1D = std::vector<float>;
using tensor_2D = std::vector<std::vector<float>>;
using tensor_3D = std::vector<std::vector<std::vector<float>>>;
using tensor_4D = std::vector<std::vector<std::vector<std::vector<float>>>>;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SC_MODULE(Process1) {
    sc_out<float> Process1_out[9216];
    // sc_port<sc_fifo_out_if<float>> out_fifo;

    void run() {
        tensor_3D image = readImageFromFile("cat.txt", 224, 224, 3);
        tensor_4D filter1 = readKernelFromFile("conv1_weight.txt", 64, 3, 11, 11);
        std::vector<float> bias1 = readBiases("conv1_bias.txt");

        vector<vector<vector<float>>> resizedImage = resizeImage(image, 227, 227);
        vector<vector<vector<float>>> transposedVector = transpose3DVector(resizedImage);
        
        auto output = conv(transposedVector, filter1, bias1, 4);
        applyReLU(output);

        auto pooledOutput = applyMaxPooling(output, 2, 2);

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        ////Conv2
        auto paddedOutput = applyZeroPadding(pooledOutput, 2);
        vector<vector<vector<vector<float>>>> filter2 = readKernelFromFile("conv2_weight.txt", 192, 64, 5, 5);
        std::vector<float> bias2 = readBiases("conv2_bias.txt");
        auto output2 = conv(paddedOutput, filter2, bias2, 1);
        applyReLU(output2);
        auto pooledOutput2 = applyMaxPooling(output2, 3, 2);
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        ////Conv3
        vector<vector<vector<vector<float>>>> filter3 = readKernelFromFile("conv3_weight.txt", 384, 192, 3, 3);
        std::vector<float> bias3 = readBiases("conv3_bias.txt");
        auto paddedOutput3 = applyZeroPadding(pooledOutput2, 1);
        auto output3 = conv(paddedOutput3, filter3, bias3, 1);
        applyReLU(output3);
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        ////Conv4
        vector<vector<vector<vector<float>>>> filter4 = readKernelFromFile("conv4_weight.txt", 256, 384, 3, 3);
        std::vector<float> bias4 = readBiases("conv4_bias.txt");
        auto paddedOutput4 = applyZeroPadding(output3, 1);
        auto output4 = conv(paddedOutput4, filter4, bias4, 1);
        applyReLU(output4);
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        ////Conv5
        vector<vector<vector<vector<float>>>> filter5 = readKernelFromFile("conv5_weight.txt", 256, 256, 3, 3);
        std::vector<float> bias5 = readBiases("conv5_bias.txt");
        auto paddedOutput5 = applyZeroPadding(output4, 1);
        auto output5 = conv(paddedOutput5, filter5, bias5, 1);
        applyReLU(output5);
        auto pooledOutput5 = applyMaxPooling(output5, 3, 2);
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        ////adaptiveAvgPool
        auto finalOutput = adaptiveAvgPool(pooledOutput5, 6, 6);
        vector<float> outputVector = flattenVector(finalOutput);
        // cout << "Flattened vector size: " << outputVector.size() << endl; // 应该输出 9216

        // cout << "27th value of vector " << outputVector[27] << endl;
        for (int i = 0; i < 9216; i++) {
            Process1_out[i].write(outputVector[i]);  
            // out_fifo->write(outputVector[i]);
        }
    }

    tensor_3D readImageFromFile(const string& filename, int width, int height, int channels) {
        ifstream file(filename);
        tensor_3D image(height, vector<vector<float>>(width, vector<float>(channels)));
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return image;
        }

        

        for (int k = 0; k < channels; ++k) {
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    if (file.eof()) {
                        std::cerr << "Reached end of file prematurely." << std::endl;
                        return image;
                    }
                    file >> image[i][j][k];
                }
            }
        }

        return image;
    }
        // Assuming the image data is stored as integers in the .txt file
    tensor_4D readKernelFromFile(const string& filename, int channel_out, int channel_in, int height, int width) {
        ifstream file(filename);

        tensor_4D filter(
            channel_out, vector<vector<vector<float>>>(
                channel_in, vector<vector<float>>(
                    height, vector<float>(
                        width, 0))));

        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << std::endl;
            return filter;
        }
        
        

        for (int k = 0; k < channel_out; ++k) {
            for (int i = 0; i < channel_in; ++i) {
                for (int j = 0; j < height; ++j) {
                    for (int l = 0; l < width; ++l) {
                        if (file.eof()) {
                            std::cerr << "Reached end of file prematurely." << std::endl;
                            return filter;
                        }
                        file >> filter[k][i][j][l];
                    }
                }
            }
        }

        return filter;
    } 
    tensor_3D resizeImage(const vector<vector<vector<float>>>& image, int newWidth, int newHeight) {
        int oldHeight = image.size();
        int oldWidth = image[0].size();
        int channels = image[0][0].size();
        
        tensor_3D resizedImage(newHeight, vector<vector<float>>(newWidth, vector<float>(channels)));

        float xRatio = static_cast<float>(oldWidth) / newWidth;
        float yRatio = static_cast<float>(oldHeight) / newHeight;

        for (int i = 0; i < newHeight; ++i) {
            for (int j = 0; j < newWidth; ++j) {
                int nearestX = round(j * xRatio);
                int nearestY = round(i * yRatio);
                // Clamping to ensure indices are within bounds
                nearestX = min(nearestX, oldWidth - 1);
                nearestY = min(nearestY, oldHeight - 1);
                for (int k = 0; k < channels; ++k) {
                    resizedImage[i][j][k] = image[nearestY][nearestX][k];
                }
            }
        }

        return resizedImage;
    }

    
    tensor_3D transpose3DVector(const vector<vector<vector<float>>>& input) {
        const int H = input.size();       
        const int W = input[0].size();    
        const int C = input[0][0].size(); 

        tensor_3D output(C, vector<vector<float>>(H, vector<float>(W)));

        for (int c = 0; c < C; ++c) {
            for (int h = 0; h < H; ++h) {
                for (int w = 0; w < W; ++w) {
                    // 在这个例子中，我们假设输入的维度是[H, W, C]，我们要转置为[C, H, W]
                    output[c][h][w] = input[h][w][c];
                }
            }
        }

        return output;
    }

    float conv2D(const vector<vector<float>>& input, const vector<vector<float>>& kernel, int startRow, int startCol) {
        float sum = 0.0;
        for(int i = 0; i < kernel.size(); ++i) {
            for(int j = 0; j < kernel[0].size(); ++j) {
                sum += input[startRow + i][startCol + j] * kernel[i][j];
            }
        }
        return sum;
    }

    tensor_3D conv(const vector<vector<vector<float>>>& input,
                                        const vector<vector<vector<vector<float>>>>& kernels,
                                        const vector<float>& biases, // Add biases as a parameter
                                        int stride) {
        const int outputDepth = kernels.size();
        const int outputHeight = (input[0].size() - kernels[0][0].size()) / stride + 1;
        const int outputWidth = (input[0][0].size() - kernels[0][0][0].size()) / stride + 1;
        
        tensor_3D output(outputDepth, vector<vector<float>>(outputHeight, vector<float>(outputWidth, 0.0)));

        for(int d = 0; d < outputDepth; ++d) { // For each output depth
            for(int h = 0; h < outputHeight; ++h) { // For each output row
                for(int w = 0; w < outputWidth; ++w) { // For each output column
                    float sum = 0.0;
                    for(int c = 0; c < input.size(); ++c) { // Sum over all input channels
                        // Apply 2D convolution
                        sum += conv2D(input[c], kernels[d][c], h*stride, w*stride);
                    }
                    output[d][h][w] = sum + biases[d];;
                }
            }
        }
        
        return output;
    }

    tensor_1D readBiases(const std::string& filename) {
        tensor_1D biases;
        ifstream file(filename);
        float bias;

        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return biases; // 返回一个空向量
        }

        while (file >> bias) {
            biases.push_back(bias);
        }

        file.close();
        return biases;
    }

    void applyReLU(vector<vector<vector<float>>>& data) {
        for (auto& depth : data) {
            for (auto& row : depth) {
                for (float& value : row) {
                    value = std::max(0.0f, value); // ReLU操作
                }
            }
        }
    }

    tensor_3D applyMaxPooling(const vector<vector<vector<float>>>& input, int poolSize, int stride) {
        const int depth = input.size();
        const int height = input[0].size();
        const int width = input[0][0].size();

        const int pooledHeight = (height - poolSize) / stride + 1;
        const int pooledWidth = (width - poolSize) / stride + 1;

        tensor_3D output(depth, vector<vector<float>>(pooledHeight, vector<float>(pooledWidth, 0)));

        for (int d = 0; d < depth; ++d) {
            for (int h = 0; h < pooledHeight; ++h) {
                for (int w = 0; w < pooledWidth; ++w) {
                    float maxVal = -FLT_MAX; // 初始化为最小浮点数
                    for (int ph = 0; ph < poolSize; ++ph) {
                        for (int pw = 0; pw < poolSize; ++pw) {
                            int hIndex = h * stride + ph;
                            int wIndex = w * stride + pw;
                            if (input[d][hIndex][wIndex] > maxVal) {
                                maxVal = input[d][hIndex][wIndex];
                            }
                        }
                    }
                    output[d][h][w] = maxVal;
                }
            }
        }

        return output;
    }

    tensor_3D applyZeroPadding(const vector<vector<vector<float>>>& input, int paddingSize) {
        const int depth = input.size();
        const int height = input[0].size();
        const int width = input[0][0].size();

        const int paddedHeight = height + 2 * paddingSize;
        const int paddedWidth = width + 2 * paddingSize;

        tensor_3D output(depth, vector<vector<float>>(paddedHeight, vector<float>(paddedWidth, 0.0f)));

        for (int d = 0; d < depth; ++d) {
            for (int h = 0; h < height; ++h) {
                for (int w = 0; w < width; ++w) {
                    output[d][h + paddingSize][w + paddingSize] = input[d][h][w];
                }
            }
        }

        return output;
    }
    tensor_3D adaptiveAvgPool(const vector<vector<vector<float>>>& input, int targetH, int targetW) {
        const int depth = input.size();
        const int inputH = input[0].size();
        const int inputW = input[0][0].size();

        tensor_3D output(depth, vector<vector<float>>(targetH, vector<float>(targetW, 0)));

        // Calculate the size of the pooling regions
        int windowH = inputH / targetH;
        int windowW = inputW / targetW;

        for (int d = 0; d < depth; ++d) {
            for (int h = 0; h < targetH; ++h) {
                for (int w = 0; w < targetW; ++w) {
                    float sum = 0;
                    for (int y = h * windowH; y < (h + 1) * windowH; ++y) {
                        for (int x = w * windowW; x < (w + 1) * windowW; ++x) {
                            sum += input[d][y][x];
                        }
                    }
                    output[d][h][w] = sum / (windowH * windowW);
                }
            }
        }

        return output;
    }

    tensor_1D flattenVector(const vector<vector<vector<float>>>& input) {
        const int depth = input.size();
        const int height = input[0].size();
        const int width = input[0][0].size();

        vector<float> flatVector(depth * height * width);
        
        int index = 0;
        for (int d = 0; d < depth; ++d) {
            for (int h = 0; h < height; ++h) {
                for (int w = 0; w < width; ++w) {
                    // 将三维向量的元素按顺序复制到一维向量中
                    flatVector[index++] = input[d][h][w];
                }
            }
        }
        
        return flatVector;
    }

    SC_CTOR(Process1) {
        SC_METHOD(run);
        // dont_initialize();
    }
};

SC_MODULE(Process2) {
    sc_in<float> Process2_in[9216];
    // sc_port<sc_fifo_in_if<float>> in_fifo;

    void run() {
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //Dropout
        // while(true){
            std::vector<float> outputVector; 
            outputVector.resize(9216);
            for (int i = 0; i < 9216; i++) {
                outputVector[i] = Process2_in[i].read();
                // cout<<outputVector[i]<<endl;
                // in_fifo->read(outputVector[i]); 
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //linear1
            vector<vector<float>> weight1 = readMatrixFromFile("fc6_weight.txt", 4096, 9216);
            std::vector<float> bias6 = readBiases("fc6_bias.txt");
            std::vector<float> fcOutput1 = linear(outputVector, weight1, bias6);
            applyReLU1D(fcOutput1);

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            ////Dropout

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            ////linear2
            vector<vector<float>> weight2 = readMatrixFromFile("fc7_weight.txt", 4096, 4096);
            std::vector<float> bias7 = readBiases("fc7_bias.txt");
            std::vector<float> fcOutput2 = linear(fcOutput1, weight2, bias7);
            applyReLU1D(fcOutput2);

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            ////linear3
            vector<vector<float>> weight3 = readMatrixFromFile("fc8_weight.txt", 1000, 4096);
            std::vector<float> bias8 = readBiases("fc8_bias.txt");
            std::vector<float> fcOutput3 = linear(fcOutput2, weight3, bias8);

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            std::vector<std::string> classes = readImageNetClasses("imagenet_classes.txt");

            std::vector<std::pair<float, int>> top5;
            for (int i = 0; i < fcOutput3.size(); ++i) {
                top5.push_back(std::make_pair(fcOutput3[i], i));
            }

            std::partial_sort(top5.begin(), top5.begin() + 5, top5.end(), std::greater<>());

            for (int i = 0; i < 5; ++i) {
                int index = top5[i].second;
                float value = top5[i].first;
                if (index < classes.size()) {
                    std::cout << "Top " << i+1 << " class: " << classes[index] << " - Value: " << value << std::endl;
                } else {
                    std::cout << "Invalid index: " << index << std::endl;
                }
            }
            // wait();
        // }
    } 

    tensor_1D linear(const vector<float>& input, const vector<vector<float>>& weights, const vector<float>& biases) {
        size_t outputSize = biases.size();
        vector<float> output(outputSize, 0.0);

        for (size_t i = 0; i < outputSize; ++i) {
            for (size_t j = 0; j < input.size(); ++j) {
                output[i] += input[j] * weights[i][j];
            }
            output[i] += biases[i];
        }

        return output;
    }

    tensor_2D readMatrixFromFile(const string& filename, int rows, int cols) {
        vector<vector<float>> matrix(rows, vector<float>(cols));
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << std::endl;
            return matrix; 
        }

        for (int i = 0; i < rows && !file.eof(); ++i) {
            for (int j = 0; j < cols && !file.eof(); ++j) {
                file >> matrix[i][j];
            }
        }

        return matrix;
    }

    tensor_1D readBiases(const std::string& filename) {
        tensor_1D biases;
        ifstream file(filename);
        float bias;

        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return biases; // 返回一个空向量
        }

        while (file >> bias) {
            biases.push_back(bias);
        }

        file.close();
        return biases;
    }
    void applyReLU1D(std::vector<float>& data) {
        for (float& value : data) {
            value = std::max(0.0f, value);
        }
    }

    std::vector<std::string> readImageNetClasses(const std::string& filename) {
        vector<string> classes;
        ifstream file(filename);
        string line;

        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << endl;
            return classes;
        }

        while (getline(file, line)) {
            classes.push_back(line);
        }

        return classes;
    }

    SC_CTOR(Process2) {
        SC_METHOD(run);
        for (int i = 0; i < 9216; i++) {
            sensitive << Process2_in[i];
        }
        dont_initialize();
    }
};


int sc_main(int argc, char* argv[]) {
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Software preprocess
    // sc_clock clk("clk", 10, SC_NS);
    sc_signal<float> signals[9216];
    // sc_buffer<float> buffers[9216];
    // sc_fifo<float> fifo(9216);

    Process1 Process1("Process1");
    Process2 Process2("Process2");
    for (int i = 0; i < 9216; i++) {
        Process1.Process1_out[i](signals[i]);
        Process2.Process2_in[i](signals[i]);
        // Process1.Process1_out[i](buffers[i]);
        // Process2.Process2_in[i](buffers[i]);
    }
    // Process1.out_fifo(fifo);
    // Process2.in_fifo(fifo);

    // Process1.run();  // Manually call produce
    // Process2.run();  // Manually call produce
    sc_start(1, SC_NS);  // Advance time to allow signals to propagate


    return 0;
}