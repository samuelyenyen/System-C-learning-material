#ifndef _HPG_router_5_proxy_h_H_
#define _HPG_router_5_proxy_h_H_

#include "systemc.h"

SC_HDL_MODULE( router_5) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<sc_lv<34> > out_flit_0;
    sc_out<sc_lv<34> > out_flit_1;
    sc_out<sc_lv<34> > out_flit_2;
    sc_out<sc_lv<34> > out_flit_3;
    sc_out<sc_lv<34> > out_flit_4;
    sc_out<bool> out_req_0;
    sc_out<bool> out_req_1;
    sc_out<bool> out_req_2;
    sc_out<bool> out_req_3;
    sc_out<bool> out_req_4;
    sc_in<bool> in_ack_0;
    sc_in<bool> in_ack_1;
    sc_in<bool> in_ack_2;
    sc_in<bool> in_ack_3;
    sc_in<bool> in_ack_4;
    sc_in<sc_lv<34> > in_flit_0;
    sc_in<sc_lv<34> > in_flit_1;
    sc_in<sc_lv<34> > in_flit_2;
    sc_in<sc_lv<34> > in_flit_3;
    sc_in<sc_lv<34> > in_flit_4;
    sc_in<bool> in_req_0;
    sc_in<bool> in_req_1;
    sc_in<bool> in_req_2;
    sc_in<bool> in_req_3;
    sc_in<bool> in_req_4;
    sc_out<bool> out_ack_0;
    sc_out<bool> out_ack_1;
    sc_out<bool> out_ack_2;
    sc_out<bool> out_ack_3;
    sc_out<bool> out_ack_4;

    std::string hpg_log_lib;
    std::string hpg_module_name;
    std::string hpg_hdl_src_path;

    std::string libraryName() { return hpg_log_lib; }

    std::string moduleName() { return hpg_module_name; }

    cwr_hdlLangType hdl_language_type() { return sc_hdl_module::cwr_verilog; }

    void getVerilogSourceFiles(std::vector<std::string>& verilog_files) {
        verilog_files.push_back(hpg_hdl_src_path + std::string("/RAID2/COURSE/mlchip/mlchip081/hw4_s/router_0.v"));
        verilog_files.push_back(hpg_hdl_src_path + std::string("/RAID2/COURSE/mlchip/mlchip081/hw4_s/router_1.v"));
        verilog_files.push_back(hpg_hdl_src_path + std::string("/RAID2/COURSE/mlchip/mlchip081/hw4_s/router_4.v"));
        verilog_files.push_back(hpg_hdl_src_path + std::string("/RAID2/COURSE/mlchip/mlchip081/hw4_s/router_5.v"));
    }

    router_5(sc_module_name name, const char* hdlSrcPath="") : 
        sc_hdl_module(name), hpg_log_lib("CWR_HDL_WORK"), hpg_module_name("router_5"), hpg_hdl_src_path()
        , clk("clk"), rst("rst"), out_flit_0("out_flit_0"), out_flit_1("out_flit_1")
        , out_flit_2("out_flit_2"), out_flit_3("out_flit_3"), out_flit_4("out_flit_4")
        , out_req_0("out_req_0"), out_req_1("out_req_1"), out_req_2("out_req_2")
        , out_req_3("out_req_3"), out_req_4("out_req_4"), in_ack_0("in_ack_0"), in_ack_1("in_ack_1")
        , in_ack_2("in_ack_2"), in_ack_3("in_ack_3"), in_ack_4("in_ack_4"), in_flit_0("in_flit_0")
        , in_flit_1("in_flit_1"), in_flit_2("in_flit_2"), in_flit_3("in_flit_3")
        , in_flit_4("in_flit_4"), in_req_0("in_req_0"), in_req_1("in_req_1"), in_req_2("in_req_2")
        , in_req_3("in_req_3"), in_req_4("in_req_4"), out_ack_0("out_ack_0"), out_ack_1("out_ack_1")
        , out_ack_2("out_ack_2"), out_ack_3("out_ack_3"), out_ack_4("out_ack_4") {

        if (hdlSrcPath != 0 && strlen(hdlSrcPath) != 0) {
          hpg_hdl_src_path = std::string(hdlSrcPath) + "/";
        }


    }
};

#endif
