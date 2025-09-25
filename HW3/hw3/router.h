#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"

SC_MODULE( Router ) {
    sc_in  < bool >  rst;
    sc_in  < bool >  clk;

    sc_out < sc_lv<34> >  out_flit[5];
    sc_out < bool >  out_req[5];
    sc_in  < bool >  in_ack[5];

    sc_in  < sc_lv<34> >  in_flit[5];
    sc_in  < bool >  in_req[5];
    sc_out < bool >  out_ack[5];

    bool store_begin0 = 0;
    bool store_begin1 = 0;
    bool store_begin2 = 0;
    bool store_begin3 = 0;
    bool store_begin4 = 0;
    vector<sc_lv<34>> lv_vector4;
    vector<sc_lv<34>> lv_vector0;
    vector<sc_lv<34>> lv_vector1;
    vector<sc_lv<34>> lv_vector2;
    vector<sc_lv<34>> lv_vector3;
    sc_lv<4> addr_14;
    sc_lv<4> addr_10;
    sc_lv<4> addr_11;
    sc_lv<4> addr_12;
    sc_lv<4> addr_13;

    sc_lv<4> addr_24;
    vector<sc_lv<4>> addr_20;
    vector<sc_lv<4>> addr_21;
    vector<sc_lv<4>> addr_22;
    vector<sc_lv<4>> addr_23;
    bool flag_from_core = 0;
    bool router_to_router0 = 0;
    bool router_to_router1 = 0;
    bool router_to_router2 = 0;
    bool router_to_router3 = 0;
    int counter0 = 0;
    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;
    int counter4 = 0;
    vector<int> data_num0;
    vector<int> data_num1;
    vector<int> data_num2;
    vector<int> data_num3;
    int index0 = 0;
    int index1 = 0;
    int index2 = 0;
    int index3 = 0;
    int send_num0 = 0;
    int send_num1 = 0;
    int send_num2 = 0;
    int send_num3 = 0;
    int router_ID;
    bool tx_busy_00 = false;
    bool tx_busy_10 = false;
    bool tx_busy_20 = false;
    bool tx_busy_30 = false;
    bool tx_busy_01 = false;
    bool tx_busy_11 = false;
    bool tx_busy_21 = false;
    bool tx_busy_31 = false;
    bool tx_busy_02 = false;
    bool tx_busy_12 = false;
    bool tx_busy_22 = false;
    bool tx_busy_32 = false;
    bool tx_busy_03 = false;
    bool tx_busy_13 = false;
    bool tx_busy_23 = false;
    bool tx_busy_33 = false;
    bool tx_busy_04 = false;
    bool tx_busy_14 = false;
    bool tx_busy_24 = false;
    bool tx_busy_34 = false;

    void set_router_ID(int new_ID) {
        router_ID = new_ID;
    }

    // Constructor that accepts a name
    SC_CTOR(Router){
        SC_METHOD(data_from_core);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(data_from_rounter);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(data_to_rounter);
        sensitive << clk.pos();
        dont_initialize();
    }

    void data_from_core() {
        if (in_req[4].read() == true){
            out_ack[4].write(true);  // No data or not acknowledged, do not transmit
            sc_lv<34> receive_data = in_flit[4].read();
            sc_lv<2> first_two_bits = receive_data.range(33, 32);

            if (first_two_bits == 2) {
                store_begin4 = 1;
                addr_14 = receive_data.range(31, 28);
                addr_24 = receive_data.range(27, 24);
                lv_vector4.push_back(receive_data);
                if (router_ID==13){
                    // cout << "Received Data: " << receive_data << endl;
                }

            } else if (first_two_bits == 1 && store_begin4 == 1) {
                store_begin4 = 0;
                flag_from_core = 1;
                lv_vector4.push_back(receive_data);
                if (router_ID==13){
                    // cout << "Received Data: " << receive_data << endl;
                    // cout << lv_vector4.size() <<endl;
                }
                // cout << "RouterID " << router_ID <<endl;
            } else if (store_begin4==1){
                lv_vector4.push_back(receive_data);
                if (router_ID==13){
                    // cout << "Received Data: " << receive_data << endl;
                }
                // cout << "addr_1 : " << addr_1 << endl;
            }
        } 
    }

    void data_to_rounter() {
        int row_1 = router_ID/4;
        int col_1 = router_ID%4;
        // int direction_4 = (row_1>row_24)? 0: (row_1<row_24)? 1: (col_1>col_24)? 2: (col_1<col_24)? 3: (row_1==row_24&&col_1==col_24)? 4;
        // int direction_3 = (row_1>row_23)? 0: (row_1<row_23)? 1: (col_1>col_23)? 2: (col_1<col_23)? 3: (row_1==row_23&&col_1==col_23)? 4;
        // int direction_2 = (row_1>row_22)? 0: (row_1<row_22)? 1: (col_1>col_22)? 2: (col_1<col_22)? 3: (row_1==row_22&&col_1==col_22)? 4;
        // int direction_1 = (row_1>row_21)? 0: (row_1<row_21)? 1: (col_1>col_21)? 2: (col_1<col_21)? 3: (row_1==row_21&&col_1==col_21)? 4;
        // int direction_0 = (row_1>row_20)? 0: (row_1<row_20)? 1: (col_1>col_20)? 2: (col_1<col_20)? 3: (row_1==row_20&&col_1==col_20)? 4;
        // if (addr_14==7 && (flag_from_core!=0&&counter4==0 || router_to_router0!=0&&counter0==0 || router_to_router1!=0&&counter1==0 || router_to_router2!=0&&counter2==0 || router_to_router3!=0&&counter3==0)) {
        //     cout << "flag_from_core   : " << flag_from_core    <<endl;
        //     cout << "router_to_router0: " << router_to_router0 <<endl;
        //     cout << "router_to_router1: " << router_to_router1 <<endl;
        //     cout << "router_to_router2: " << router_to_router2 <<endl;
        //     cout << "router_to_router3: " << router_to_router3 <<endl;
        //     cout << "Current simulation time: " << sc_time_stamp() << endl;
        // }
        // bool flag = ((addr_20==0||addr_21==0||addr_22==0||addr_23==0||addr_24==0) && (flag_from_core!=0&&counter4==0 || router_to_router0!=0&&counter0==0 || router_to_router1!=0&&counter1==0 || router_to_router2!=0&&counter2==0 || router_to_router3!=0&&counter3==0));
        // if ((addr_20==0||addr_21==0||addr_22==0||addr_23==0||addr_24==0) && (flag_from_core!=0&&counter4==0 || router_to_router0!=0&&counter0==0 || router_to_router1!=0&&counter1==0 || router_to_router2!=0&&counter2==0 || router_to_router3!=0&&counter3==0)) {
            // cout << "current_router   : "   << router_ID    <<endl;
            // // cout << "row   : "   << row_1    <<endl;
            // // cout << "col   : "   << col_1    <<endl;
            // cout << "flag_from_core   : " << flag_from_core    <<endl;
            // cout << "router_to_router0: " << router_to_router0 <<endl;
            // cout << "router_to_router1: " << router_to_router1 <<endl;
            // cout << "router_to_router2: " << router_to_router2 <<endl;
            // cout << "router_to_router3: " << router_to_router3 <<endl;
            // if (flag_from_core) cout << "destination: "   << addr_24.to_uint()    <<endl;
            // if (router_to_router0) cout << "destination: "   << addr_20.to_uint()    <<endl;
            // if (router_to_router1) cout << "destination: "   << addr_21.to_uint()    <<endl;
            // if (router_to_router2) cout << "destination: "   << addr_22.to_uint()    <<endl;
            // if (router_to_router3) cout << "destination: "   << addr_23.to_uint()    <<endl;
            // cout << "Current simulation time: " << sc_time_stamp() << endl<<endl;
        // }
        if (flag_from_core) {
            int row_24 = addr_24.range(3,2).to_uint();
            int col_24 = addr_24.range(1,0).to_uint();
            if (router_ID==11) {
                // cout<< "addr_24: " <<addr_24.to_uint()<<endl;
            }
            if (counter4 <lv_vector4.size()) {
                if (router_ID==11) {
                    // cout << "counter: " <<counter4 <<endl;
                    // // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                    // cout << "receive_data: " <<lv_vector4[counter4] <<endl;
                    // cout<< "tx_busy_01: " <<tx_busy_01<<endl;
                    // cout<< "tx_busy_02: " <<tx_busy_02<<endl;
                    // cout<< "tx_busy_03: " <<tx_busy_03<<endl;
                    // cout<< "tx_busy_00: " <<tx_busy_00<<endl;
                }
                if (row_1>row_24) {
                    if (tx_busy_03==false&&tx_busy_02==false&&tx_busy_01==false&&tx_busy_00==false) {
                        if (router_ID==11) {
                            // cout << "counter: " <<counter4 <<endl;
                            // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                            // cout << "receive_data: " <<lv_vector4[counter4] <<endl;
                            // cout<< "tx_busy_01: " <<tx_busy_01<<endl;
                        }
                        out_flit[0].write(lv_vector4[counter4]);
                        out_req[0].write(true);
                        counter4 = counter4+1;
                        // if (flag) cout << "direction_0: " << endl;
                        tx_busy_04 = true;
                    }
                }
                else if (row_1<row_24) {
                    if (tx_busy_13==false&&tx_busy_12==false&&tx_busy_11==false&&tx_busy_10==false){
                        out_flit[1].write(lv_vector4[counter4]);
                        out_req[1].write(true);
                        counter4 = counter4+1;
                        // if (flag) cout << "direction_1: "<<endl;
                        tx_busy_14 = true;
                    }
                }
                else if (col_1>col_24) {
                    if (tx_busy_23==false&&tx_busy_22==false&&tx_busy_21==false&&tx_busy_20==false) {
                        out_flit[2].write(lv_vector4[counter4]);
                        out_req[2].write(true);
                        counter4 = counter4+1;
                        // if (flag) cout << "direction_2: "<<endl;
                        tx_busy_24 = true;
                    }
                }
                else if (col_1<col_24) {
                    if (tx_busy_33==false&&tx_busy_32==false&&tx_busy_31==false&&tx_busy_30==false) {
                        out_flit[3].write(lv_vector4[counter4]);
                        out_req[3].write(true);
                        counter4 = counter4+1;
                        // if (flag) cout << "direction_3: "<<endl;
                        tx_busy_34 = true;
                    }
                }
            } else if (counter4 == lv_vector4.size()) {
                counter4 = 0;
                if (row_1>row_24) {
                    out_req[0].write(false);
                    tx_busy_04 = false;
                }
                else if (row_1<row_24) {
                    out_req[1].write(false);
                    tx_busy_14 = false;
                }
                else if (col_1>col_24) {
                    out_req[2].write(false);
                    tx_busy_24 = false;
                }
                else if (col_1<col_24) {
                    out_req[3].write(false);
                    tx_busy_34 = false;
                }
                flag_from_core = 0;
                lv_vector4.clear();
            }
        }
        if (router_to_router3) {
            int row_23 = addr_23[send_num3].range(3,2).to_uint();
            int col_23 = addr_23[send_num3].range(1,0).to_uint();
            if (counter3+index3 <data_num3[send_num3]) {
                if (router_ID==11) {
                    // cout << "counter: " <<counter4 <<endl;
                    // // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                    // cout << "receive_data: " <<lv_vector4[counter4] <<endl;
                    // cout<< "tx_busy_01: " <<tx_busy_01<<endl;
                    // cout<< "tx_busy_02: " <<tx_busy_02<<endl;
                    // cout<< "tx_busy_03: " <<tx_busy_03<<endl;
                    // cout<< "tx_busy_00: " <<tx_busy_00<<endl;
                }
                if (row_1>row_23) {
                    if (tx_busy_04==false&&tx_busy_02==false&&tx_busy_01==false&&tx_busy_00==false) {
                        out_flit[0].write(lv_vector3[counter3+index3]);
                        out_req[0].write(true);
                        // if (flag) cout << "direction_0: " << endl;
                        counter3 = counter3+1;
                        tx_busy_03 = true;
                    }
                }
                else if (row_1<row_23) {
                    if (tx_busy_14==false&&tx_busy_12==false&&tx_busy_11==false&&tx_busy_10==false){
                        out_flit[1].write(lv_vector3[counter3+index3]);
                        out_req[1].write(true);
                        // if (flag) cout << "direction_1: "<<endl;
                        counter3 = counter3+1;
                        tx_busy_13 = true;
                    }
                }
                else if (col_1>col_23) {
                    if (tx_busy_24==false&&tx_busy_22==false&&tx_busy_21==false&&tx_busy_20==false){
                        out_flit[2].write(lv_vector3[counter3+index3]);
                        out_req[2].write(true);
                        // if (flag) cout << "direction_2: "<<endl;
                        counter3 = counter3+1;
                        tx_busy_23 = true;
                    }
                }
                else if (col_1<col_23) {
                    if (tx_busy_34==false&&tx_busy_32==false&&tx_busy_31==false&&tx_busy_30==false) {
                        out_flit[3].write(lv_vector3[counter3+index3]);
                        out_req[3].write(true);
                        // if (flag) cout << "direction_3: "<<endl;
                        counter3 = counter3+1;
                        tx_busy_33 = true;
                    }
                }
                else if (col_1==col_23&&row_1==row_23) {
                    out_flit[4].write(lv_vector3[counter3+index3]);
                    out_req[4].write(true);
                    // counter3 = 0;
                    // router_to_router3 = 0;
                    if (addr_23[send_num3]==0) {
                        // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                        // cout << "counter: " <<counter3 <<endl;
                        // cout << "receive_data: " <<lv_vector3[counter3] <<endl;
                        // cout << "lv_vector3_size: "<< lv_vector3.size() <<endl;
                        // cout << "index3: "<< index3 <<endl;
                        // cout << "data_num3[send_num3]: "<< data_num3[send_num3] <<endl;
                    }
                    counter3 = counter3+1;
                }
            } else if (counter3+index3 == data_num3[send_num3]) {
                index3 = counter3+index3;
                counter3 = 0;
                send_num3 = send_num3+1;
                out_req[4].write(false);
                if (row_1>row_23) {
                    out_req[0].write(false);
                    tx_busy_03 = false;
                }
                else if (row_1<row_23) {
                    out_req[1].write(false);
                    tx_busy_13 = false;
                }
                else if (col_1>col_23) {
                    out_req[2].write(false);
                    tx_busy_23 = false;
                }
                else if (col_1<col_23) {
                    out_req[3].write(false);
                    tx_busy_33 = false;
                }
                else if (col_1==col_23&&row_1==row_23) {
                    out_req[4].write(false);
                }
                router_to_router3 = 0;
                // lv_vector3.clear();
            }
        }
        if (router_to_router2) {
            int row_22 = addr_22[send_num2].range(3,2).to_uint();
            int col_22 = addr_22[send_num2].range(1,0).to_uint();
            if (counter2+index2 <data_num2[send_num2]) {
                // if (router_ID==10) {
                //     cout << "counter: " <<counter2 <<endl;
                //     // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                //     cout << "receive_data: " <<lv_vector2[counter2+index2] <<endl;
                //     cout<< "tx_busy_31: " <<tx_busy_31<<endl;
                //     cout<< "tx_busy_34: " <<tx_busy_34<<endl;
                //     cout<< "tx_busy_33: " <<tx_busy_33<<endl;
                //     cout<< "tx_busy_30: " <<tx_busy_30<<endl;
                //     cout<< "index2: " <<index2<<endl;
                //     cout<< "data_num2[send_num2]: "<<data_num2[send_num2]<<endl;
                //     cout<< "addr_22: "<<addr_22<<endl;
                // }
                if (row_1>row_22) {
                    if (tx_busy_04==false&&tx_busy_03==false&&tx_busy_01==false&&tx_busy_00==false){
                        out_flit[0].write(lv_vector2[counter2+index2]);
                        out_req[0].write(true);
                        // if (flag) cout << "direction_0: " << endl;
                        counter2 = counter2+1;
                        tx_busy_02 = true;
                    }
                }
                else if (row_1<row_22 ) {

                    if (tx_busy_14==false&&tx_busy_13==false&&tx_busy_11==false&&tx_busy_10==false) { 
                        out_flit[1].write(lv_vector2[counter2+index2]);
                        out_req[1].write(true);
                        // if (flag) cout << "direction_1: "<<endl;
                        counter2 = counter2+1;
                        tx_busy_12 = true;
                    }
                }
                else if (col_1>col_22) {
                    if (tx_busy_24==false&&tx_busy_23==false&&tx_busy_21==false&&tx_busy_20==false) {
                        out_flit[2].write(lv_vector2[counter2+index2]);
                        out_req[2].write(true);
                        // if (flag) cout << "direction_2: "<<endl;
                        counter2 = counter2+1;
                        tx_busy_22 = true;
                    }
                }
                else if (col_1<col_22) {
                    if (tx_busy_34==false&&tx_busy_33==false&&tx_busy_31==false&&tx_busy_30==false) {
                        if (router_ID==10) {
                            // cout << "counter: " <<counter2 <<endl;
                            // // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                            // cout << "receive_data: " <<lv_vector2[counter2+index2] <<endl;
                            // cout<< "tx_busy_31: " <<tx_busy_31<<endl;
                            // cout<< "tx_busy_34: " <<tx_busy_34<<endl;
                            // cout<< "tx_busy_33: " <<tx_busy_33<<endl;
                            // cout<< "tx_busy_30: " <<tx_busy_30<<endl;
                            // cout<< "index2: " <<index2<<endl;
                            // cout<< "data_num2[send_num2]: "<<data_num2[send_num2]<<endl;
                            // cout<< "addr_22: "<<addr_22[send_num2]<<endl;
                        }
                        out_flit[3].write(lv_vector2[counter2+index2]);
                        out_req[3].write(true);
                        // if (flag) cout << "direction_3: "<<endl;
                        counter2 = counter2+1;
                        tx_busy_32 = true;
                    }
                }
                else if (col_1==col_22&&row_1==row_22) {
                    out_flit[4].write(lv_vector2[counter2+index2]);
                    out_req[4].write(true);
                    // counter2 = 0;
                    // router_to_router2 = 0;
                    if (router_ID==4) {
                        // cout << "counter: " <<counter2 <<endl;
                        // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                        // cout << "receive_data: " <<lv_vector2[counter2+index2] <<endl;
                    }
                    counter2 = counter2+1;
                }
                // if (router_ID==10) cout<< "addr_22: "<<addr_22[send_num2]<<endl;
            } else if (counter2+index2 == data_num2[send_num2]) {
                index2 = counter2+index2;
                counter2 = 0;
                // if (addr_22==10&&router_ID==10) cout << "data_num2 "<< data_num2[send_num2]<< endl;
                // if (addr_22==10&&router_ID==10) cout << "send_num2 "<< send_num2<< endl;
                send_num2 = send_num2+1;
                // if (addr_22==10) cout << "data size "<< lv_vector2.size()<< endl;
                if (row_1>row_22) {
                    out_req[0].write(false);
                    tx_busy_02 = false;
                }
                else if (row_1<row_22) {
                    out_req[1].write(false);
                    tx_busy_12 = false;
                }
                else if (col_1>col_22) {
                    out_req[2].write(false);
                    tx_busy_22 = false;
                }
                else if (col_1<col_22) {
                    out_req[3].write(false);
                    tx_busy_32 = false;
                }
                else if (col_1==col_22&&row_1==row_22) {
                    out_req[4].write(false);
                }
                router_to_router2 = 0;
                // lv_vector2.clear();
            }
        }
        if (router_to_router1) {
            int row_21 = addr_21[send_num1].range(3,2).to_uint();
            int col_21 = addr_21[send_num1].range(1,0).to_uint();
            if (counter1+index1 <data_num1[send_num1]) {
                if (router_ID==8) {
                    // cout << "counter: " <<counter1 <<endl;
                    // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                    // cout << "receive_data: " <<lv_vector1[counter1] <<endl;
                    // cout<< "tx_busy_34: " <<tx_busy_01<<endl;
                    // cout<< "tx_busy_32: " <<tx_busy_02<<endl;
                    // cout<< "tx_busy_33: " <<tx_busy_03<<endl;
                    // cout<< "tx_busy_30: " <<tx_busy_00<<endl;
                }
                if (row_1>row_21) {
                    if (tx_busy_04==false&&tx_busy_03==false&&tx_busy_02==false&&tx_busy_00==false) {
                        // if (router_ID==7) {
                        //     cout << "counter: " <<counter1 <<endl;
                        //     cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                        //     cout << "receive_data: " <<lv_vector1[counter1] <<endl;
                        // }
                        out_flit[0].write(lv_vector1[counter1+index1]);
                        out_req[0].write(true);
                        // if (flag) cout << "direction_0: " <<endl;
                        tx_busy_01 = true;
                        counter1 = counter1+1;
                    }
                }
                else if (row_1<row_21) {
                    if  (tx_busy_14==false&&tx_busy_13==false&&tx_busy_12==false&&tx_busy_10==false){
                        out_flit[1].write(lv_vector1[counter1+index1]);
                        out_req[1].write(true);
                        // if (flag) cout << "direction_1: "<<endl;
                        tx_busy_11 = true;
                        counter1 = counter1+1;
                    }
                }
                else if (col_1>col_21) {
                    if (tx_busy_24==false&&tx_busy_23==false&&tx_busy_22==false&&tx_busy_20==false) {
                        out_flit[2].write(lv_vector1[counter1+index1]);
                        out_req[2].write(true);
                        // if (flag) cout << "direction_2: "<<endl;
                        tx_busy_21 = true;
                        counter1 = counter1+1;
                    }
                }
                else if (col_1<col_21) {
                    if (tx_busy_34==false&&tx_busy_33==false&&tx_busy_32==false&&tx_busy_30==false) {
                        out_flit[3].write(lv_vector1[counter1+index1]);
                        out_req[3].write(true);
                        // if (flag) cout << "direction_3: "<<endl;
                        tx_busy_31 = true;
                        counter1 = counter1+1;
                    }
                }
                else if (col_1==col_21&&row_1==row_21) {
                    out_flit[4].write(lv_vector1[counter1+index1]);
                    out_req[4].write(true);
                    // counter1 = 0;
                    // router_to_router1 = 0;
                    if (addr_21[send_num1]==7) {
                        // cout << "counter: " <<counter1 <<endl;
                        // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                        // cout << "receive_data: " <<lv_vector1[counter1] <<endl;
                    }
                    counter1 = counter1+1;
                }
                
            } else if (counter1+index1 == data_num1[send_num1]) {
                index1 = counter1+index1;
                counter1 = 0;
                send_num1 = send_num1+1;
                if (row_1>row_21) {
                    out_req[0].write(false);
                    tx_busy_01 = false;
                }
                else if (row_1<row_21) {
                    out_req[1].write(false);
                    tx_busy_11 = false;
                }
                else if (col_1>col_21) {
                    out_req[2].write(false);
                    tx_busy_21 = false;
                }
                else if (col_1<col_21) {
                    out_req[3].write(false);
                    tx_busy_31 = false;
                }
                else if (col_1==col_21&&row_1==row_21) {
                    out_req[4].write(false);
                }
                router_to_router1 = 0;
            }
        }
        if (router_to_router0) {
            int row_20 = addr_20[send_num0].range(3,2).to_uint();
            int col_20 = addr_20[send_num0].range(1,0).to_uint();
            // if (addr_14==8) cout << col_2 << endl;
            if (counter0+index0 <data_num0[send_num0]) {
                // if (router_ID==11 && addr_20==0) {
                //     cout << "counter: " <<counter1 <<endl;
                //     // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                //     cout << "receive_data: " <<lv_vector0[counter0+index0] <<endl;
                // }
                if (row_1>row_20) {
                    if (tx_busy_04==false&&tx_busy_03==false&&tx_busy_02==false&&tx_busy_01==false) {
                        out_flit[0].write(lv_vector0[counter0+index0]);
                        out_req[0].write(true);
                        // if (flag) cout << "direction_0: "<<endl;
                        counter0 = counter0+1;
                        tx_busy_00=true;
                    }
                }
                else if (row_1<row_20) {
                    if (tx_busy_14==false&&tx_busy_13==false&&tx_busy_12==false&&tx_busy_11==false){
                        out_flit[1].write(lv_vector0[counter0+index0]);
                        out_req[1].write(true);
                        // if (flag) cout << "direction_1: "<<endl;
                        counter0 = counter0+1;
                        tx_busy_10=true;
                    }
                }
                else if (col_1>col_20) {
                    if  (tx_busy_24==false&&tx_busy_23==false&&tx_busy_22==false&&tx_busy_21==false) {
                        out_flit[2].write(lv_vector0[counter0+index0]);
                        out_req[2].write(true);
                        // if (flag) cout << "direction_2: "<<endl;
                        counter0 = counter0+1;
                        tx_busy_20=true;
                    }
                }
                else if (col_1<col_20 ) {
                    if (tx_busy_34==false&&tx_busy_33==false&&tx_busy_32==false&&tx_busy_31==false) {
                        out_flit[3].write(lv_vector0[counter0+index0]);
                        out_req[3].write(true);
                        // if (flag) cout << "direction_3: "<<endl;
                        counter0 = counter0+1;
                        tx_busy_30=true;
                    }
                }
                else if (col_1==col_20&&row_1==row_20) {
                    out_flit[4].write(lv_vector0[counter0+index0]);
                    out_req[4].write(true);
                    // counter0 = 0;
                    // router_to_router0 = 0;
                    // if (addr_20[send_num0]==14) {
                    //     cout << "counter: " <<counter0 <<endl;
                    // // cout << "congratulation!!" << "router "<<router_ID<<" arrive" <<endl;
                    //     cout << "receive_data: " <<lv_vector0[counter0+index0] <<endl;
                    // }
                    counter0 = counter0+1;
                }
            } else if (counter0+index0 == data_num0[send_num0]) {
                index0 = counter0+index0;
                counter0 = 0;
                send_num0 = send_num0+1;
                if (row_1>row_20) {
                    out_req[0].write(false);
                    tx_busy_00=false;
                }
                else if (row_1<row_20) {
                    out_req[1].write(false);
                    tx_busy_10=false;
                }
                else if (col_1>col_20) {
                    out_req[2].write(false);
                    tx_busy_20=false;
                }
                else if (col_1<col_20) {
                    out_req[3].write(false);
                    tx_busy_30=false;
                }
                else if (col_1==col_20&&row_1==row_20) {
                    out_req[4].write(false);
                }
                router_to_router0 = 0;
            }
        }
    }
    sc_lv<34> receive_data[5];
    sc_lv<2> first_two_bits[5];
    void data_from_rounter() {
        if (in_req[1].read() == true){
            receive_data[1] = in_flit[1].read();
            first_two_bits[1] = receive_data[1].range(33, 32);

            if (first_two_bits[1] == 2) {
                store_begin1 = 1;
                addr_11 = receive_data[1].range(31, 28);
                addr_21.push_back(receive_data[1].range(27, 24));
                // addr_21 = receive_data[1].range(27, 24);
                lv_vector1.push_back(receive_data[1]);
                if (router_ID==4) {
                    cout << "Received Data: " << receive_data[1] << endl;
                }
            } else if (first_two_bits[1] == 1 && store_begin1 == 1) {
                store_begin1 = 0;
                lv_vector1.push_back(receive_data[1]);
                data_num1.push_back(lv_vector1.size());
                if (router_ID==4){
                    cout << "Received Data: " << receive_data[1] << endl;
                    cout << lv_vector1.size() <<endl;
                    cout << data_num1.size() <<endl;
                    cout << "addr_1 : " << addr_11 << endl;
                }
                router_to_router1 = 1;

            } else if (store_begin1==1){
                lv_vector1.push_back(receive_data[1]);
                if (router_ID==4){
                    cout << "Received Data: " << receive_data[1] << endl;
                }
                
            }
        }
        if (in_req[0].read() == true){
            // for (size_t i = 0; i < 70; ++i) {
                // out_ack[4].write(true);  // No data or not acknowledged, do not transmit
                receive_data[0] = in_flit[0].read();
                first_two_bits[0] = receive_data[0].range(33, 32);

                if (first_two_bits[0] == 2) {
                    store_begin0 = 1;
                    addr_10 = receive_data[0].range(31, 28);
                    // addr_20 = receive_data[0].range(27, 24);
                    addr_20.push_back( receive_data[0].range(27, 24));
                    lv_vector0.push_back(receive_data[0]);
                    // if (router_ID==14){
                    //     cout << "Received Data: " << receive_data[0] << endl;
                    // }
                } else if (first_two_bits[0] == 1 && store_begin0 == 1) {

                    store_begin0 = 0;
                    lv_vector0.push_back(receive_data[0]);
                    data_num0.push_back(lv_vector0.size());
                    // if (router_ID==14){
                    //     cout << "Received Data: " << receive_data[0] << endl;
                    //     cout << lv_vector0.size() <<endl;
                    //     // cout << "addr_1 : " << addr_10 << endl;
                    //     cout << "addr_2 : " << addr_20[send_num0] << endl;
                    // }
                    router_to_router0 = 1;
                } else if (store_begin0==1){
                    lv_vector0.push_back(receive_data[0]);
                    // if (router_ID==14){
                    //     cout << "Received Data: " << receive_data[0] << endl;
                    // }
                    
                }
        } 
        if (in_req[2].read() == true){
            // for (size_t i = 0; i < 70; ++i) {
                // out_ack[4].write(true);  // No data or not acknowledged, do not transmit
                receive_data[2] = in_flit[2].read();
                first_two_bits[2] = receive_data[2].range(33, 32);

                if (first_two_bits[2] == 2) {
                    store_begin2 = 1;
                    addr_12 = receive_data[2].range(31, 28);
                    // addr_22 = receive_data[2].range(27, 24);
                    lv_vector2.push_back(receive_data[2]);
                    addr_22.push_back(receive_data[2].range(27, 24));
                    if (router_ID==11){
                        // cout << "Received Data: " << receive_data[2] << endl;
                    }
                } else if (first_two_bits[2] == 1 && store_begin2 == 1) {
                    store_begin2 = 0;
                    lv_vector2.push_back(receive_data[2]);
                    data_num2.push_back(lv_vector2.size());
                    if (router_ID==11){
                        // cout << "Received Data: " << receive_data[2] << endl;
                        // cout << data_num2.back() <<endl;
                        // cout << "addr_1 : " << addr_12 << endl;
                    }
                    router_to_router2 = 1;
                } else if (store_begin2==1){
                    lv_vector2.push_back(receive_data[2]);
                    if (router_ID==11){
                        // cout << "Received Data: " << receive_data[2] << endl;
                    }
                    
                }
        } 
        if (in_req[3].read() == true){
            // for (size_t i = 0; i < 70; ++i) {
                // out_ack[4].write(true);  // No data or not acknowledged, do not transmit
                receive_data[3] = in_flit[3].read();
                first_two_bits[3] = receive_data[3].range(33, 32);

                if (first_two_bits[3] == 2) {
                    store_begin3 = 1;
                    addr_13 = receive_data[3].range(31, 28);
                    // addr_23 = receive_data[3].range(27, 24);
                    addr_23.push_back(receive_data[3].range(27, 24));
                    lv_vector3.push_back(receive_data[3]);
                    // if (addr_23==10){
                        // cout << "Received Data: " << receive_data[3] << endl;
                    // }
                } else if (first_two_bits[3] == 1 && store_begin3 == 1) {
                    store_begin3 = 0;
                    lv_vector3.push_back(receive_data[3]);
                    data_num3.push_back(lv_vector3.size());
                    // if (addr_23==10){
                        // cout << "Received Data: " << receive_data[3] << endl;
                        // cout << lv_vector3.size() <<endl;
                        // cout << "addr_1 : " << addr_13 << endl;
                    // }
                    router_to_router3 = 1;
                } else if (store_begin3==1){
                    lv_vector3.push_back(receive_data[3]);
                    // if (addr_23==10){
                        // cout << "Received Data: " << receive_data[3] << endl;
                    // }
                    
                }
        } 
    }


};

#endif