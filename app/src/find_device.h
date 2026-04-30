/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     16/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <string>
#include <vector>
enum class DAQ_DEVICE_STATUS {
    NO_ERR = 0,
    GET_DEV_FAIL,
    DEV_EMPTY,
    DEV_NOT_CONNECTED
};
// To create the device used in cppDaq
class cppDaqDevice {
private:
#ifndef __aarch64__
    class cppDaqDeviceImpl;
    std::unique_ptr<cppDaqDeviceImpl> pdaqdev;
#endif
public:
    cppDaqDevice(); // Use first found
    cppDaqDevice(std::string dev); // Use particular device
    std::string get_name();
    DAQ_DEVICE_STATUS get_status();

    ~cppDaqDevice();
};

// Forward class
// To find a device Name before using cppDaq
class cppDaqFindDevice {
private:
#ifndef __aarch64__
    class cppDaqFindDeviceImpl;
    std::unique_ptr< cppDaqFindDeviceImpl> pfinddev;
#endif
public:
    cppDaqFindDevice();
    std::vector<std::string> get_list(); // Return all devices found on computer
    DAQ_DEVICE_STATUS get_status();
    ~cppDaqFindDevice();
};
