#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"
#include <bitset>
#include <cstring> // 用于memcpy

SC_MODULE( Core ) {
    sc_in  < bool >  rst;
    sc_in  < bool >  clk;
    // receive
    sc_in  < sc_lv<34> > flit_rx;	// The input channel
    sc_in  < bool > req_rx;	        // The request associated with the input channel
    sc_out < bool > ack_rx;	        // The outgoing ack signal associated with the input channel
    // transmit
    sc_out < sc_lv<34> > flit_tx;	// The output channel
    sc_out < bool > req_tx;	        // The request associated with the output channel
    sc_in  < bool > ack_tx;	        // The outgoing ack signal associated with the output channel

    PE pe;
    Packet* init_pkt;
    Packet* final_pkt = new Packet;
    sc_event trigger_event; // 自定義事件用於觸發一次性方法
    vector<sc_lv<34>> lv_vector;
    vector<sc_lv<34>> re_lv_vector;
    bool init_done=false;
    bool store_begin = false;
    int counter;

    SC_CTOR(Core) {
        // Process received data on positive clock edge
        SC_METHOD(receive_process);
        sensitive << clk.pos();
        dont_initialize();  // Avoid running on initialization

        // Process data to transmit on positive clock edge
        SC_METHOD(transmit_process);
        sensitive << clk.pos();
        dont_initialize();  // Avoid running on initialization

        SC_METHOD(init);
        sensitive << trigger_event;  // 使方法對時鐘的正邊緣敏感
        dont_initialize();
    }

    void init() {
        init_pkt = pe.get_packet();

        if (init_pkt != nullptr && !init_pkt->datas.empty()) {
            // cout << "Source ID: " << init_pkt->source_id << endl;
            // cout << "Destination ID: " << init_pkt->dest_id << endl;
            sc_lv<34> special_entry;
            sc_lv<2> prefix = "10"; // The first 2 bits
            sc_lv<4> source_bits = sc_lv<4>(init_pkt->source_id);
            sc_lv<4> dest_bits = sc_lv<4>(init_pkt->dest_id);
            sc_lv<24> padding = "000000000000000000000000"; // Remaining 24 bits
            special_entry = (prefix, source_bits, dest_bits, padding);
            lv_vector.push_back(special_entry);
            // std::cout << "Special entry: " << special_entry << std::endl;

            for (size_t i = 0; i < init_pkt->datas.size(); ++i) {
                uint32_t binary_representation;
                memcpy(&binary_representation, &init_pkt->datas[i], sizeof(float));   // 将浮点数内存拷贝到整数变量

                // 使用bitset来将整数表示的浮点数转换成32位的二进制形式
                // cout << "First data element (binary): " << std::bitset<32>(binary_representation) << endl;
                sc_lv<34> lv_value;
                if (i == init_pkt->datas.size() - 1) {
                    lv_value = (sc_lv<2>("01"), sc_lv<32>(binary_representation));
                } else {
                    lv_value = (sc_lv<2>("00"), sc_lv<32>(binary_representation));
                }

                lv_vector.push_back(lv_value);
                // std::cout << "Converted " << init_pkt->datas[i] << " to " << lv_value << std::endl;
            }
            // cout << "original data size "<< init_pkt->datas.size()<<" now data size " << lv_vector.size() <<endl;
            // cout << "data size " << lv_vector.size() <<endl;
            init_done = true;
        } else {
            // cout << endl;
            // cout << "No packet retrieved or data is empty." << endl;
            // cout << endl;
        }
        
        counter = 0;
        // cout << "init_done" << endl;
    }

    void receive_process() {
        if (req_rx.read() == true){
            ack_rx.write(true);  // No data or not acknowledged, do not transmit
            sc_lv<34> receive_data = flit_rx.read();
            sc_lv<2> first_two_bits = receive_data.range(33, 32);

            if (first_two_bits == 2) {
                store_begin = 1;
                final_pkt->source_id = receive_data.range(31, 28).to_uint();
                final_pkt->dest_id = receive_data.range(27, 24).to_uint();
                // final_pkt->dest_id = 3;
                re_lv_vector.push_back(receive_data);
                if (init_pkt->source_id==0){
                    // cout << "Received Data: " << receive_data << endl;
                    // cout << "source_id: " << final_pkt->source_id << endl;
                    // cout << "dest_id: " << final_pkt->dest_id << endl;
                }

            } else if (first_two_bits == 1 && store_begin == 1) {
                
                // cout << "source_id: " << final_pkt->source_id << endl;
                // cout << "dest_id: " << final_pkt->dest_id << endl;
                store_begin = 0;
                re_lv_vector.push_back(receive_data);
                // if (init_pkt->source_id==0){
                    // cout << "congratulation!!" << endl;
                    // cout << "Received Data: " << receive_data << endl;
                    // cout << re_lv_vector.size() <<endl;
                // }
                vector<float> converted_back;
                for (size_t i = 1; i < re_lv_vector.size(); ++i) {
                    uint32_t float_bits = re_lv_vector[i].range(31, 0).to_uint();
                    float recovered_float;
                    memcpy(&recovered_float, &float_bits, sizeof(float));

                    converted_back.push_back(recovered_float);
                }
                // cout << "congratulation!! from " <<final_pkt->source_id<<" to "<<final_pkt->dest_id<< "  Current simulation time: " << sc_time_stamp()<< endl;

                final_pkt->datas = converted_back;
                pe.check_packet(final_pkt);
            } else if (store_begin==1){
                re_lv_vector.push_back(receive_data);
                if (init_pkt->source_id==4){
                    // cout << "Received Data: " << receive_data << endl;
                }
            }
        } 
    }

    void transmit_process() {
        if (rst.read()==true) {
            req_tx.write(false);
        }
        else if (init_done == 1 && init_pkt != nullptr && !init_pkt->datas.empty()) {  // Check for acknowledgment and data availability
            // cout << "wait" << endl;
            req_tx.write(true);  // Indicate a new request to transmit
             // Convert packet data back to sc_lv and transmit
            // wait(ack_tx.posedge_event());
            // cout << "rst event" << endl;
            // if (ack_tx.read()) {
            // for (size_t i = 0; i < lv_vector.size(); ++i) {
            if (counter <lv_vector.size()) {
                // cout << counter << endl;
                // cout << lv_vector[counter] << endl;
                if (ack_tx.read()==true) {
                    flit_tx.write(lv_vector[counter]);
                    counter = counter+1;
                }
            }
            else if (counter==lv_vector.size()){
                counter = 0;
                init_done = false;
                req_tx.write(false);
                lv_vector.clear();
            }
                

                
            // }
            // }
            // req_tx.write(false);  // Indicate a new request to transmit
            // init_done = false;
        } 
    }
};

#endif