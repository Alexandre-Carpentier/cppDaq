/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     13/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////

#include "write.h"
#include "dbgprint.h"
#include <print>
#include <vector>
#include <string>
#include <assert.h>
#include <variant>
#include <format>
#ifdef WIN32
#include <Windows.h>
#endif

#ifndef __aarch64__
// Sim mock
[[nodiscard]] bool cDigitalWriteSim::write(TaskHandle m_digital_task_handle, digital_state state)
{
    std::print("[*] Digital write sim\n");
#ifdef _WIN32
    std::string msg = std::format("cAnalogReadSim called\n");
    MessageBox(0, msg.c_str(), 0, S_OK);
#endif
    return true;
}

// Real impl
[[nodiscard]] bool cDigitalWrite::write(TaskHandle digital_task_handle, digital_state state)
{

    dbg::print("[*] Digital write\n");
    static TaskHandle s_lastTaskHandle { 0 };
    static uInt8 s_last_cmd[1] = {0};
    uInt8 cmd[1];

    if (state == digital_state::HIGH)
    {
        cmd[0] = 1;
    }
    else
    {
        cmd[0] = 0;
    }

    // Prevent calling again
    if(cmd[0] == s_last_cmd[0])
    {
        if (digital_task_handle == s_lastTaskHandle)
        {
            // No change
            return true;
        }
    }
    s_last_cmd[0] = cmd[0];

    if (!digital_task_handle)
    {
        return false;
    }

    const bool bAutostart = true;
    const double timeout_ms = 2000;
    int32 written = 0;
    int32 daq_ret = DAQmxWriteDigitalLines(digital_task_handle, 1, bAutostart, timeout_ms, DAQmx_Val_GroupByChannel, cmd, &written, NULL);
    if (0 != daq_ret)
    {
        // Error -200088 Task specified is invalid or does not exist. 
        // Error -200477 Specified operation cannot be performed when there are no devices in the task. 
        // Error -200587 Requested operation could not be performed, because the specified digital lines are either reserved or the device is not present in NI - DAQmx.
        std::println("[!] DAQmxWriteDigitalLines Failed with err: {}", daq_ret);
        return false;
    }

    //std::print("[*] Digital write\n");
    return true;
}

// Real impl
[[nodiscard]] bool cDigitalDaqWriteCO::write(TaskHandle digital_task_handle, Frequency_hz<double> rate)
{
    dbg::print("[*] Digital write continuously\n");

    // Digital out
    // 
    // Create an appropriate chan with DAQmxCreateCOPulseChanFreq()
	static TaskHandle s_lastTaskHandle{ 0 };
    static Frequency_hz<double> last_freq = Frequency_hz<double>(0.0);
    last_freq = rate;

    if (rate.get() == last_freq.get())
    {
        if (digital_task_handle == s_lastTaskHandle)
        {
            // No change
            return true;
        }
    }

        // Stop the task before reconfiguring the sample clock
    int32 daq_ret = DAQmxStopTask(digital_task_handle);
    // Ignore stop error, try reconfigure anyway

    // Reconfigure the sample clock rate to the desired frequency.
    // For a COPulseChanFreq configured to use the sample clock, changing the
    // sample clock rate updates the output frequency.
    daq_ret = DAQmxCfgSampClkTiming(
        digital_task_handle,
        "",                 // use internal onboard clock
        rate.get(),         // new rate (Hz)
        DAQmx_Val_Rising,
        DAQmx_Val_ContSamps,
        1                   // minimal buffer; hardware dependent
    );
    if (0 != daq_ret)
    {
        return false;
    }

    // Hardware mode implicitly set to sample clock
    daq_ret = DAQmxStartTask(digital_task_handle);
    if (0 != daq_ret)
    {        
        return false;
    }
    return true;
}

// Real impl
[[nodiscard]] bool cAnalogDaqWriteCO::write(TaskHandle m_analog_task_handle, Frequency_hz<double> rate)
{
    dbg::print("[*] Analog write continuously\n");
    int32 daq_ret = 0;

    // Signal parameter
    const double highV = 3.3;
    const double lowV = 0.0;
    const int samplesPerCycle = 2; // low, high => 50% duty
    double sampleRate = rate.get() * samplesPerCycle;
    int32 sampsPerChanToGenerate = samplesPerCycle;

    // Configure sample clock pour g�n�ration continue
    daq_ret = DAQmxCfgSampClkTiming(
        m_analog_task_handle,
        "",                         // use internal onboard clock
        sampleRate,
        DAQmx_Val_Rising,
        DAQmx_Val_ContSamps,
        sampsPerChanToGenerate
    );
    //Error -200077 	Requested value is not a supported value for this property. The property value may be invalid because it conflicts with another property. 
    if (0 != daq_ret)
    {
#ifdef _WIN32
        char errBuf[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuf, sizeof(errBuf));
        std::string msg = std::format("[!] DAQmxCfgSampClkTiming failed code: {} info: {}\n", daq_ret, errBuf);
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return false;
    }

    // Buffer 2 échantillons représentant une période (low, high)
    std::vector<double> data;
    data.reserve(samplesPerCycle);
    data.push_back(lowV);
    data.push_back(highV);

    // Write initial buffer (blocking write)
    int32 written = 0;
    daq_ret = DAQmxWriteAnalogF64(
        m_analog_task_handle,
        sampsPerChanToGenerate,
        0,                  // autostart = false; on démarre explicitement après écriture
        10.0,               // timeout s
        DAQmx_Val_GroupByScanNumber,
        data.data(),
        &written,
        NULL
    );
    if (0 != daq_ret || written != sampsPerChanToGenerate)
    {
#ifdef _WIN32
        char errBuf[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuf, sizeof(errBuf));
        std::string msg = std::format("[!] DAQmxWriteAnalogF64 failed code: {} info: {}\n", daq_ret, errBuf);
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return false;
    }

    // Start generation
    daq_ret = DAQmxStartTask(m_analog_task_handle);
    if (0 != daq_ret)
    {
#ifdef _WIN32
        char errBuf[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuf, sizeof(errBuf));
        std::string msg = std::format("[!] DAQmxStartTask failed code: {} info: {}\n", daq_ret, errBuf);
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return false;
    }

    return true;
}
#endif

// Select vec of writer types
// vector factory
[[nodiscard]] std::vector<writer_single> factory_writers(writer_type choice)
{
    std::vector<writer_single> wrs;
    if (choice == writer_type::sim)
    {
        wrs.emplace_back(cDigitalWriteSim());
    }
    if (choice == writer_type::digital_real)
    {
        wrs.emplace_back(cDigitalWrite());
    }
    return wrs;
}

// elem factory
[[nodiscard]] writer_single factory_writer_single(writer_type choice)
{
    if (choice == writer_type::sim)
    {
        return cDigitalWriteSim();
    }
    if (choice == writer_type::digital_real)
    {
        return cDigitalWrite();
    }

    assert(false);
    return cDigitalWriteSim();
}

// elem factory
[[nodiscard]] writer_continuous factory_writer_co(writer_type choice)
{
    if (choice == writer_type::digital_real_co)
    {
        return cDigitalDaqWriteCO();
    }
    if (choice == writer_type::analog_real_co)
    {
        return cAnalogDaqWriteCO();
    }
    assert(false);
    return cDigitalDaqWriteCO();
}