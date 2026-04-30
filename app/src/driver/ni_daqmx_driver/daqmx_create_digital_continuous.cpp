/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     03/01/2026
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "daqmx_create_digital_continuous.h"
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

/// <Digital Continuous output>
/// 
/// </separate Analog and digital>
/// <returns></returns>

DAQ_CO_CREATE_STATUS daqmxCreateDigitalContinuous::get_status()
{
    return m_status;
}

IO_TYPE daqmxCreateDigitalContinuous::get_type()
{
    return m_type;
}

double daqmxCreateDigitalContinuous::read(pin_t digital_io_id)
{
    std::print("[!] daqmxCreateDigitalContinuous::read() Not yet implemented\n");
#ifdef _WIN32
    std::string msg = std::format("[!] daqmxCreateDigitalContinuous::read() Not yet implemented\n");
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    assert(false);
    return 42.0;
}

bool daqmxCreateDigitalContinuous::write(pin_t digital_io_id, Frequency_hz<double> rate)
{
    TaskHandle task = m_digital_task_handle[digital_io_id.get()];
    auto result = std::visit([&task, &rate](auto&& arg) {return arg.write(task, rate);
        }, wr
    );
    return result;
}

bool daqmxCreateDigitalContinuous::stop(pin_t digital_io_id)
{
    TaskHandle task = m_digital_task_handle[digital_io_id.get()];
    if (task)
    {
        DAQmxStopTask(task);
        return true;
    }
    return false;
}

void daqmxCreateDigitalContinuous::close()
{
    for (auto task : m_digital_task_handle)
    {
        if (task)
        {
            DAQmxStopTask(task);
            DAQmxClearTask(task);
            task = 0;
        }
    }
}

daqmxCreateDigitalContinuous::daqmxCreateDigitalContinuous(const std::string device_name, digital_pins_continuous digital_io_id) :
    daq_ret(0), m_status(DAQ_CO_CREATE_STATUS::NO_ERR), m_type(IO_TYPE::dig_co)
{
    dbg::print("[*] daqmxCreateDigitalContinuous ctor\n");

    if (!sanitize_inputs(digital_io_id, MAX_DIGITAL_CHANNELS))
    {
        m_status = DAQ_CO_CREATE_STATUS::INPUTS_NOT_VALID;
        std::print(RED"[!] Sanitize inputs detect access violation.\n");
        std::print(NC);
#ifdef _WIN32
        std::string msg = std::format("[!] daqmxCreateDigitalContinuous::Sanitize_inputs() detect access violation\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return;
    }

        // Init some tasks of size max
    for (size_t o = 0; o < MAX_DIGITAL_CHANNELS; o++)
    {
        m_digital_task_handle.push_back(TaskHandle());
    }

    size_t c = 0;

    // Pf0.<0..7> created.
    for (auto& task : m_digital_task_handle)
    {
        assert(c < MAX_DIGITAL_CHANNELS);
        daq_ret = DAQmxCreateTask("", &m_digital_task_handle[c]);
        if (0 != daq_ret)
        {
            m_status = DAQ_CO_CREATE_STATUS::ERR_CREATE_TASK;
            std::print(RED"[!] DAQmxCreateTask for digital channels Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateDigitalContinuous::DAQmxCreateTask for digital channels Failed\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }
        c++;
    }

    std::vector<std::string> channel_name;
    std::vector<std::string> channel_physical_name;
    std::string base_channel_name = device_name + "/port0/line";

    for (auto& input : digital_io_id)
    {
        channel_name.push_back("digital" + std::to_string(input));
        channel_physical_name.push_back(base_channel_name + std::to_string(input));
    }

    c = 0;
    for (auto& task : m_digital_task_handle)
    {
        if (c >= digital_io_id.size())
        {
            break;
        }

        // Only works with counter output
        daq_ret = DAQmxCreateCOPulseChanFreq(
            task,
            channel_physical_name[c].c_str(),//"Dev2/port1/line1",
            channel_name[c].c_str(),
            DAQmx_Val_Hz,
            DAQmx_Val_Rising,
            0.0,                // idle state low
            1000,               // frequency 1kHz
            0.5				    // duty cycle
        );
        // -201003 	Device cannot be accessed. Possible causes: Device is no longer present in the system. Device is not powered. Device is powered, but was temporarily without power. Device is damaged. Ensure the device is properly connected and powered. Turn the computer off and on again. If you suspect that the device is damaged, contact National Instruments at ni.com/support. 
        // -200376 Requested creation of a separate channel for each line is not possible when a digital port is specified as the physical channel. Specify a range of digital lines, such as "Dev1/port0/line0:7", as the physical channel. 
        // -200430:I/O type of the physical channel does not match the I/O type required for the virtual channel you are creating. 
        if (0 != daq_ret)
        {
            m_status = DAQ_CO_CREATE_STATUS::ERR_CREATE_CO_PULSE;

            std::print(RED"DAQmxCreateCOChan() Failedwith code: {}\n", daq_ret);
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateDigitalContinuous::DAQmxCreateCOChan() Failed with code: {}\n", daq_ret);
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }

        c++;
    }

    // Select the current writer type (sim, real,...)
    wr = factory_writer_co(writer_type::digital_real_co);
}

daqmxCreateDigitalContinuous::~daqmxCreateDigitalContinuous()
{
    dbg::print("[*] daqmxCreateDigitalContinuous dtor\n");
}

#endif