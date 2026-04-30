/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     15/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __aarch64__
#include <NIDAQmx.h>
#endif
#include <string>
enum class DAQ_CHECK_STATUS {
    NO_ERR = 0,
    DEV_EMPTY,
    NO_SERIAL_FOUND,
    SELF_TEST_ISSUE,
    ERR_AI_SUPPORTED,
    ERR_NO_DEV_PRODUCT_TYPE,
    ERR_NO_DEV_CHASSIS_DEV_NAMES
};

#ifndef __aarch64__
class daqmxcppDaqCheck {
private:
    std::string m_device_name;
    int32 m_meas_types[29] = { 0 }; // officially 29 different type is supported by NI
    std::string m_product_type;
    std::string m_module_names;
    int32 m_daq_ret;
    DAQ_CHECK_STATUS m_status;
public:
    daqmxcppDaqCheck(){};
    daqmxcppDaqCheck(std::string device_name);
    DAQ_CHECK_STATUS get_status();
};
#endif

// Forward class
class cppDaqCheck {
public:
    cppDaqCheck(std::string device_name)
#ifndef __aarch64__
        :check(device_name)
#endif
    {
        if(device_name.size()==0)
        {
            res = DAQ_CHECK_STATUS::DEV_EMPTY;
        }
    }
    DAQ_CHECK_STATUS get_status()
    {
#ifndef __aarch64__
        res = check.get_status();
#endif
        return res;
    }
private:
DAQ_CHECK_STATUS res = DAQ_CHECK_STATUS::NO_SERIAL_FOUND;
#ifndef __aarch64__
    daqmxcppDaqCheck check;
#endif
};


