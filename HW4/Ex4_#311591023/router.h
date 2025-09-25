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

    
    int router_ID;
    bool first_turn = true;

    void set_router_ID(int new_ID) {
        router_ID = new_ID;
    }

    SC_CTOR(Router) {

        SC_METHOD(run);
        sensitive << clk.pos() << rst.neg(); 
        dont_initialize();
    }

    void run() {
        out_req[0].write(true);
        out_req[1].write(true);
        out_req[2].write(true);
        out_req[3].write(true);
        out_req[4].write(true);
        out_ack[0].write(true);
        out_ack[1].write(true);
        out_ack[2].write(true);
        out_ack[3].write(true);
        out_ack[4].write(true);
        sc_lv<34>  receive_data = in_flit[0].read();
        sc_lv<2> first_two_bits = receive_data.range(33, 32);
        if (first_two_bits==3 && first_turn == true) {
            first_turn = false;
            cout << "It's second RUN" <<endl;
        }
        if (router_ID==0) {
            if (first_turn) {
                out_flit[4].write(in_flit[0].read());
            } else {
                out_flit[4].write(in_flit[1].read()); 
            }
            out_flit[3].write(in_flit[4].read());
            out_flit[0].write(in_flit[4].read());
        }   
        if (router_ID==1) {
            out_flit[4].write(in_flit[2].read());
            out_flit[1].write(in_flit[4].read()); 
        } 
        if (router_ID==5) {
            out_flit[4].write(in_flit[0].read());
            out_flit[2].write(in_flit[4].read()); 
        } 
        if (router_ID==4) {
            out_flit[4].write(in_flit[3].read());
            out_flit[0].write(in_flit[4].read()); 
        } 
    }
};

#endif