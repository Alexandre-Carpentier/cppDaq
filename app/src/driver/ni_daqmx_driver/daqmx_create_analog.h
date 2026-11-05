/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     01/01/26
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

#include <read.h> // To create reader type : reader rd;
#include <write.h> // To create writer type : writer wr;

enum class DAQ_AN_CREATE_STATUS {
    NO_ERR = 0,
	DEVICE_NOT_FOUND,
    INPUTS_NOT_VALID,
    OUTPUTS_NOT_VALID,
    ERR_CREATE_TASK,
    ERR_CREATE_AI_VOLTAGE,
    ERR_CREATE_AO_VOLTAGE,
    ERR_READ_TASK
};

#ifndef __aarch64__
class daqmxCreateAnalogInputs final{
private:
    // Holds the TaskHandle of both analog and digital side
    std::vector<TaskHandle> m_analog_task_handle;
    //TaskHandle m_digital_task_handle[MAX_DIGITAL_CHANNELS] = { 0 };
    // Error stored in daq_ret and consultable
    // The error code driver are all consultable from NI
    int32 m_daq_ret;
    // Store the reader type (simulated or real,...),  
    reader rd; // defined at read.h
    DAQ_AN_CREATE_STATUS m_status;
    IO_TYPE m_type;
public:
    [[nodiscard]] DAQ_AN_CREATE_STATUS get_status();
    [[nodiscard]] IO_TYPE get_type();

    [[nodiscard]] double read(pin_t analog_io_id);
    bool write(pin_t analog_io_id, double voltage); // TODO: Fix bad abstraction

    void close();

    daqmxCreateAnalogInputs(const std::string device_name, analog_input_pins analog_io_id);

    ~daqmxCreateAnalogInputs();
};

class daqmxCreateAnalogOutputs final {
private:
    // Holds the TaskHandle of both analog and digital side
    std::vector<TaskHandle> m_analog_task_handle;
    //TaskHandle m_digital_task_handle[MAX_DIGITAL_CHANNELS] = { 0 };
    // Error stored in daq_ret and consultable
    // The error code driver are all consultable from NI
    int32 m_daq_ret;
    // Store the writer type (simulated or real,...),  
    writer_single wr; // defined at write.h
    DAQ_AN_CREATE_STATUS m_status;
    IO_TYPE m_type;
public:
    [[nodiscard]] DAQ_AN_CREATE_STATUS get_status();
    [[nodiscard]] IO_TYPE get_type();

    [[nodiscard]] double read(pin_t analog_io_id); // TODO: Fix bad abstraction
    bool write(pin_t analog_io_id, double voltage);

    void close();

    daqmxCreateAnalogOutputs(const std::string device_name, analog_output_pins analog_io_id);

    ~daqmxCreateAnalogOutputs();
};
#endif
