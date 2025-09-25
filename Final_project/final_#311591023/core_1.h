
#include "systemc.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cfloat> // 对于 FLT_MAX
#include <iostream>
#include <fstream>


using namespace std;

SC_MODULE( Core_1 ) {
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


    SC_CTOR(Core_1) {

        SC_METHOD(run);
        sensitive << clk.pos() << rst.neg();; 
        dont_initialize();

    }

    sc_uint<32> float_to_sc_uint(float f) {
        uint32_t int_rep = *reinterpret_cast<uint32_t*>(&f);
        sc_uint<32> uint_rep = int_rep;
        return uint_rep;
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
            // cout << "receive_data "<<receive_data<<endl;
           

            if (first_two_bits == 2 && destination==1) {
                store_begin_1 = 1;
                // output5_f_from_r0.push_back(recovered_float);
                // cout << "receive_data "<<receive_data<<endl;
            } else if (first_two_bits == 1 && store_begin_1 == 1) {
                store_begin_1 = 0;
                output5_f_from_r0.push_back(recovered_float);
                cout << "output5_f_from_r0.size() " <<output5_f_from_r0.size()<<endl;
                relu_start = true;
                // cout << "receive_data "<<receive_data<<endl;
            } else if (store_begin_1==1){
                output5_f_from_r0.push_back(recovered_float); 
                // cout << "receive_data "<<receive_data<<endl;
            }
        }
        
        if (relu_start) {
            int index = 0;
            std::vector<std::vector<std::vector<float>>> output5_from_r0(256, std::vector<std::vector<float>>(13, std::vector<float>(13)));
            for (int d = 0; d < 256; ++d) {
                for (int h = 0; h < 13; ++h) {
                    for (int w = 0; w < 13; ++w) {
                        output5_from_r0[d][h][w] = output5_f_from_r0[index++];
                    }
                }
            }
            applyReLU(output5_from_r0);
            output5_from_r0_f = flattenVector(output5_from_r0);  
            
            relu_start = false;
            cout << "core1 complete at time " << sc_time_stamp() << endl;
            core1_send = true;
        }
        if (core1_send) {
            sc_bv<34> data_read_v;
            if (count_c1==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,9) = "00000000000000000000000";
                data_read_v.range(8,5) = "0101"; // destination  
                data_read_v.range(4,0) = "00000";
            } else if (count_c1 == output5_from_r0_f.size()) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0) = float_to_sc_uint(output5_from_r0_f[count_c1-1]);
                core1_send = false;
            } else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0) = float_to_sc_uint(output5_from_r0_f[count_c1-1]);       
            }
            flit_tx.write(data_read_v);
            count_c1 = count_c1+1;
        }
    };
};
