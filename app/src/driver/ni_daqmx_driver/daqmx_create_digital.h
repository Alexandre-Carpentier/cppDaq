#pragma once

#ifdef __x64_x86__
#include <NIDAQmx.h>
#endif
#include <vector>
#include "cppDaq.h"
#include "io_types.h"

#include <read.h> // To create reader type : reader rd;
#include <write.h> // To create writer type : writer wr;

enum class DAQ_DI_CREATE_STATUS {
    NO_ERR = 0,
    INPUTS_NOT_VALID,
    ERR_CREATE_TASK,
    ERR_CREATE_DO_DRIVE_TYPE,
    ERR_CREATE_DO_LOGIC,
    ERR_READ_TASK
};

#ifndef __aarch64__
class daqmxCreateDigital final {
private:
    std::vector<TaskHandle> m_digital_task_handle;
	int32 daq_ret;
    // Store the writer type (simulated or real,...)
    writer_single wr; // defined at write.h
    DAQ_DI_CREATE_STATUS m_status;
    IO_TYPE m_type;
public:
    [[nodiscard]] DAQ_DI_CREATE_STATUS get_status();
    [[nodiscard]] IO_TYPE get_type();

    [[nodiscard]] double read(pin_t digital_io_id);

    bool write(pin_t digital_io_id, digital_state state);

    void close();

    daqmxCreateDigital(const std::string device_name, digital_pins digital_io_ids);

    ~daqmxCreateDigital();
};
#endif