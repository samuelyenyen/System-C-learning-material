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

// Assuming the image data is stored as integers in the .txt file
vector<vector<vector<float>>> readImageFromFile(const std::string& filename, int width, int height, int channels) {
    std::ifstream file(filename);
    std::vector<std::vector<std::vector<float>>> image(height, std::vector<std::vector<float>>(width, std::vector<float>(channels)));
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

    // Here you can process the image as needed
    // For demonstration, just printing the value of the first pixel
    // std::cout << "Last pixel (R, G, B): (" << image[223][223][0] << ", " << image[223][223][1] << ", " << image[223][223][2] << ")" << std::endl;
    return image;
}

// Assuming the image data is stored as integers in the .txt file
vector<vector<vector<vector<float>>>> readKernelFromFile(const std::string& filename, int channel_out, int channel_in, int height, int width) {
    std::ifstream file(filename);

    // 創建4維vector，並用0初始化所有元素
    std::vector<std::vector<std::vector<std::vector<float>>>> filter(
        channel_out, std::vector<std::vector<std::vector<float>>>(
            channel_in, std::vector<std::vector<float>>(
                height, std::vector<float>(
                    width, 0))));

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
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

    // Here you can process the image as needed
    // For demonstration, just printing the value of the first pixel
    // std::cout << "Last pixel (R, G, B): (" << image[223][223][0] << ", " << image[223][223][1] << ", " << image[223][223][2] << ")" << std::endl;
    return filter;
}

// Function to resize the image using nearest neighbor interpolation
vector<vector<vector<float>>> resizeImage(const vector<vector<vector<float>>>& image, int newWidth, int newHeight) {
    int oldHeight = image.size();
    int oldWidth = image[0].size();
    int channels = image[0][0].size();
    
    vector<vector<vector<float>>> resizedImage(newHeight, vector<vector<float>>(newWidth, vector<float>(channels)));

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

void saveImageToFile(const vector<vector<vector<float>>>& image, const std::string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file for writing: " << filename << endl;
        return;
    }

    int height = image.size();
    int width = image[0].size();
    int channels = image[0][0].size();

    // Set the precision to 16 digits after the decimal point
    file << fixed << setprecision(19);

    for (int k = 0; k < channels; ++k) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                file << image[i][j][k];
                file << " "; // Separate channels with a space
            }
            file << "\n"; // Start a new line for each pixel
        }
    }

    file.close();
    cout << "Image saved to: " << filename << endl;
}

vector<vector<vector<float>>> transpose3DVector(const vector<vector<vector<float>>>& input) {
    const int H = input.size();       
    const int W = input[0].size();    
    const int C = input[0][0].size(); 

    // 创建一个新的3D vector来存储转置后的数据
    vector<vector<vector<float>>> output(C, vector<vector<float>>(H, vector<float>(W)));

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

// Function to perform a single 2D convolution operation
float conv2D(const vector<vector<float>>& input, const vector<vector<float>>& kernel, int startRow, int startCol) {
    float sum = 0.0;
    for(int i = 0; i < kernel.size(); ++i) {
        for(int j = 0; j < kernel[0].size(); ++j) {
            sum += input[startRow + i][startCol + j] * kernel[i][j];
        }
    }
    return sum;
}

// Main convolution function
vector<vector<vector<float>>> conv(const vector<vector<vector<float>>>& input,
                                    const vector<vector<vector<vector<float>>>>& kernels,
                                    const vector<float>& biases, // Add biases as a parameter
                                    int stride) {
    const int outputDepth = kernels.size();
    const int outputHeight = (input[0].size() - kernels[0][0].size()) / stride + 1;
    const int outputWidth = (input[0][0].size() - kernels[0][0][0].size()) / stride + 1;
    
    vector<vector<vector<float>>> output(outputDepth, vector<vector<float>>(outputHeight, vector<float>(outputWidth, 0.0)));
    
    // cout << input.size()<<endl;
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

std::vector<float> readBiases(const std::string& filename) {
    std::vector<float> biases;
    std::ifstream file(filename);
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

void applyReLU1D(std::vector<float>& data) {
    for (float& value : data) {
        value = std::max(0.0f, value);
    }
}

vector<vector<vector<float>>> applyMaxPooling(const vector<vector<vector<float>>>& input, int poolSize, int stride) {
    const int depth = input.size();
    const int height = input[0].size();
    const int width = input[0][0].size();

    const int pooledHeight = (height - poolSize) / stride + 1;
    const int pooledWidth = (width - poolSize) / stride + 1;

    vector<vector<vector<float>>> output(depth, vector<vector<float>>(pooledHeight, vector<float>(pooledWidth, 0)));

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

vector<vector<vector<float>>> applyZeroPadding(const vector<vector<vector<float>>>& input, int paddingSize) {
    const int depth = input.size();
    const int height = input[0].size();
    const int width = input[0][0].size();

    // 计算填充后的高度和宽度
    const int paddedHeight = height + 2 * paddingSize;
    const int paddedWidth = width + 2 * paddingSize;

    // 创建填充后的输出向量，初始化为0
    vector<vector<vector<float>>> output(depth, vector<vector<float>>(paddedHeight, vector<float>(paddedWidth, 0.0f)));

    // 填充输入数据到输出向量的中心位置
    for (int d = 0; d < depth; ++d) {
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                output[d][h + paddingSize][w + paddingSize] = input[d][h][w];
            }
        }
    }

    return output;
}

vector<vector<vector<float>>> adaptiveAvgPool(const vector<vector<vector<float>>>& input, int targetH, int targetW) {
    const int depth = input.size();
    const int inputH = input[0].size();
    const int inputW = input[0][0].size();

    vector<vector<vector<float>>> output(depth, vector<vector<float>>(targetH, vector<float>(targetW, 0)));

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

vector<float> flattenVector(const vector<vector<vector<float>>>& input) {
    // 确保输入向量的形状是 [256, 6, 6]
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

// 实现一个简单的线性层
std::vector<float> linear(const std::vector<float>& input, const std::vector<std::vector<float>>& weights, const std::vector<float>& biases) {
    size_t outputSize = biases.size();
    std::vector<float> output(outputSize, 0.0);

    for (size_t i = 0; i < outputSize; ++i) {
        for (size_t j = 0; j < input.size(); ++j) {
            output[i] += input[j] * weights[i][j];
        }
        output[i] += biases[i];
    }

    return output;
}

std::vector<std::vector<float>> readMatrixFromFile(const std::string& filename, int rows, int cols) {
    std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return matrix; // 返回一个空的矩阵
    }

    for (int i = 0; i < rows && !file.eof(); ++i) {
        for (int j = 0; j < cols && !file.eof(); ++j) {
            file >> matrix[i][j];
        }
    }

    return matrix;
}

std::vector<std::string> readImageNetClasses(const std::string& filename) {
    std::vector<std::string> classes;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return classes;
    }

    while (std::getline(file, line)) {
        classes.push_back(line);
    }

    return classes;
}

int sc_main(int, char*[]) {
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Conv1
    vector<vector<vector<float>>> image = readImageFromFile("cat.txt", 224, 224, 3);
    vector<vector<vector<vector<float>>>> filter1 = readKernelFromFile("conv1_weight.txt", 64, 3, 11, 11);
    std::vector<float> bias1 = readBiases("conv1_bias.txt");

    vector<vector<vector<float>>> resizedImage = resizeImage(image, 227, 227);
    vector<vector<vector<float>>> transposedVector = transpose3DVector(resizedImage);
    
    // cout << transposedVector.size()<<endl;
    // cout << transposedVector[0].size()<<endl;
    // cout << transposedVector[0][0].size()<<endl;
    // Save the resized image to a file
    //saveImageToFile(resizedImage, "resized_cat.txt");
    //std::cout << "Last pixel (R, G, B): (" << image[0][1][0] << ", " << image[0][0][0] << ", " << image[1][0][1] << ")" << std::endl;
    // std::cout << "first filter1: (" << filter1[0][0][0][0] << ", " << filter1[0][1][0][0] << ", " << filter1[0][2][0][0] << ")" << std::endl;    
    
    auto output = conv(transposedVector, filter1, bias1, 4);
    // cout << "Sample value before ReLU: " << output[0][0][0] << endl;
    // 应用ReLU激活函数
    applyReLU(output);

    // cout << "Output size: " << output.size() << "x" << output[0].size() << "x" << output[0][0].size() << endl;
    // cout << "Sample value after ReLU: " << output[0][0][0] << endl;
    // cout <<"Original  : = " << resizedImage[226][0][0] << endl;
    // cout <<"Original  : = " << resizedImage[226][0][1] << endl;
    // cout <<"Original  : = " << resizedImage[226][0][2] << endl;
    // cout <<"Transposed: = " << transposedVector[0][226][0] << endl;
    // cout <<"Transposed: = " << transposedVector[1][226][0] << endl;
    // cout <<"Transposed: = " << transposedVector[2][226][0] << endl;
    
    // 应用MaxPooling
    auto pooledOutput = applyMaxPooling(output, 2, 2);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Conv2
    auto paddedOutput = applyZeroPadding(pooledOutput, 2);
    // cout << "Output size after Padding: " << paddedOutput.size() << "x" << paddedOutput[0].size() << "x" << paddedOutput[0][0].size() << endl;
    
    vector<vector<vector<vector<float>>>> filter2 = readKernelFromFile("conv2_weight.txt", 192, 64, 5, 5);
    std::vector<float> bias2 = readBiases("conv2_bias.txt");
    auto output2 = conv(paddedOutput, filter2, bias2, 1);
    applyReLU(output2);
    auto pooledOutput2 = applyMaxPooling(output2, 3, 2);
    // cout << "Output size after Pooling: " << pooledOutput2.size() << "x" << pooledOutput2[0].size() << "x" << pooledOutput2[0][0].size() << endl;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Conv3
    vector<vector<vector<vector<float>>>> filter3 = readKernelFromFile("conv3_weight.txt", 384, 192, 3, 3);
    std::vector<float> bias3 = readBiases("conv3_bias.txt");
    auto paddedOutput3 = applyZeroPadding(pooledOutput2, 1);
    auto output3 = conv(paddedOutput3, filter3, bias3, 1);
    applyReLU(output3);
    // cout << "Output size after conv: " << output3.size() << "x" << output3[0].size() << "x" << output3[0][0].size() << endl;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Conv4
    vector<vector<vector<vector<float>>>> filter4 = readKernelFromFile("conv4_weight.txt", 256, 384, 3, 3);
    std::vector<float> bias4 = readBiases("conv4_bias.txt");
    auto paddedOutput4 = applyZeroPadding(output3, 1);
    auto output4 = conv(paddedOutput4, filter4, bias4, 1);
    applyReLU(output4);
    // cout << "Output size after conv: " << output4.size() << "x" << output4[0].size() << "x" << output4[0][0].size() << endl;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Conv5
    vector<vector<vector<vector<float>>>> filter5 = readKernelFromFile("conv5_weight.txt", 256, 256, 3, 3);
    std::vector<float> bias5 = readBiases("conv5_bias.txt");
    auto paddedOutput5 = applyZeroPadding(output4, 1);
    auto output5 = conv(paddedOutput5, filter5, bias5, 1);
    applyReLU(output5);
    auto pooledOutput5 = applyMaxPooling(output5, 3, 2);
    cout << "Output size after applyReLU: " << output5.size() << "x" << output5[0].size() << "x" << output5[0][0].size() << endl;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////adaptiveAvgPool
    auto finalOutput = adaptiveAvgPool(pooledOutput5, 6, 6);
    // cout << "Output size after AdaptiveAvgPool: " << finalOutput.size() << "x" << finalOutput[0].size() << "x" << finalOutput[0][0].size() << endl;   

    vector<float> outputVector = flattenVector(finalOutput);
    // cout << "Flattened vector size: " << outputVector.size() << endl; // 应该输出 9216

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Dropout

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////linear1
    vector<vector<float>> weight1 = readMatrixFromFile("fc6_weight.txt", 4096, 9216);
    std::vector<float> bias6 = readBiases("fc6_bias.txt");
    std::vector<float> fcOutput1 = linear(outputVector, weight1, bias6);
    // std::cout << "weight1 size: " << weight1.size() << std::endl;
    // std::cout << "bias6 size: " << bias6.size() << std::endl;
    // std::cout << "fcOutput1 size: " << fcOutput1.size() << std::endl;
    applyReLU1D(fcOutput1);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Dropout

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////linear2
    vector<vector<float>> weight2 = readMatrixFromFile("fc7_weight.txt", 4096, 4096);
    std::vector<float> bias7 = readBiases("fc7_bias.txt");
    std::vector<float> fcOutput2 = linear(fcOutput1, weight2, bias7);
    // std::cout << "weight1 size: " << weight1.size() << std::endl;
    // std::cout << "bias6 size: " << bias6.size() << std::endl;
    // std::cout << "fcOutput1 size: " << fcOutput1.size() << std::endl;
    applyReLU1D(fcOutput2);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ////linear3
    vector<vector<float>> weight3 = readMatrixFromFile("fc8_weight.txt", 1000, 4096);
    std::vector<float> bias8 = readBiases("fc8_bias.txt");
    std::vector<float> fcOutput3 = linear(fcOutput2, weight3, bias8);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // 读取ImageNet类别
    std::vector<std::string> classes = readImageNetClasses("imagenet_classes.txt");

    // 找到top 5的索引和值
    std::vector<std::pair<float, int>> top5;
    for (int i = 0; i < fcOutput3.size(); ++i) {
        top5.push_back(std::make_pair(fcOutput3[i], i));
    }

    std::partial_sort(top5.begin(), top5.begin() + 5, top5.end(), std::greater<>());

    // 打印top 5类别和值
    for (int i = 0; i < 5; ++i) {
        int index = top5[i].second;
        float value = top5[i].first;
        if (index < classes.size()) {
            std::cout << "Top " << i+1 << " class: " << classes[index] << " - Value: " << value << std::endl;
        } else {
            std::cout << "Invalid index: " << index << std::endl;
        }
    }

    return 0;
}