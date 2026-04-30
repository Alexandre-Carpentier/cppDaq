/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     16/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////

#include "find_device.h"
#ifndef __aarch64__
#include <NIDAQmx.h>
#endif
#include "term_colors.h"
#include <print>
#include <assert.h>

#ifndef __aarch64__
class cppDaqDevice::cppDaqDeviceImpl
{
private:
    std::string m_devices_list;
    std::string m_device_name;
    int32 m_daq_ret;
    DAQ_DEVICE_STATUS m_status;
public:
    ~cppDaqDeviceImpl() = default;

    cppDaqDeviceImpl() :
        m_daq_ret(0), m_status(DAQ_DEVICE_STATUS::DEV_EMPTY)
    {
        size_t len = DAQmxGetSysDevNames(NULL, 0);
        if (len == 0)
        {
            m_status = DAQ_DEVICE_STATUS::GET_DEV_FAIL;
            std::print(RED"[!] DAQ name is empty.\n");
            std::print(NC);
            return;
        }

        std::string devices(len, 0);
        int32 daq_ret = DAQmxGetSysDevNames(devices.data(), len);
        if ((devices.empty()) || (0 != daq_ret))
        {
            m_status = DAQ_DEVICE_STATUS::DEV_EMPTY;
            std::print(RED"[!] DAQ name is empty.\n");
            std::print(NC);
            return;
        }

        std::string begin_str = devices.substr(0, 3);
        if (begin_str.compare("Sim") == 0)
        {
            m_status = DAQ_DEVICE_STATUS::DEV_NOT_CONNECTED;
            std::print(RED"[!] DAQ not connected.\n");
            std::print(NC);
            return;
        }
            // Removing trailing \0 "Dev1\0"

        devices.resize(len - 1);
        m_devices_list = devices;

            // Look for multiple devices "Dev1,Dev1sim,Dev..."

        size_t pos = m_devices_list.find(",");
        m_device_name = m_devices_list;
        if (pos != std::string::npos)
        {
            // Select first available "Dev1"
            m_device_name = m_devices_list.substr(0, pos);
        }

        if (m_device_name.size() == 0)
        {
            m_status = DAQ_DEVICE_STATUS::DEV_EMPTY;
        }

        // Daq found
        std::print("[*] Device found:{}\n", m_device_name);
        m_status = DAQ_DEVICE_STATUS::NO_ERR;
        return;
    }

    cppDaqDeviceImpl(std::string dev) :
        m_daq_ret(0), m_status(DAQ_DEVICE_STATUS::DEV_EMPTY)
    {
        if ((dev.empty()))
        {
            m_status = DAQ_DEVICE_STATUS::DEV_EMPTY;
            std::print(RED"[!] DAQ name is empty.\n");
            std::print(NC);
            return;
        }

        m_device_name = dev;
        // Daq found
        std::print("[*] Device found:{}\n", m_device_name);
        m_status = DAQ_DEVICE_STATUS::NO_ERR;
        return;
    }

    [[nodiscard]] DAQ_DEVICE_STATUS get_status()
    {
        return m_status;
    }

    [[nodiscard]] std::string get_name()
    {
        return m_device_name;
    }
};
#endif
// Forward

// Constructor 1: found auto the first device to connect
cppDaqDevice::cppDaqDevice() 
#ifndef __aarch64__
:
    pdaqdev(std::make_unique<cppDaqDeviceImpl>())
#endif
{
    #ifndef __aarch64__
    assert(pdaqdev);
    #endif
}

// Constructor 2: use this device
cppDaqDevice::cppDaqDevice(std::string dev)
#ifndef __aarch64__
:
    pdaqdev(std::make_unique<cppDaqDeviceImpl>(dev))
#endif
{
    #ifndef __aarch64__
    if (dev.size() == 0)
    {
        pdaqdev = std::make_unique<cppDaqDeviceImpl>();
    }
    assert(pdaqdev);
    #endif
}

// Forward
[[nodiscard]] std::string cppDaqDevice::get_name()
{
    std::string res = "Unsuported_platform_apple_silicon";
    #ifndef __aarch64__
    res = pdaqdev->get_name();
    #endif
    return res;
}

// Forward
[[nodiscard]] DAQ_DEVICE_STATUS cppDaqDevice::get_status()
{
    DAQ_DEVICE_STATUS res = DAQ_DEVICE_STATUS::DEV_EMPTY;
    #ifndef __aarch64__
    res = pdaqdev->get_status();
    #endif
    return res;
}

// Mandatory to pimpl+unique_ptr
cppDaqDevice::~cppDaqDevice()= default;

/////////////////////////////////////////////////////
//
// This helper function is list all availables interface from NI
#ifndef __aarch64__
class cppDaqFindDevice::cppDaqFindDeviceImpl
{
private:
    std::vector<std::string> m_device_list;
    int32 m_daq_ret;
    DAQ_DEVICE_STATUS m_status;
public:
    cppDaqFindDeviceImpl() :m_status(DAQ_DEVICE_STATUS::NO_ERR), m_daq_ret(0)
    {

        size_t len = DAQmxGetSysDevNames(NULL, 0);
        if (len == 0)
        {
            m_status = DAQ_DEVICE_STATUS::GET_DEV_FAIL;
            std::print(RED"[!] Get DEV names fail.\n");
            std::print(NC);
            return;
        }

        std::string devices(len-1, 0); // len-1 = remove the \0 for C style
        int32 daq_ret = DAQmxGetSysDevNames(devices.data(), len);
        if ((devices.empty()) || (0 != daq_ret))
        {
            m_status = DAQ_DEVICE_STATUS::DEV_EMPTY;
            std::print(RED"[!] DAQ name is empty.\n");
            std::print(NC);
            return;
        }

        std::string begin_str = devices.substr(0, 3);
        if (begin_str.compare("Sim") == 0)
        {
            m_status = DAQ_DEVICE_STATUS::DEV_NOT_CONNECTED;
            std::print(RED"[!] DAQ not connected.\n");
            std::print(NC);
            return;
        }

        size_t pos = 0;
        while (pos < std::string::npos)
        {
            pos = devices.find(",");
            m_device_list.emplace_back(devices.substr(0, pos));
            devices = devices.erase(0, pos+1);
        }

        if (m_device_list.size() == 0)
        {
            assert(m_device_list.size() > 0);
            m_status = DAQ_DEVICE_STATUS::DEV_EMPTY;
        }

        // Daq found
        std::print("[*] Device found:\n");
        for (auto& dev : m_device_list)
        {
            std::print("[*]\"{}\"\n", dev);
        }

        m_status = DAQ_DEVICE_STATUS::NO_ERR;
        return;
    }

    std::vector<std::string> get_list()
    {
        if (m_device_list.size() == 0)
        {
            m_status = DAQ_DEVICE_STATUS::DEV_EMPTY;
        }
        return m_device_list;
    }

    DAQ_DEVICE_STATUS get_status()
    {
        return m_status;
    }
};
#endif

cppDaqFindDevice::cppDaqFindDevice()
#ifndef __aarch64__
:
    pfinddev(std::make_unique<cppDaqFindDeviceImpl>())
    #endif
{
    #ifndef __aarch64__
    assert(pfinddev);
    #endif
    return;
}

std::vector<std::string> cppDaqFindDevice::get_list()
{
    std::vector<std::string> res{};
    #ifndef __aarch64__
    res = pfinddev->get_list();
    #endif
    return res;
}

DAQ_DEVICE_STATUS cppDaqFindDevice::get_status()
{
    DAQ_DEVICE_STATUS res = DAQ_DEVICE_STATUS::DEV_EMPTY;
    #ifndef __aarch64__
    res = pfinddev->get_status();
    #endif
    return res;
}

cppDaqFindDevice::~cppDaqFindDevice()
{
    std::print("cppDaqFindDevice dtor\n");
}
