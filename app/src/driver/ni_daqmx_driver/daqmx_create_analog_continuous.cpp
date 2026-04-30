/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     03/01/2026
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////

#include "daqmx_create_analog_continuous.h"
#include "sanitize_inputs.h"
#include "term_colors.h"
#include "dbgprint.h"
#include <print>
#include <format>
#include <cassert>
#include <string>
#include <variant>
#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef __aarch64__
/// <Analog Continuous output>
/// 
/// </separate Analog and digital>
/// <returns></returns>

DAQ_CO_CREATE_STATUS daqmxCreateAnalogContinuous::get_status()
{
    return m_status;
}

IO_TYPE daqmxCreateAnalogContinuous::get_type()
{
    return m_type;
}

double daqmxCreateAnalogContinuous::read(pin_t analog_io_id)
{
    std::print("[!] daqmxCreateAnalogContinuous::read() Not yet implemented\n");
#ifdef _WIN32
    std::string msg = std::format("[!] daqmxCreateAnalogContinuous::read() Not yet implemented\n");
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    assert(false);
    return 42.0;
}

bool daqmxCreateAnalogContinuous::write(pin_t analog_io_id, Frequency_hz<double> rate)
{
    TaskHandle task = m_analog_task_handle[analog_io_id.get()];
    auto result = std::visit([&task, &rate](auto&& arg) {return arg.write(task, rate);
        }, wr
    );
    return result;
}

bool daqmxCreateAnalogContinuous::stop(pin_t analog_io_id)
{
    TaskHandle task = m_analog_task_handle[analog_io_id.get()];
    if (task)
    {
        DAQmxStopTask(task);
        return true;
    }
    return false;
}

void daqmxCreateAnalogContinuous::close()
{
    for (auto task : m_analog_task_handle)
    {
        if (task)
        {
            DAQmxStopTask(task);
            DAQmxClearTask(task);
            task = 0;
        }
    }
}
daqmxCreateAnalogContinuous::daqmxCreateAnalogContinuous(const std::string device_name, analog_pins_continuous analog_io_ids) :
    daq_ret(0), m_status(DAQ_CO_CREATE_STATUS::NO_ERR), m_type(IO_TYPE::ana_co)
{
    dbg::print("[*] daqmxCreateAnalogContinuous ctor\n");
    // Init some tasks of size max

    for (size_t o = 0; o < MAX_ANALOG_OUTPUT_CHANNELS; o++)
    {
        m_analog_task_handle.push_back(TaskHandle());
    }

    if (!sanitize_inputs(analog_io_ids, MAX_DIGITAL_CHANNELS))
    {
        m_status = DAQ_CO_CREATE_STATUS::INPUTS_NOT_VALID;
        std::print(RED"[!] Sanitize inputs detect access violation.\n");
        std::print(NC);
#ifdef _WIN32
        std::string msg = std::format("[!] daqmxCreateAnalogContinuous::Sanitize_inputs() detect access violation\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return;
    }

    size_t c = 0;

    // Only AO<0..1> created as it seams sufficent.
    for (auto& task : m_analog_task_handle)
    {
        assert(c < MAX_ANALOG_OUTPUT_CHANNELS);
        daq_ret = DAQmxCreateTask("", &m_analog_task_handle[c]);
        if (0 != daq_ret)
        {
            m_status = DAQ_CO_CREATE_STATUS::ERR_CREATE_TASK;
            std::print(RED"[!] DAQmxCreateTask for analog continuous channels Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateAnalogContinuous::DAQmxCreateTask for analog channels Failed\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }
        c++;
    }

    std::vector<std::string> channel_name;
    std::vector<std::string> channel_physical_name;
    std::string base_channel_name = device_name + "/AO";

    for (auto& input : analog_io_ids)
    {
        channel_name.push_back("analog" + std::to_string(input));
        channel_physical_name.push_back(base_channel_name + std::to_string(input));
    }

    c = 0;
    for (auto& task : m_analog_task_handle)
    {
        if (c >= analog_io_ids.size())
        {
            break;
        }
        // Analog out
        // 
        // Create an appropriate chan with 

        daq_ret = DAQmxCreateAOVoltageChan(
            m_analog_task_handle[c],
            channel_physical_name[c].c_str(),
            channel_name[c].c_str(),
            0.0,
            3.3,
            DAQmx_Val_Volts,
            NULL
        );
        //-200376 Requested creation of a separate channel for each line is not possible when a digital port is specified as the physical channel. Specify a range of digital lines, such as "Dev1/port0/line0:7", as the physical channel. 
        // -200430:I/O type of the physical channel does not match the I/O type required for the virtual channel you are creating. 
        if (0 != daq_ret)
        {
            m_status = DAQ_CO_CREATE_STATUS::ERR_CREATE_CO_PULSE;

            std::print(RED"DAQmxCreateAOVoltageChan() Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateAnalogContinuous::DAQmxCreateAOVoltageChan() Failed with error: {}\n", daq_ret);
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }

        c++;
    }

    // Select the current writer type (sim, real,...)
    wr = factory_writer_co(writer_type::analog_real_co);
}

daqmxCreateAnalogContinuous::~daqmxCreateAnalogContinuous()
{
    dbg::print("[*] daqmxCreateAnalogContinuous dtor\n");
}
#endif