#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "systemc.h"
#include <vector>
using namespace std;

SC_MODULE(Controller) {
    sc_in<bool> rst;
    sc_in<bool> clk;

    // to ROM
    sc_out<int> layer_id;
    sc_out<bool> layer_id_type;
    sc_out<bool> layer_id_valid;

    // from ROM
    sc_in<float> data;
    sc_in<bool> data_valid;

    // // to router0
    sc_out < sc_lv<34> > flit_tx;
    sc_out < bool > req_tx;
    sc_in  < bool > ack_tx;

    // // from router0
    sc_in  < sc_lv<34> > flit_rx;
    sc_in  < bool > req_rx;
    sc_out < bool > ack_rx;
    //////////////////////////////////////////////////////////////
    std::vector<float> big_packet;
    std::vector<sc_bv<34>> r_big_packet;
    int send_count = 0;
    vector<float> fcOutput3;
    
    sc_uint<32> float_to_sc_uint(float f) {
        uint32_t int_rep = *reinterpret_cast<uint32_t*>(&f);
        sc_uint<32> uint_rep = int_rep;
        return uint_rep;
    }

    int done_for_pic = 0;
    int done_for_cw1 = 0;
    int done_for_cb1 = 0;
    int done_for_cw2 = 0;
    int done_for_cb2 = 0;
    int done_for_cw3 = 0;
    int done_for_cb3 = 0;
    int done_for_cw4 = 0;
    int done_for_cb4 = 0;
    int done_for_cw5 = 0;
    int done_for_cb5 = 0;
    int done_for_fc1_w = 0;
    int done_for_fc1_b = 0;
    int done_for_fc2_w = 0;
    int done_for_fc2_b = 0;
    int done_for_fc3_w = 0;
    int done_for_fc3_b=  0;
    bool send_start = false;
    bool start_rom = true;
    bool store_begin_00 = false;
    bool print_result = false;

    SC_CTOR(Controller) {
        SC_THREAD(Read_input);
        sensitive << clk.pos() << rst.neg();
        dont_initialize();

        SC_METHOD(RX);
        sensitive << clk.pos() << rst.neg();
        dont_initialize();
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

void Read_input() {
    
    int flag = 0;
    int data_count = 0;

    int flag_for_cw1 = 0;
    int cw1_count = 0;

    int flag_for_cb1 = 0;
    int cb1_count = 0;

    int flag_for_cw2 = 0;
    int cw2_count = 0;

    int flag_for_cb2 = 0;
    int cb2_count = 0;

    int flag_for_cw3 = 0;
    int cw3_count = 0;

    int flag_for_cb3 = 0;
    int cb3_count = 0;

    int flag_for_cw4 = 0;
    int cw4_count = 0;

    int flag_for_cb4 = 0;
    int cb4_count = 0;

    int flag_for_cw5 = 0;
    int cw5_count = 0;
    
    int flag_for_cb5 = 0;
    int cb5_count = 0;

    int flag_for_fc1w = 0;
    int fc1w_count = 0;

    int flag_for_fc1b = 0;
    int fc1b_count = 0;

    int flag_for_fc2w = 0;
    int fc2w_count = 0;

    int flag_for_fc2b = 0;
    int fc2b_count = 0;

    int flag_for_fc3w = 0;
    int fc3w_count = 0;

    int flag_for_fc3b = 0;
    int fc3b_count = 0;

    req_tx.write(true);
    ack_rx.write(true);
    while (start_rom) {

        // cout <<"data_count"<<data_count<<endl;
        if (flag == 0) {
            layer_id.write(0);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag = 1;
        }

        wait();

        layer_id_valid.write(false);

        if (data_valid.read()) {
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (data_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (data_count==150527) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            // std::cout << "data_count " << data_count<< std::endl;
            data_count++;
        }
        else if (data_count==150528) {
            done_for_pic = 1;  
            start_rom = false;
            break;  
        }
    }
    if (done_for_pic == 1) {
        std::cout << "All data for the picture has been received." << std::endl;
    }//////////// read in conv layer 1 weights
    while(done_for_pic){
        if(flag_for_cw1==0){
            layer_id.write(1);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_cw1 = 1;
        }
        wait();
        layer_id_valid.write(false);

        if (data_valid.read()) {
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cw1_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cw1_count==23231) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cw1_count++;
        }
        else if(cw1_count==23232){
            done_for_cw1 = 1;
            done_for_pic = false;
            break;
        }
    }
    if(done_for_cw1 ==1){
        std::cout << "ALL data for CW1 is received " << std::endl;
    }
    while (done_for_cw1){
        if(flag_for_cb1==0){
            layer_id.write(1);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_cb1 = 1;
        }
        wait();
        layer_id_valid.write(false);

        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cb1_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cb1_count==63) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cb1_count++;
        }
        else if(cb1_count==64){
            done_for_cb1 = 1;
            done_for_cw1 = false;
            break;
        }
    }
    if(done_for_cb1==1){
        cout<< "ALL data for CB1 is received "<<endl;
    }////////// 
    while (done_for_cb1){
        if(flag_for_cw2==0){
            layer_id.write(2);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_cw2 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cw2_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cw2_count==307199) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cw2_count++;
        }
        else if(cw2_count==307200){
            done_for_cw2 = 1;
            done_for_cb1 = false;
            break;
        }
    }
    if(done_for_cw2==1){
        cout<<"ALL data Form CW2 is received "<<endl;
    }
    while(done_for_cw2){
        if(flag_for_cb2==0){
            layer_id.write(2);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_cb2 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cb2_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cb2_count==191) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cb2_count++;
        }
        else if(cb2_count==192){
            done_for_cb2 = 1;
            done_for_cw2 = false;
            break;
        }     
    }
     if(done_for_cb2==1){
        cout<<"ALL data Form Cb2 is received "<<endl;
     }
    while(done_for_cb2){
        if(flag_for_cw3==0){
            layer_id.write(3);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_cw3 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cw3_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cw3_count==663551) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cw3_count++;
            // cout<<cw3_count<<endl;
        }
        else if(cw3_count==663552){
            done_for_cw3 = 1;
            done_for_cb2 = false;
            break;
        }
    }
    if(done_for_cw3){
         cout<<"ALL data Form CW3 is received "<<endl;
    }
    while(done_for_cw3){
        if(flag_for_cb3==0){
            layer_id.write(3);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_cb3 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cb3_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cb3_count==383) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cb3_count++;
        }
        else if(cb3_count==384){
            done_for_cb3 = 1;
            done_for_cw3 = false;
            break;
        }
    }
    if(done_for_cb3){
         cout<<"ALL data Form CB3 is received "<<endl;
    }
    while(done_for_cb3){
        if(flag_for_cw4==0){
            layer_id.write(4);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_cw4 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cw4_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cw4_count==884735) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cw4_count++;
        }
         else if(cw4_count==884736){
            done_for_cw4 = 1;
            done_for_cb3 = false;
            break;
        }
    }
     if(done_for_cw4){
         cout<<"ALL data Form CW4 is received "<<endl;
    }
    while(done_for_cw4){
        if(flag_for_cb4==0){
            layer_id.write(4);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_cb4 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cb4_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cb4_count==255) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cb4_count++;
        }
        else if(cb4_count==256){
            done_for_cb4 = 1;
            done_for_cw4 = false;
            break;
        }
    }
    if(done_for_cb4){
         cout<<"ALL data Form CB4 is received "<<endl;
    }
    while(done_for_cb4){
        if(flag_for_cw5==0){
            layer_id.write(5);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_cw5 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cw5_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cw5_count==589823) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cw5_count++;
        }
        else if(cw5_count==589824){
            done_for_cw5 = 1;
            done_for_cb4 = false;
            break;
        }
    }
    if(done_for_cw5){
         cout<<"ALL data Form CW5 is received "<<endl;
    }
    while(done_for_cw5){
        if(flag_for_cb5==0){
            layer_id.write(5);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_cb5 = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (cb5_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (cb5_count==255) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            cb5_count++;
        }
        else if(cb5_count==256){
            done_for_cb5 = 1;
            done_for_cw5 = false;
            break;
        }
    }
    if(done_for_cb5){
        cout<<"ALL data Form CB5 is received "<<endl;
        cout<<" DONE FOR ALL CONV LAYER READING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
    }
    while(done_for_cb5){
        if(flag_for_fc1w==0){
            layer_id.write(6);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_fc1w = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (fc1w_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (fc1w_count==37748735) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            fc1w_count++;
        }
        else if(fc1w_count==37748736){
            done_for_fc1_w = 1;
            done_for_cb5 = false;
            break;
        }
    }
    if(done_for_fc1_w){
        cout<<"ALL data from FC1W is rececived "<<endl;
    }
    while(done_for_fc1_w){
        if(flag_for_fc1b==0){
            layer_id.write(6);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_fc1b = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (fc1b_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (fc1b_count==4095) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            fc1b_count++;
        }
        else if(fc1b_count==4096){
            done_for_fc1_b = 1;
            done_for_fc1_w = false;
            break;
        }
    }
    if(done_for_fc1_b){
        cout<<"ALL data from FC1B is rececived "<<endl;
    }
    while(done_for_fc1_b){
         if(flag_for_fc2w==0){
            layer_id.write(7);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_fc2w = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (fc2w_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (fc2w_count==16777215) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            fc2w_count++;
        }
        else if(fc2w_count==16777216){
            done_for_fc2_w = 1;
            done_for_fc1_b = false;
            break;
        }
    }
    if(done_for_fc2_w){
        cout<<"ALL data from FC2W is rececived "<<endl;
    }
    while(done_for_fc2_w){
        if(flag_for_fc2b==0){
            layer_id.write(7);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_fc2b = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (fc2b_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (fc2b_count==4095) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            fc2b_count++;
        }
        else if(fc2b_count==4096){
            done_for_fc2_b = 1;
            done_for_fc2_w = false;
            break;
        }
    }
    if(done_for_fc2_b){
        cout<<"ALL data from FC2B is rececived "<<endl;
    }
    while(done_for_fc2_b){
        if(flag_for_fc3w==0){
            layer_id.write(8);
            layer_id_type.write(0);
            layer_id_valid.write(true);
            flag_for_fc3w = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (fc3w_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (fc3w_count==4095999) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            fc3w_count++;
        }
        else if(fc3w_count==4096000){
            done_for_fc3_w = 1;
            done_for_fc2_b = false;
            break;
        }
    }
    if(done_for_fc3_w){
        cout<<"ALL data from FC3W is rececived "<<endl;
    }
    while(done_for_fc3_w){
        if(flag_for_fc3b==0){
            layer_id.write(8);
            layer_id_type.write(1);
            layer_id_valid.write(true);
            flag_for_fc3b = 1;
        }
        wait();
        layer_id_valid.write(false);
        if(data_valid.read()){
            float data_read = data.read();
            sc_bv<34> data_read_v;
            if (fc3b_count==0) {
                data_read_v.range(33,32) = 10;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else if (fc3b_count==999) {
                data_read_v.range(33,32) = 01;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            else {
                data_read_v.range(33,32) = 00;
                data_read_v.range(31,0)  = float_to_sc_uint(data_read);
            }
            flit_tx.write(data_read_v);
            fc3b_count++;
        }
        else if(fc3b_count==1000){
            done_for_fc3_b = 1;
            done_for_fc3_w =  false;
            break;
        }
    }
     if(done_for_fc3_b){
        sc_bv<34> data_read_v;
        data_read_v.range(33,32) = 11;
        data_read_v.range(31,0)  = "00000000000000000000000000000000";
        flit_tx.write(data_read_v);
        cout<<"ALL data from FC3B is rececived "<<endl;
        done_for_fc3_b = false;
    }


    

}

void RX() {
    sc_lv<34> receive_data = flit_rx.read();
    
    if (receive_data.is_01()) {
        sc_lv<2> first_two_bits = receive_data.range(33, 32);
        sc_lv<4> destination = receive_data.range(8,5); // destination  
        uint32_t float_bits = receive_data.range(31, 0).to_uint();
        float recovered_float;
        memcpy(&recovered_float, &float_bits, sizeof(float));
        if (first_two_bits == 2 && destination==7) {
            store_begin_00 = true;
            // fcOutput3.push_back(recovered_float);
            
        } else if (first_two_bits == 1 && store_begin_00 == 1) {
            store_begin_00 = false;
            fcOutput3.push_back(recovered_float);
            cout << "fcOutput3 in Controller " <<fcOutput3.size()<<endl;
            print_result = true;
        } else if (store_begin_00==true){
            fcOutput3.push_back(recovered_float); 
        }

        if (print_result) {
            std::vector<std::string> classes = readImageNetClasses("/RAID2/COURSE/mlchip/mlchip081/hw4_s/imagenet_classes.txt");

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
            print_result = false;
        }
    }
}





    
};

#endif // CONTROLLER_H
