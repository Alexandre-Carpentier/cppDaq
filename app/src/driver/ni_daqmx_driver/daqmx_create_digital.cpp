/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     03/01/26
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "daqmx_create_digital.h"
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
/// <Digital>
/// 
/// </separate Analog and digital>
/// <returns></returns>

DAQ_DI_CREATE_STATUS daqmxCreateDigital::get_status()
{
    return m_status;
}

IO_TYPE daqmxCreateDigital::get_type()
{
    return m_type;
}

double daqmxCreateDigital::read(pin_t digital_io_id)
{
    std::print("[!] daqmxCreateDigital::read() Not yet implemented\n");
#ifdef _WIN32
    std::string msg = std::format("[!] daqmxCreateDigital::read() Not yet implemented\n");
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    assert(false);
    return 42.0;
}

bool daqmxCreateDigital::write(pin_t digital_io_id, digital_state state)
{
    TaskHandle task = m_digital_task_handle[digital_io_id.get()];

    // Error indice = 1 when 1 pin.
    //TaskHandle task = m_digital_task_handle[0];
    // --> Fix digital_io_id is the Task position.
    auto result = std::visit([&task, &state](auto&& arg) {return arg.write(task, state);
        }, wr
    );
    return result;
}

void daqmxCreateDigital::close()
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

daqmxCreateDigital::daqmxCreateDigital(const std::string device_name, digital_pins digital_io_ids):
    daq_ret(0), m_status(DAQ_DI_CREATE_STATUS::NO_ERR), m_type(IO_TYPE::dig)
{
    dbg::print("[*] daqmxCreateDigital ctor\n");
    // Init some tasks of size max

    for (size_t o = 0; o < MAX_DIGITAL_CHANNELS; o++)
    {
        m_digital_task_handle.push_back(TaskHandle());
    }

    if (!sanitize_inputs(digital_io_ids, MAX_DIGITAL_CHANNELS))
    {
        m_status = DAQ_DI_CREATE_STATUS::INPUTS_NOT_VALID;
        std::print(RED"[!] Sanitize inputs detect access violation.\n");
        std::print(NC);
#ifdef _WIN32
        std::string msg = std::format("[!] daqmxCreateDigital::Sanitize_inputs() detect access violation\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return;
    }

    size_t c = 0;

    // Only Pf0.<0..7> created as it seams sufficent.
    for (auto& task : m_digital_task_handle)
    {
        assert(c<MAX_DIGITAL_CHANNELS);
        daq_ret = DAQmxCreateTask("", &m_digital_task_handle[c]);
        if (0 != daq_ret)
        {
            m_status = DAQ_DI_CREATE_STATUS::ERR_CREATE_TASK;
            std::print(RED"[!] DAQmxCreateTask for digital channels Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateDigital::DAQmxCreateTask for digital channels Failed\n");
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

    for (auto& input : digital_io_ids)
    {
        channel_name.push_back("digital" + std::to_string(input));
        channel_physical_name.push_back(base_channel_name + std::to_string(input));
    }

    c = 0;
    for (auto& task : m_digital_task_handle)
    {
        if (c >= digital_io_ids.size())
        {
            break;
        }


        // Digital out
        // 
        // Create an appropriate chan with DAQmxCreateDOChan()

        daq_ret = DAQmxCreateDOChan(
            m_digital_task_handle[c],
            channel_physical_name[c].c_str(),            // DEV1/ai0
            channel_name[c].c_str(),                     // Digital 0
            DAQmx_Val_ChanForAllLines                    // lineGrouping:  DAQmx_Val_ChanPerLine/ DAQmx_Val_ChanForAllLines              
        );

        if (0 != daq_ret)
        {
            m_status = DAQ_DI_CREATE_STATUS::ERR_CREATE_DO_LOGIC;
            std::print(RED"DAQmxCreateDOChan() Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateDigital::DAQmxCreateDOChan() Failed\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }

        // 
        // Set digital output to active drive (push pull mode
        daq_ret = DAQmxSetDOOutputDriveType(m_digital_task_handle[c], channel_physical_name[c].c_str(), DAQmx_Val_ActiveDrive); //DAQmx_Val_OpenCollector
        if (0 != daq_ret)
        {
            //-200486 Specified channel is not in the task. 
            m_status = DAQ_DI_CREATE_STATUS::ERR_CREATE_DO_DRIVE_TYPE;
            std::print(RED"DAQmxCreateDOChan() Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] daqmxCreateDigital::DAQmxCreateDOChan() Failed\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }


        c++;
    }

    // Select the current writer type (sim, real,...)
    wr = factory_writer_single(writer_type::digital_real);
}

daqmxCreateDigital::~daqmxCreateDigital()
{
    dbg::print("[*] daqmxCreateDigital dtor\n");
}
#endif