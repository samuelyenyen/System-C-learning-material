#include "systemc.h"
#include "clockreset.h"
#include "core.h"
#include "router.h"
#define GRID_SIZE 4




int sc_main(int argc, char* argv[])
{   
    Router* routers[GRID_SIZE][GRID_SIZE];
    Core* cores[GRID_SIZE][GRID_SIZE];
    // =======================
    //   signals declaration
    // =======================
    sc_signal < bool > clk;
    sc_signal < bool > rst;
    sc_signal<sc_lv<34>> flit_signals1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals1[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals2[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_north[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_north[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_north[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_south[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_south[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_south[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_east[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_east[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_east[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_west[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_west[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_west[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_north_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_north_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_north_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<sc_lv<34>> flit_signals_north_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_north_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_north_edge2[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_south_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_south_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_south_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<sc_lv<34>> flit_signals_south_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_south_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_south_edge2[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_east_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_east_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_east_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<sc_lv<34>> flit_signals_east_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_east_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_east_edge2[GRID_SIZE][GRID_SIZE];

    sc_signal<sc_lv<34>> flit_signals_west_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_west_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_west_edge1[GRID_SIZE][GRID_SIZE];
    sc_signal<sc_lv<34>> flit_signals_west_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> req_signals_west_edge2[GRID_SIZE][GRID_SIZE];
    sc_signal<bool> ack_signals_west_edge2[GRID_SIZE][GRID_SIZE];
    // =======================
    //   modules declaration
    // =======================
    Clock m_clock("m_clock", 10);
    Reset m_reset("m_reset", 15);
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            char router_name[20];
            sprintf(router_name, "Router_%d_%d", i, j);
            routers[i][j] = new Router(router_name);

            char core_name[20];
            sprintf(core_name, "Core_%d_%d", i, j);
            cores[i][j] = new Core(core_name);
        }
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cores[i][j]->clk(clk);
            cores[i][j]->rst(rst);
            routers[i][j]->clk(clk);
            routers[i][j]->rst(rst);

            // Connect to Core
            routers[i][j]->out_flit[4](flit_signals1[i][j]);
            cores[i][j]->flit_rx(flit_signals1[i][j]);

            routers[i][j]->out_req[4](req_signals1[i][j]);
            cores[i][j]->req_rx(req_signals1[i][j]);

            cores[i][j]->ack_rx(ack_signals1[i][j]);
            routers[i][j]->in_ack[4](ack_signals1[i][j]);
            // Assuming Core also sends data back to the router
            cores[i][j]->flit_tx(flit_signals2[i][j]);
            routers[i][j]->in_flit[4](flit_signals2[i][j]);
            
            cores[i][j]->req_tx(req_signals2[i][j]);
            routers[i][j]->in_req[4](req_signals2[i][j]);

            routers[i][j]->out_ack[4](ack_signals2[i][j]);
            cores[i][j]->ack_tx(ack_signals2[i][j]);

            int north = i - 1;
            int south = i + 1;
            int west = j - 1;
            int east = j + 1;

            // Connect to North
            if (north >= 0) {
                routers[i][j]->out_flit[0](flit_signals_north[i][j]);
                routers[north][j]->in_flit[1](flit_signals_north[i][j]);
                routers[i][j]->out_req[0](req_signals_north[i][j]);
                routers[north][j]->in_req[1](req_signals_north[i][j]);
                routers[i][j]->out_ack[0](ack_signals_north[i][j]);
                routers[north][j]->in_ack[1](ack_signals_north[i][j]);
            }
            else {
                routers[i][j]->out_flit[0](flit_signals_north_edge1[i][j]);
                routers[i][j]->out_req[0](req_signals_north_edge1[i][j]);
                routers[i][j]->out_ack[0](ack_signals_north_edge1[i][j]);
                routers[i][j]->in_flit[0](flit_signals_north_edge2[i][j]);
                routers[i][j]->in_req[0](req_signals_north_edge2[i][j]);
                routers[i][j]->in_ack[0](ack_signals_north_edge2[i][j]);
            }

            // Connect to South
            if (south < GRID_SIZE) {
                routers[i][j]->out_flit[1](flit_signals_south[i][j]);
                routers[south][j]->in_flit[0](flit_signals_south[i][j]);
                routers[i][j]->out_req[1](req_signals_south[i][j]);
                routers[south][j]->in_req[0](req_signals_south[i][j]);
                routers[i][j]->out_ack[1](ack_signals_south[i][j]);
                routers[south][j]->in_ack[0](ack_signals_south[i][j]);
            }
            else {
                routers[i][j]->out_flit[1](flit_signals_south_edge1[i][j]);
                routers[i][j]->out_req[1](req_signals_south_edge1[i][j]);
                routers[i][j]->out_ack[1](ack_signals_south_edge1[i][j]);
                routers[i][j]->in_flit[1](flit_signals_south_edge2[i][j]);
                routers[i][j]->in_req[1](req_signals_south_edge2[i][j]);
                routers[i][j]->in_ack[1](ack_signals_south_edge2[i][j]);
            }
            // Connect to West
            if (west >= 0) {
                routers[i][j]->out_flit[2](flit_signals_west[i][j]);
                routers[i][west]->in_flit[3](flit_signals_west[i][j]);
                routers[i][j]->out_req[2](req_signals_west[i][j]);
                routers[i][west]->in_req[3](req_signals_west[i][j]);
                routers[i][j]->out_ack[2](ack_signals_west[i][j]);
                routers[i][west]->in_ack[3](ack_signals_west[i][j]);
            }
            else {
                routers[i][j]->out_flit[2](flit_signals_west_edge1[i][j]);
                routers[i][j]->out_req[2](req_signals_west_edge1[i][j]);
                routers[i][j]->out_ack[2](ack_signals_west_edge1[i][j]);
                routers[i][j]->in_flit[2](flit_signals_west_edge2[i][j]);
                routers[i][j]->in_req[2](req_signals_west_edge2[i][j]);
                routers[i][j]->in_ack[2](ack_signals_west_edge2[i][j]);
            }

            // Connect to East
            if (east < GRID_SIZE) {
                routers[i][j]->out_flit[3](flit_signals_east[i][j]);
                routers[i][east]->in_flit[2](flit_signals_east[i][j]);
                routers[i][j]->out_req[3](req_signals_east[i][j]);
                routers[i][east]->in_req[2](req_signals_east[i][j]);
                routers[i][j]->out_ack[3](ack_signals_east[i][j]);
                routers[i][east]->in_ack[2](ack_signals_east[i][j]);
            }
            else {
                routers[i][j]->out_flit[3](flit_signals_east_edge1[i][j]);
                routers[i][j]->out_req[3](req_signals_east_edge1[i][j]);
                routers[i][j]->out_ack[3](ack_signals_east_edge1[i][j]);
                routers[i][j]->in_flit[3](flit_signals_east_edge2[i][j]);
                routers[i][j]->in_req[3](req_signals_east_edge2[i][j]);
                routers[i][j]->in_ack[3](ack_signals_east_edge2[i][j]);
            }
        }
    }

    // vector<vector<Packet*>> packets(4,vector<Packet*>(4,nullptr));
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cores[i][j]->pe.init(i*GRID_SIZE+j);  // Initialize PE with an ID or other parameters
            routers[i][j]->set_router_ID(i*GRID_SIZE+j);
            cores[i][j]->trigger_event.notify(SC_ZERO_TIME); // 觸發事件，執行一次方法
            // packets[i][j] = cores[i][j]->pe.get_packet();  // Retrieve and store the packet pointer
            // packets[i][j] = cores[i][j]->pe.get_packet();  // Retrieve and store the packet pointer
            // packets[i][j] = cores[i][j]->pe.get_packet();  // Retrieve and store the packet pointer
        }
    }
    // =======================
    //   modules connection
    // =======================
    m_clock( clk );
    m_reset( rst );
    sc_start( 7000, SC_NS );
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cores[i][j]->trigger_event.notify(SC_ZERO_TIME); // 觸發事件，執行一次方法
            // packets[i][j] = cores[i][j]->pe.get_packet();  // Retrieve and store the packet pointer
            // packets[i][j] = cores[i][j]->pe.get_packet();  // Retrieve and store the packet pointer
            // packets[i][j] = cores[i][j]->pe.get_packet();  // Retrieve and store the packet pointer
        }
    }
    



    // sc_trace_file *tf = sc_create_vcd_trace_file("timer");
    // sc_trace(tf, clk, "clk");
    // sc_trace(tf, rst, "rst");

    
    sc_start( 50000, SC_NS );
    // sc_close_vcd_trace_file(tf);
    return 0;
}