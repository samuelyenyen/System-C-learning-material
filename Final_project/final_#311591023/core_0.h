
#include "systemc.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cfloat> 
#include <iostream>
#include <fstream>


using namespace std;

SC_MODULE( Core_0 ) {
    sc_in  < bool >  rst;
    sc_in  < bool >  clk;
    // receive
    sc_in  < sc_lv<34> > flit_rx;   // The input channel
    sc_in  < bool > req_rx;         // The request associated with the input channel
    sc_out < bool > ack_rx;         // The outgoing ack signal associated with the input channel
    // transmit
    sc_out < sc_lv<34> > flit_tx;   // The output channel
    sc_out < bool > req_tx;         // The request associated with the output channel
    sc_in  < bool > ack_tx;         // The outgoing ack signal associated with the output channel
    

    std::vector<float> cw1_packet;
    std::vector<float> cw2_packet;
    std::vector<float> cw3_packet;
    std::vector<float> cw4_packet;
    std::vector<float> cw5_packet;
    std::vector<float> cb1_packet;
    std::vector<float> cb2_packet;
    std::vector<float> cb3_packet;
    std::vector<float> cb4_packet;
    std::vector<float> cb5_packet;
    std::vector<float> fc_w1_packet;
    std::vector<float> fc_w2_packet;
    std::vector<float> fc_w3_packet;
    std::vector<float> fc_b1_packet;
    std::vector<float> fc_b2_packet;
    std::vector<float> fc_b3_packet;
    std::vector<float> pic_packet;
    std::vector<float> con2in_packet;


    bool cw1_flag   =false;   
    bool cw2_flag   =false;   
    bool cw3_flag   =false;   
    bool cw4_flag   =false;   
    bool cw5_flag   =false;   
    bool cb1_flag   =false;   
    bool cb2_flag   =false;   
    bool cb3_flag   =false;   
    bool cb4_flag   =false;   
    bool cb5_flag   =false;   
    bool fc_w1_flag =false;
    bool fc_w2_flag =false;
    bool fc_w3_flag =false;
    bool fc_b1_flag =false;
    bool fc_b2_flag =false;
    bool fc_b3_flag =false;
    bool pic_flag   =true;
    bool data_from_contr   =true;

    std::vector<std::vector<std::vector<std::vector<float>>>> r_cw2_packet;
    std::vector<std::vector<std::vector<std::vector<float>>>> r_cw3_packet;
    std::vector<std::vector<std::vector<std::vector<float>>>> r_cw4_packet;
    std::vector<std::vector<std::vector<std::vector<float>>>> r_cw5_packet;
    std::vector<std::vector<std::vector<float>>> r_con2in_packet;

    
    sc_lv<4> layer_id;
    sc_lv<1> layer_id_type;
    sc_lv<2> destination;
    std::vector<sc_bv<34>> my_packet;
    int core_ID;
    bool start_flag = false;
    int count_packet = 0;
    bool resize_start = false;
    bool store_begin_0 = false;
    bool store_begin_00 = false;
    bool store_begin_1 = false;
    bool store_begin_5 = false;
    bool store_begin_4 = false;
    bool relu_start = false;
    bool maxp_start = false;
    bool adap_start = false;
    bool fc_start = false;
    bool core0_send = false;
    bool core1_send = false;
    bool core5_send = false;
    bool core4_send = false;
    bool core00_send = false;
    int count_c0 = 0;
    int count_c1 = 0;
    int count_c5 = 0;
    int count_c4 = 0;
    int count_c00 = 0;

    vector<float> output5_f;
    vector<float> finalOutput_f;
    vector<float> pooledOutput5_f;
    vector<float> output5_from_r0_f;
    vector<float> fcOutput3;
    vector<float> finalOutput_from_r4;


    vector<float> output5_f_from_r0;
    vector<float> output5_f_from_r1;
    vector<float> output5_f_from_r5;


    SC_CTOR(Core_0) {

        SC_METHOD(run);
        sensitive << clk.pos() << rst.neg();; 
        dont_initialize();

    }

    sc_uint<32> float_to_sc_uint(float f) {
        uint32_t int_rep = *reinterpret_cast<uint32_t*>(&f);
        sc_uint<32> uint_rep = int_rep;
        return uint_rep;
    }


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
                    float maxVal = -FLT_MAX;
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

        const int paddedHeight = height + 2 * paddingSize;
        const int paddedWidth = width + 2 * paddingSize;

        vector<vector<vector<float>>> output(depth, vector<vector<float>>(paddedHeight, vector<float>(paddedWidth, 0.0f)));

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
                    flatVector[index++] = input[d][h][w];
                }
            }
        }
        
        return flatVector;
    }


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


void run() {
        req_tx.write(true);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sc_lv<34> receive_data = flit_rx.read();
        
        if (receive_data.is_01()) {
            sc_lv<2> first_two_bits = receive_data.range(33, 32);
            sc_lv<4> destination = receive_data.range(8,5); // destination  
            uint32_t float_bits = receive_data.range(31, 0).to_uint();
            float recovered_float;
            memcpy(&recovered_float, &float_bits, sizeof(float));
            
            if (first_two_bits == 2 && destination==6 & data_from_contr==false) {
                store_begin_00 = 1;
                // finalOutput_from_r4.push_back(recovered_float);
                
            } else if (first_two_bits == 1 && store_begin_00 == 1) {
                store_begin_00 = 0;
                finalOutput_from_r4.push_back(recovered_float);
                cout << "finalOutput_from_r4.size() " <<finalOutput_from_r4.size()<<endl;
                fc_start = true;
            } else if (store_begin_00==1){
                finalOutput_from_r4.push_back(recovered_float); 
                // cout << "receive_data "<<receive_data<<endl;
            }

            if (first_two_bits == 2 && data_from_contr==1) {
                store_begin_0 = 1;

                if (pic_flag) pic_packet.push_back(recovered_float);
                else if (cw1_flag) cw1_packet.push_back(recovered_float);
                else if (cw2_flag) cw2_packet.push_back(recovered_float);
                else if (cw3_flag) cw3_packet.push_back(recovered_float);
                else if (cw4_flag) cw4_packet.push_back(recovered_float);
                else if (cw5_flag) cw5_packet.push_back(recovered_float);
                else if (cb1_flag) cb1_packet.push_back(recovered_float);
                else if (cb2_flag) cb2_packet.push_back(recovered_float);
                else if (cb3_flag) cb3_packet.push_back(recovered_float);
                else if (cb4_flag) cb4_packet.push_back(recovered_float);
                else if (cb5_flag) cb5_packet.push_back(recovered_float);
                else if (fc_w1_flag) fc_w1_packet.push_back(recovered_float);
                else if (fc_w2_flag) fc_w2_packet.push_back(recovered_float);
                else if (fc_w3_flag) fc_w3_packet.push_back(recovered_float);
                else if (fc_b1_flag) fc_b1_packet.push_back(recovered_float);
                else if (fc_b2_flag) fc_b2_packet.push_back(recovered_float);
                else if (fc_b3_flag) fc_b3_packet.push_back(recovered_float);
                
            } else if (first_two_bits == 1 && store_begin_0 == 1) {
                store_begin_0 = 0;

                if (pic_flag) {
                    pic_packet.push_back(recovered_float);
                    cout << "pic_packet.size() " <<pic_packet.size()<<endl;
                    pic_flag = false;
                    cw1_flag = true;
                } else if (cw1_flag) {
                    cw1_packet.push_back(recovered_float);
                    cout << "cw1_packet.size() " <<cw1_packet.size()<<endl;
                    cw1_flag = false;
                    cb1_flag = true;
                } else if (cw2_flag) {
                    cw2_packet.push_back(recovered_float);
                    cout << "cw2_packet.size() " <<cw2_packet.size()<<endl;
                    cw2_flag = false;
                    cb2_flag = true;
                } else if (cw3_flag) {
                    cw3_packet.push_back(recovered_float);
                    cout << "cw3_packet.size() " <<cw3_packet.size()<<endl;
                    cw3_flag = false;
                    cb3_flag = true;
                } else if (cw4_flag) {
                    cw4_packet.push_back(recovered_float);
                    cout << "cw4_packet.size() " <<cw4_packet.size()<<endl;
                    cw4_flag = false;
                    cb4_flag = true;
                } else if (cw5_flag) {
                    cw5_packet.push_back(recovered_float);
                    cout << "cw5_packet.size() " <<cw5_packet.size()<<endl;
                    cw5_flag = false;
                    cb5_flag = true;
                } else if (cb1_flag) {
                    cb1_packet.push_back(recovered_float);
                    cout << "cb1_packet.size() " <<cb1_packet.size()<<endl;
                    cb1_flag = false;
                    cw2_flag = true;
                } else if (cb2_flag) {
                    cb2_packet.push_back(recovered_float);
                    cout << "cb2_packet.size() " <<cb2_packet.size()<<endl;
                    cb2_flag = false;
                    cw3_flag = true;
                } else if (cb3_flag) {
                    cb3_packet.push_back(recovered_float);
                    cout << "cb3_packet.size() " <<cb3_packet.size()<<endl;
                    cb3_flag = false;
                    cw4_flag = true;
                } else if (cb4_flag) {
                    cb4_packet.push_back(recovered_float);
                    cout << "cb4_packet.size() " <<cb4_packet.size()<<endl;
                    cb4_flag = false;
                    cw5_flag = true;
                } else if (cb5_flag) {
                    cb5_packet.push_back(recovered_float);
                    cout << "cb5_packet.size() " <<cb5_packet.size()<<endl;
                    cb5_flag = false;
                    fc_w1_flag = true;
                } else if (fc_w1_flag) {
                    fc_w1_packet.push_back(recovered_float);
                    cout << "fc_w1_packet.size() " <<fc_w1_packet.size()<<endl;
                    fc_w1_flag = false;
                    fc_b1_flag = true;
                } else if (fc_w2_flag) {
                    fc_w2_packet.push_back(recovered_float);
                    cout << "fc_w2_packet.size() " <<fc_w2_packet.size()<<endl;
                    fc_w2_flag = false;
                    fc_b2_flag = true;
                } else if (fc_w3_flag) {
                    fc_w3_packet.push_back(recovered_float);
                    cout << "fc_w3_packet.size() " <<fc_w3_packet.size()<<endl;
                    fc_w3_flag = false;
                    fc_b3_flag = true;
                } else if (fc_b1_flag) {
                    fc_b1_packet.push_back(recovered_float);
                    cout << "fc_b1_packet.size() " <<fc_b1_packet.size()<<endl;
                    fc_b1_flag = false;
                    fc_w2_flag = true;
                } else if (fc_b2_flag) {
                    fc_b2_packet.push_back(recovered_float);
                    cout << "fc_b2_packet.size() " <<fc_b2_packet.size()<<endl;
                    fc_b2_flag = false;
                    fc_w3_flag = true;
                } else if (fc_b3_flag) {
                    fc_b3_packet.push_back(recovered_float);
                    cout << "fc_b3_packet.size() " <<fc_b3_packet.size()<<endl;
                    fc_b3_flag = false;
                    resize_start = true;
                    data_from_contr = false;
                }
                
            } else if (store_begin_0==1){
                if (pic_flag) {
                    pic_packet.push_back(recovered_float); 
                    // cout << "receive_data "<<receive_data<<endl;
                } else if (cw1_flag) cw1_packet.push_back(recovered_float);
                else if (cw2_flag) cw2_packet.push_back(recovered_float);
                else if (cw3_flag) cw3_packet.push_back(recovered_float);
                else if (cw4_flag) cw4_packet.push_back(recovered_float);
                else if (cw5_flag) cw5_packet.push_back(recovered_float);
                else if (cb1_flag) cb1_packet.push_back(recovered_float);
                else if (cb2_flag) cb2_packet.push_back(recovered_float);
                else if (cb3_flag) cb3_packet.push_back(recovered_float);
                else if (cb4_flag) cb4_packet.push_back(recovered_float);
                else if (cb5_flag) cb5_packet.push_back(recovered_float);
                else if (fc_w1_flag) fc_w1_packet.push_back(recovered_float);
                else if (fc_w2_flag) fc_w2_packet.push_back(recovered_float);
                else if (fc_w3_flag) fc_w3_packet.push_back(recovered_float);
                else if (fc_b1_flag) fc_b1_packet.push_back(recovered_float);
                else if (fc_b2_flag) fc_b2_packet.push_back(recovered_float);
                else if (fc_b3_flag) fc_b3_packet.push_back(recovered_float);
            }
        }

        
        if (resize_start ){ 
            std::vector<std::vector<std::vector<float>>> r_pic_packet(224, std::vector<std::vector<float>>(224, std::vector<float>(3)));
            cout<< " pic_packet.size() " <<pic_packet.size()<< endl;
            int count = 0;
            for (int k = 0; k < 3; ++k) {
                for (int i = 0; i < 224; ++i) {
                    for (int j = 0; j < 224; ++j) {
                        r_pic_packet[i][j][k] = pic_packet[count];
                        count = count+1;
                    }
                }
            }

            count = 0;
            int k,i,j;
            std::vector<std::vector<std::vector<std::vector<float>>>> r_cw1_packet(64, std::vector<std::vector<std::vector<float>>>(3, std::vector<std::vector<float>>(11, std::vector<float>(11))));
            cout<< " cw1_packet.size() " <<cw1_packet.size() <<endl;
            for (k = 0; k < 64; ++k) {
                for (i = 0; i < 3; ++i) {
                    for (j = 0; j < 11; ++j) {
                        for (int l = 0; l < 11; ++l) {
                            r_cw1_packet[k][i][j][l] = cw1_packet[count];
                            count = count+1;
                        }
                    }
                }
            }

            std::vector<std::vector<std::vector<std::vector<float>>>> r_cw2_packet(192, std::vector<std::vector<std::vector<float>>>(64, std::vector<std::vector<float>>(5, std::vector<float>(5))));
            count = 0;
            cout<< " cw2_packet.size() " <<cw2_packet.size() <<endl;
            for (k = 0; k < 192; ++k) {
                for (i = 0; i < 64; ++i) {
                    for (j = 0; j < 5; ++j) {
                        for (int l = 0; l < 5; ++l) {
                            r_cw2_packet[k][i][j][l] = cw2_packet[count];
                            count = count+1;
                        }
                    }
                }
            }

            std::vector<std::vector<std::vector<std::vector<float>>>> r_cw3_packet(384, std::vector<std::vector<std::vector<float>>>(192, std::vector<std::vector<float>>(3, std::vector<float>(3))));
            count = 0;
            cout<< " cw3_packet.size() " <<cw3_packet.size() <<endl;
            for (k = 0; k < 384; ++k) {
                for (i = 0; i < 192; ++i) {
                    for (j = 0; j < 3; ++j) {
                        for (int l = 0; l < 3; ++l) {
                            r_cw3_packet[k][i][j][l] = cw3_packet[count];
                            count = count+1;
                        }
                    }
                }
            }

            std::vector<std::vector<std::vector<std::vector<float>>>> r_cw4_packet(256, std::vector<std::vector<std::vector<float>>>(384, std::vector<std::vector<float>>(3, std::vector<float>(3))));
            count = 0;
            cout<< " cw4_packet.size() " <<cw4_packet.size() <<endl;
            for (k = 0; k < 256; ++k) {
                for (i = 0; i < 384; ++i) {
                    for (j = 0; j < 3; ++j) {
                        for (int l = 0; l < 3; ++l) {
                            r_cw4_packet[k][i][j][l] = cw4_packet[count];
                            count = count+1;
                        }
                    }
                }
            }

            std::vector<std::vector<std::vector<std::vector<float>>>> r_cw5_packet(256, std::vector<std::vector<std::vector<float>>>(256, std::vector<std::vector<float>>(3, std::vector<float>(3))));
            count = 0;
            cout<< " cw5_packet.size() " <<cw5_packet.size() <<endl;
            for (k = 0; k < 256; ++k) {
                for (i = 0; i < 256; ++i) {
                    for (j = 0; j < 3; ++j) {
                        for (int l = 0; l < 3; ++l) {
                            r_cw5_packet[k][i][j][l] = cw5_packet[count];
                            count = count+1;
                        }
                    }
                }
            }

            
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            vector<vector<vector<float>>> resizedImage = resizeImage(r_pic_packet, 227, 227);
            vector<vector<vector<float>>> transposedVector = transpose3DVector(resizedImage);

            auto output = conv(transposedVector, r_cw1_packet, cb1_packet, 4);
            applyReLU(output);
            auto pooledOutput = applyMaxPooling(output, 2, 2);
            auto paddedOutput = applyZeroPadding(pooledOutput, 2);
            auto output2 = conv(paddedOutput, r_cw2_packet, cb2_packet, 1);
            applyReLU(output2);
            auto pooledOutput2 = applyMaxPooling(output2, 3, 2);
            auto paddedOutput3 = applyZeroPadding(pooledOutput2, 1);
            auto output3 = conv(paddedOutput3, r_cw3_packet, cb3_packet, 1);
            applyReLU(output3);
            auto paddedOutput4 = applyZeroPadding(output3, 1);
            auto output4 = conv(paddedOutput4, r_cw4_packet, cb4_packet, 1);
            applyReLU(output4);
            auto paddedOutput5 = applyZeroPadding(output4, 1);
            auto output5 = conv(paddedOutput5, r_cw5_packet, cb5_packet, 1);



            output5_f = flattenVector(output5); 
            
            resize_start = false;
            cout << "core0 complete1 at time " << sc_time_stamp() << endl;
            core0_send = true;
        }

        if (core0_send) {
            sc_bv<34> data_read_v;
            // cout << "count_c0 " << count_c0 << endl;
            if (count_c0==0) {
                data_read_v.range(33,32) = "10";
                data_read_v.range(31,9) = "00000000000000000000000";
                data_read_v.range(8,5) = "0001"; // destination  
                data_read_v.range(4,0) = "00000";
            } else if (count_c0 == output5_f.size()) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0) = float_to_sc_uint(output5_f[count_c0-1]);
                core0_send = false;
            } else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0) = float_to_sc_uint(output5_f[count_c0-1]);       
            }
            flit_tx.write(data_read_v);
            count_c0 = count_c0+1;
        }
        
        if (fc_start){
            // fc6
            vector<vector<float>> r_fc_w1_packet(4096, vector<float>(9216));
            int count = 0;
            cout<< " fc_w1_packet " <<fc_w1_packet.size() <<endl;
            for (int i = 0; i < 4096; ++i) {
                for (int j = 0; j < 9216; ++j) {
                    r_fc_w1_packet[i][j] = fc_w1_packet[count];
                    count = count+1;
                }
            }
            // fc7
            
            vector<vector<float>> r_fc_w2_packet(4096, vector<float>(4096));
            count = 0;
            cout<< " fc_w2_packet.size() " <<fc_w2_packet.size() <<endl;
            for (int i = 0; i < 4096; ++i) {
                for (int j = 0; j < 4096; ++j) {
                    r_fc_w2_packet[i][j] = fc_w2_packet[count];
                    count = count+1;
                }
            }
            // fc8
            
            vector<vector<float>> r_fc_w3_packet(1000, vector<float>(4096));
            count = 0;
            cout<< " fc_w3_packet.size() " <<fc_w3_packet.size() <<endl;
            for (int i = 0; i < 1000; ++i) {
                for (int j = 0; j < 4096; ++j) {
                    r_fc_w3_packet[i][j] = fc_w3_packet[count];
                    count = count+1;
                }
            }
            
            std::vector<float> fcOutput1 = linear(finalOutput_from_r4, r_fc_w1_packet, fc_b1_packet);
            applyReLU1D(fcOutput1);
            std::vector<float> fcOutput2 = linear(fcOutput1, r_fc_w2_packet, fc_b2_packet);
            applyReLU1D(fcOutput2);
            fcOutput3 = linear(fcOutput2, r_fc_w3_packet, fc_b3_packet);

        
            core00_send = true;
            fc_start = false;
            cout << "core0 complete2 at time " << sc_time_stamp() << endl;
        }
        if (core00_send){
            sc_bv<34> data_read_v;
            if (count_c00==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,9) = "00000000000000000000000";
                data_read_v.range(8,5) = "0111"; // destination  
                data_read_v.range(4,0) = "00000";
            } else if (count_c00 == fcOutput3.size()) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0) = float_to_sc_uint(fcOutput3[count_c00-1]);
                core00_send = false;
            } else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0) = float_to_sc_uint(fcOutput3[count_c00-1]);       
            }
            flit_tx.write(data_read_v);
            count_c00 = count_c00+1;
        }   
    };
};
