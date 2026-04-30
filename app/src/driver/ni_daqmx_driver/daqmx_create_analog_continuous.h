/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     03/01/2026
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef __x64_x86__
#include <NIDAQmx.h>
#endif
#include <vector>
#include "cppDaq.h"
#include "io_types.h"
#include "daq_co_create_status.h"

#include <read.h> // To create reader type : reader rd;
#include <write.h> // To create writer type : writer wr;


#ifndef __aarch64__
class daqmxCreateAnalogContinuous {
private:
    std::vector<TaskHandle> m_analog_task_handle;
    int32 daq_ret;
    // Store the writer type (simulated or real,...)
    writer_continuous wr;// defined at write.h
    DAQ_CO_CREATE_STATUS m_status;
    IO_TYPE m_type;
public:
    [[nodiscard]] DAQ_CO_CREATE_STATUS get_status();
    [[nodiscard]] IO_TYPE get_type();

    [[nodiscard]] double read(pin_t analog_io_id);

    bool write(pin_t analog_io_id, Frequency_hz<double> rate);
    bool stop(pin_t analog_io_id);
    void close();

    daqmxCreateAnalogContinuous(const std::string device_name, analog_pins_continuous analog_io_ids);

    ~daqmxCreateAnalogContinuous();
};
#endif