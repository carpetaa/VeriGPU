// initially will be similar to src/comp_driver.sv, but we will extend this.

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <verilated.h>
#include <vector>
#include <bitset>
#include <iomanip>
#include <verilated_vcd_c.h>
#include "core_and_mem.h"

#define MAX_SIM_TIME 5000000
// #define MAX_SIM_TIME 2000
vluint64_t sim_time = 0;

double sc_time_stamp() {
    return sim_time;
}

const int kernel_offset = 128;

int main(int argc, char **argv, char **env)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    core_and_mem *dut = new core_and_mem;

    // Verilated::traceEverOn(true);
    // VerilatedVcdC *m_trace = new VerilatedVcdC;
    // dut->trace(m_trace, 5);
    // m_trace->open("waveform.vcd");
    std::cout << "verilator_driver" << std::endl;

    dut->rst = 0;
    // dut->oob_wen = 0;
    dut->contr_core1_ena = 0;
    dut->contr_core1_clr = 0;
    // dut->ena = 0;
    dut->clk = 0;
    sim_time += 5;
    dut->eval();

    dut->clk = 1;
    sim_time += 5;
    dut->eval();

    dut->clk = 0;
    sim_time += 5;
    dut->eval();

    dut->clk = 1;
    sim_time += 5;
    dut->eval();

    dut->rst = 1;
    dut->contr_core1_set_pc_req = 1;
    dut->contr_core1_set_pc_addr = 128;
    dut->clk = 0;
    sim_time += 5;
    dut->eval();

    dut->clk = 1;
    sim_time += 5;
    dut->eval();

    dut->contr_core1_set_pc_req = 0;

    std::fstream infile;
    // infile.exceptions(std::ios::failbit | std::ios::badbit);
    std::string hex_path = "../../../../build/prog.hex";
    infile.open(hex_path, std::fstream::in);
    if(!infile) {
        std::cout << "" << std::endl;
        std::cout << "ERROR: failed to open file " << hex_path << std::endl;
        return -1;
    }
    // infile.exceptions(std::ios::goodbit);
    unsigned int a;
    unsigned int addr = kernel_offset;
    // std::cout << "iterating file" << std::endl;
    while(infile >> std::hex >> a) {
        // dut->oob_wen = 1;
        // dut->oob_wr_addr = addr;
        // dut->oob_wr_data = a;
        // std::cout << "writing to global memory pos=" << addr << " v=" << a << std::endl;
        dut->contr_mem_wr_en = 1;
        dut->contr_mem_wr_addr = addr;
        dut->contr_mem_wr_data = a;

        addr += 4;
        dut->clk = 0;
        sim_time += 5;
        dut->eval();
        dut->clk = 1;
        sim_time += 5;
        dut->eval();
    }
    // std::cout << "finished iterating file" << std::endl;

    dut->contr_mem_wr_en = 0;
    // dut->oob_wen = 0;
    dut->contr_core1_ena = 1;
    dut->clk = 0;
    sim_time += 5;
    dut->eval();
    dut->clk = 1;
    sim_time += 5;
    dut->eval();

    std::vector<unsigned int> outs;
    std::vector<int> out_types;
    std::vector<float> out_floats;

    while (sim_time < MAX_SIM_TIME)
    {
        dut->clk = 0;
        sim_time += 5;
        dut->eval();
        dut->clk = 1;
        sim_time += 5;
        dut->eval();
        if(int(dut->contr_core1_halt)) {
            // std::cout << "HALT" << std::endl;
            break;
        }
        if(int(dut->outen)) {
            out_types.push_back(0);
            // std::cout << "OUT " << int(dut->out) << std::endl;
            outs.push_back(int(dut->out));
        }
        if(int(dut->outflen)) {
            out_types.push_back(1);
            // std::cout << "OUT.S " << reinterpret_cast<float &>(dut->out) << std::endl;
            out_floats.push_back(reinterpret_cast<float &>(dut->out));
        }
        // m_trace->dump(sim_time);
    }

    unsigned i = 0;
    int int_i = 0;
    int float_i = 0;
    for(auto it = begin(out_types); it != end(out_types); ++it) {
        // std::cout << i << " " << *it << std::endl;
        if(*it) {
            std::bitset<32> bits(reinterpret_cast<int &>(out_floats[float_i]));
            std::cout << "out.s " << i << " " << bits << " " << std::fixed << std::setprecision(6) << out_floats[float_i] << std::endl;
            float_i++;
        } else {
            std::bitset<32> bits(outs[int_i]);
            std::cout << "out " << i << " " << bits << " " << std::hex << std::setw(8) << std::setfill('0') << outs[int_i] << " " << std::dec << outs[int_i] << std::endl;
            int_i++;
        }
        i += 1;
    }

    std::string test_name = "";
    std::string argv_str = "";
    //parse char argv** with for loop
    for (int i = 0; i < argc; i++)
    {
        argv_str=argv[i];
        // check if the start of argv_str is +TEST_NAME=
        if (argv_str.substr(0,11)=="+TEST_NAME=")
        {
            //if it is, then set test_name to the rest of the string
            test_name = argv_str.substr(11);
        }
    }

    //format test_name to a coverage path
    std::string coverage_data_path = "sim.out/cov_logs/" + test_name + "_cov.dat";
    Verilated::threadContextp()->coveragep()->write(coverage_data_path.c_str());
    // m_trace->close();
    delete dut;
    exit(EXIT_SUCCESS);
}
