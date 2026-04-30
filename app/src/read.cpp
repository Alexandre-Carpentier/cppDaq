/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     12/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "read.h"
#include "dbgprint.h"
#include <print>
#include <vector>
#include <string>
#include <assert.h>
#include <variant>

#ifdef WIN32
#include "Windows.h"
#endif

#ifndef __aarch64__
// Sim mock
[[nodiscard]] double cAnalogReadSim::read(TaskHandle m_analog_task_handle)
{
#ifdef _WIN32
    std::string msg = std::format("cAnalogReadSim called\n");
    MessageBox(0, msg.c_str(), 0, S_OK);
#endif

    std::print("Analog read\n");
    return 12.0;
}

// Real impl
[[nodiscard]] double cAnalogRead::read(TaskHandle m_analog_task_handle)
{
    assert(m_analog_task_handle);

    double value = { 0 };
    size_t sample_number =1;
    float timeout_s = 2.0;
    int32 read_nb = 0;

    int32 daq_ret = DAQmxReadAnalogF64(m_analog_task_handle, sample_number, timeout_s, DAQmx_Val_GroupByChannel, &value, sample_number, &read_nb, NULL);
    if ((0 != daq_ret) || (read_nb != sample_number))
    {
        // -200088  	Task specified is invalid or does not exist. 
        std::println("[!] DAQmxReadAnalogF64 Failed");
        return NAN;
    }
    return value;
}
#endif

// The reading behaviours can be configured statically using the std::vector<reader> factory_readers() factory inside daq_read_io.h
// To extend just add new behaviours in the vector
// 
// If needed can be easilly rewrited with variadic template to use with various arguments
// 
// vector factory
[[nodiscard]]std::vector<reader> factory_readers(reader_type choice)
{
    std::vector<reader> rds;
    if (choice == reader_type::sim)
    {
        rds.push_back(cAnalogReadSim());
    }
    if (choice == reader_type::real)
    {
        rds.push_back(cAnalogRead());
    }
    return rds;
}

// elem factory
[[nodiscard]] reader factory_reader(reader_type choice)
{
    if (choice == reader_type::sim)
    {
        return cAnalogReadSim();
    }
    if (choice == reader_type::real)
    {
        return cAnalogRead();
    }
    assert(false);
    return cAnalogReadSim();
}
