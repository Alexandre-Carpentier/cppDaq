/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     15/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////

#include "check.h"
#include <print>
#include <assert.h>
#include "term_colors.h"
#ifndef __aarch64__
daqmxcppDaqCheck::daqmxcppDaqCheck(std::string device_name) :m_device_name(device_name)
{
    m_status = DAQ_CHECK_STATUS::NO_ERR;

    std::print("[*] Checking Daq\n");

        // Sanitize input

    if (m_device_name.empty())
    {
        m_status = DAQ_CHECK_STATUS::DEV_EMPTY;
        std::print(RED"[!] m_device_name empty.\n");
        std::print(NC);
        return;
    }

    // It is important for string concatenation that \0 is not include ex: "Dev1\0/ai2" -> "Dev1/ai2"
    char end = m_device_name[m_device_name.size()-1];
    if ( end == '\0')
    {
        m_device_name.resize(m_device_name.size() - 1);
        std::print(CYN"[!] m_device_name incorporate \0 char: removing it for string compatibility.\n");
        std::print(NC);
    }

        // Check serial number to avoid simulated devices

    uInt32 dev_sn = 0;
    DAQmxGetDevSerialNum(m_device_name.c_str(), &dev_sn);
    if (dev_sn == 0)
    {
        m_status = DAQ_CHECK_STATUS::NO_SERIAL_FOUND;
        std::print(RED"[!] Serial number not found. This is not a valid device (probably a simulated device).\n");
        std::print(NC);
        return;
    }

        // Check DAQmx

    m_daq_ret = DAQmxSelfTestDevice(m_device_name.c_str());
    if (0 != m_daq_ret)
    {
        m_status = DAQ_CHECK_STATUS::SELF_TEST_ISSUE;
        std::print(RED"[!] DAQmxSelfTestDevice Failed.\n");
        std::print(NC);
        return;
    }

        // Check compatible measurement

    m_daq_ret = DAQmxGetDevAISupportedMeasTypes(m_device_name.c_str(), m_meas_types, _countof(m_meas_types));
    if (0 != m_daq_ret)
    {
        m_status = DAQ_CHECK_STATUS::ERR_AI_SUPPORTED;
        std::print(RED"[!] Error when calling DAQmxGetDevAISupportedMeasTypes().\n");
        std::print(NC);
        return;
    }

    bool bvolt = false;
    for (auto& meastype : m_meas_types)
    {
        if (meastype == DAQmx_Val_Voltage)  // DAQmx_Val_Voltage 	    10322 	Voltage measurement.  
            bvolt = true;
    }

        // Warn user if voltage measurement is not supported

    if (!bvolt)
    {
        std::print(RED"[!] Be carefull your DAQ system desn't support Voltage measurements.\n");
        std::print(NC);
    }

        // Get product type and module names

    char producttype[256] = "";
    m_daq_ret = DAQmxGetDevProductType(m_device_name.c_str(), producttype, _countof(producttype)); // Indicates the product name of the device.
    m_product_type = producttype;
    if ((m_product_type.size() == 0) || (m_daq_ret != 0))
    {
        m_status = DAQ_CHECK_STATUS::ERR_NO_DEV_PRODUCT_TYPE;
        std::print(RED"[!] DAQmxGetDevProductType() failed.\n");
        std::print(NC);
        return;
    }

    char modulenames[256] = "";
    m_daq_ret = DAQmxGetDevChassisModuleDevNames(m_device_name.c_str(), modulenames, _countof(modulenames));
    if (m_daq_ret != 0)
    {
        m_status = DAQ_CHECK_STATUS::ERR_NO_DEV_CHASSIS_DEV_NAMES;
    }

    m_module_names = modulenames;
    if ((m_module_names.size() == 0) )
    {
        std::print("[*] No chassis mounted.\n");
    }
}

DAQ_CHECK_STATUS daqmxcppDaqCheck::get_status()
{
    return m_status;
}
#endif