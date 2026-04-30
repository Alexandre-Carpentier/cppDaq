/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     01/01/26
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include <print>
#include <format>
#include <cassert>
#include <string>
#include <variant>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "daqmx_create_analog.h"
#include "sanitize_inputs.h"
#include "term_colors.h"
#include "check.h"
#include "dbgprint.h"

#ifndef __aarch64__
/// <Analog>
/// 
/// </separate Analog and digital>
/// <returns></returns>

DAQ_AN_CREATE_STATUS daqmxCreateAnalog::get_status()
{
    return m_status;
}

IO_TYPE daqmxCreateAnalog::get_type()
{
    return m_type;
}

double daqmxCreateAnalog::read(pin_t analog_io_id)
{ 
    assert(analog_io_id.get() >= 0);
    assert(analog_io_id.get() < MAX_ANALOG_INPUT_CHANNELS);
    auto result = std::visit([this, analog_io_id](auto&& arg) { return arg.read(m_analog_task_handle[analog_io_id.get()]); }
        , rd
    );
    return result;
}

bool daqmxCreateAnalog::write(pin_t analog_io_id, digital_state state)
{
    std::print("[!] daqmxCreateAnalog::write() Not yet implemented\n");
#ifdef _WIN32
    std::string msg = std::format("[!] Error daqmxCreateAnalog::write() Not yet implemented\n");
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif

    return false;
}

void daqmxCreateAnalog::close()
{
    #ifndef __aarch64__ 
    for (auto task : m_analog_task_handle)
    {
        if (task)
        {
            DAQmxStopTask(task);
            DAQmxClearTask(task);
            task = 0;
        }
    }
    #endif
}

daqmxCreateAnalog::daqmxCreateAnalog(const std::string device_name, analog_pins analog_io_ids):
    m_daq_ret(0), m_status(DAQ_AN_CREATE_STATUS::NO_ERR), m_type(IO_TYPE::ana)
{

        //Sanitize

    cppDaqCheck m_daq_dev(device_name);
    if (m_daq_dev.get_status() != DAQ_CHECK_STATUS::NO_ERR)
    {
        m_status = DAQ_AN_CREATE_STATUS::DEVICE_NOT_FOUND;
        std::print(RED"[!] DAQ fail to retrieve daq dev name\n");
        std::print(NC);
#ifdef _WIN32
        std::string msg = std::format("DAQ fail to retrieve daq dev name\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return;
    }

    if (!sanitize_inputs(analog_io_ids, MAX_ANALOG_INPUT_CHANNELS))
    {
        m_status = DAQ_AN_CREATE_STATUS::INPUTS_NOT_VALID;
        std::print(RED"[!] Sanitize inputs detect access violation.\n");
        std::print(NC);
#ifdef _WIN32
        std::string msg = std::format("[!] Error in daqmxCreateAnalog DAQ_AN_CREATE_STATUS::INPUTS_NOT_VALID\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return;
    }

        // Init some tasks of size max

    for (size_t o = 0; o < MAX_ANALOG_INPUT_CHANNELS; o++)
    {
        m_analog_task_handle.push_back(TaskHandle());
    }
    dbg::print("[*] daqmxCreateAnalog ctor\n");
    //std::print("[*] daqmxCreateAnalog ctor\n");
    assert(m_analog_task_handle.size() > 0);

    size_t c = 0;

    // Only Pf0.<0..7> created as it seams sufficent.
    for (auto& task : m_analog_task_handle)
    {
        std::string task_name = "daqmxCreateAnalog" + std::to_string(c);
        assert(c < MAX_ANALOG_INPUT_CHANNELS);
        m_daq_ret = DAQmxCreateTask(task_name.c_str(), &task);
        if (0 != m_daq_ret)
        {
            //Error -200089 	Task name specified conflicts with an existing task name. 
            m_status = DAQ_AN_CREATE_STATUS::ERR_CREATE_TASK;
            std::print(RED"[!] DAQmxCreateTask for analog channels Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("Error in daqmxCreateAnalog DAQ_AN_CREATE_STATUS::ERR_CREATE_TASK code: {}\n", m_daq_ret);
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }
        c++;
    }

    std::string base_channel_name = device_name + "/ai";
    std::vector<std::string> channel_name;
    std::vector<std::string> channel_physical_name;

    for (auto &input: analog_io_ids)
    {
        channel_name.push_back("analog" + std::to_string(input));
        channel_physical_name.push_back(base_channel_name + std::to_string(input));
    }

    size_t pos = 0;
    for (auto& input : analog_io_ids)
    {

        double min_range = -10.0;
        double max_range = +10.0;

        // Analog in
        // 
        // Create an appropriate chan with DAQmxCreateAIVoltageChan()

        m_daq_ret = DAQmxCreateAIVoltageChan(
            m_analog_task_handle[pos],
            channel_physical_name[pos].c_str(),           // DEV1/ai0
            channel_name[pos].c_str(),                    // Chan0
            DAQmx_Val_RSE,                              // RSE/Differential/...
            min_range,                                  // -10V
            max_range,                                  // +10V
            DAQmx_Val_Volts,                            // return Volt or custom scale
            NULL);

        if (0 != m_daq_ret)
        {
            m_status = DAQ_AN_CREATE_STATUS::ERR_CREATE_AI_VOLTAGE;
            std::print(RED"[!] DAQmxCreateAIVoltageChan() Failed\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("[!] Error in daqmxCreateAnalog DAQ_AN_CREATE_STATUS::ERR_CREATE_AI_VOLTAGE\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            close();
            return;
        }

            // Check the task

        char buffer[100] = "";
        DAQmxGetNthTaskReadChannel(m_analog_task_handle[pos], 1, buffer, 100);
        std::string task_name = buffer;
        if (task_name.size() == 0)
        {
#ifdef _WIN32
            std::string msg = std::format("[!] Error in daqmxCreateAnalog task_name is 0\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            m_status = DAQ_AN_CREATE_STATUS::ERR_READ_TASK;
            assert(false);
        }

        pos++;
    }

    // Select the current reader type (sim, real,...)
    rd = factory_reader(reader_type::real);
}

daqmxCreateAnalog::~daqmxCreateAnalog()
{
    dbg::print("[*] daqmxCreateAnalog dtor\n");
};
#endif