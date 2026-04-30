/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     15/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef _WIN32
#include "Windows.h"
#endif
#ifdef __x64_x86__
#include <NIDAQmx.h>
#endif

#include <string>

#include "cppDaq.h" 
#include "driver/ni_daqmx_driver/daqmx_create_analog.h"
#include "driver/ni_daqmx_driver/daqmx_create_digital.h"
#include "driver/ni_daqmx_driver/daqmx_create_analog_continuous.h"
#include "driver/ni_daqmx_driver/daqmx_create_digital_continuous.h"
#include "physic_types.h"
#include "io_types.h"
#include "sanitize_inputs.h"
#include "daq_co_create_status.h"


// Proxy classes to platform dependent implementation details
class CreateAnalog {
public:
    CreateAnalog(const std::string device_name, analog_pins analog_io_id)
#ifndef __aarch64__
        : analog{ device_name, analog_io_id }
#endif
    {
    }
    double read(pin_t analog_io_id)
    {
        double res = 0.0;
#ifndef __aarch64__
        res = analog.read(analog_io_id);
#endif
        return res;
    }
    bool write(pin_t analog_io_id, digital_state state)
    {
        bool res = false;
#ifndef __aarch64__
        res = analog.write(analog_io_id, state);
#endif
        return res;
    }
    void close()
    {
#ifndef __aarch64__
        analog.close();
#endif
    }
    DAQ_AN_CREATE_STATUS get_status()
    {
        DAQ_AN_CREATE_STATUS res = DAQ_AN_CREATE_STATUS::INPUTS_NOT_VALID;
#ifndef __aarch64__
        res = analog.get_status();
#endif
        return res;
    }

    IO_TYPE get_type()
    {
        IO_TYPE res = IO_TYPE::unknown;
#ifndef __aarch64__
        res = analog.get_type();
#endif
        return res;
    }

    ~CreateAnalog()
    {
#ifndef __aarch64__
#endif
    }

private:
#ifndef __aarch64__
    daqmxCreateAnalog analog;
#endif
};

class CreateDigital {
public:
    CreateDigital(const std::string device_name, digital_pins digital_io_id)
#ifndef __aarch64__
        :digital(device_name, digital_io_id)
#endif
    {

    }
    double read(pin_t digital_io_id)
    {
        double res = 0.0;
#ifndef __aarch64__
        res = digital.read(digital_io_id);
#endif
        return res;
    }
    bool write(pin_t digital_io_id, digital_state state)
    {
        bool res = false;
#ifndef __aarch64__
        res = digital.write(digital_io_id, state);
#endif
        return res;
    }
    void close()
    {
#ifndef __aarch64__
        digital.close();
#endif
    }
    DAQ_DI_CREATE_STATUS get_status()
    {
        DAQ_DI_CREATE_STATUS res = DAQ_DI_CREATE_STATUS::INPUTS_NOT_VALID;
#ifndef __aarch64__
        res = digital.get_status();
#endif
        return res;
    }

    IO_TYPE get_type()
    {
        IO_TYPE res = IO_TYPE::unknown;
#ifndef __aarch64__
        res = digital.get_type();
#endif
        return res;
    }

    ~CreateDigital()
    {
#ifndef __aarch64__
#endif
    }
private:
#ifndef __aarch64__
    daqmxCreateDigital digital;
#endif
};

class CreateAnalogContinuous {
public:
    CreateAnalogContinuous(const std::string device_name, analog_pins_continuous analog_io_id)
#ifndef __aarch64__
       : analog_continuous(device_name, analog_io_id)
#endif
    {

    }
    double read(pin_t analog_io_id)
    {
        double res = 0.0;
#ifndef __aarch64__
        res = analog_continuous.read(analog_io_id);
#endif
        return res;
    }
    bool write(pin_t analog_io_id, Frequency_hz<double> rate)
    {
        bool res = false;
#ifndef __aarch64__
        res = analog_continuous.write(analog_io_id, rate);
#endif
        return res;
    }
    bool stop(pin_t analog_io_id)
    {
        bool res = false;
#ifndef __aarch64__
        res = analog_continuous.stop(analog_io_id);
#endif
        return res;
    }
    void close()
    {
#ifndef __aarch64__
        analog_continuous.close();
#endif
    }
    DAQ_CO_CREATE_STATUS get_status()
    {
        DAQ_CO_CREATE_STATUS res = DAQ_CO_CREATE_STATUS::INPUTS_NOT_VALID;
#ifndef __aarch64__
        res = analog_continuous.get_status();
#endif
        return res;
    }

    IO_TYPE get_type()
    {
        IO_TYPE res = IO_TYPE::unknown;
#ifndef __aarch64__
        res = analog_continuous.get_type();
#endif
        return res;
    }

    ~CreateAnalogContinuous()
    {
#ifndef __aarch64__
#endif
    }
private:
#ifndef __aarch64__
    daqmxCreateAnalogContinuous analog_continuous;
#endif
};

class CreateDigitalContinuous {
public:
    CreateDigitalContinuous(const std::string device_name, digital_pins_continuous digital_io_id)
#ifndef __aarch64__
       : digital_continuous(device_name, digital_io_id)
#endif
    {

    }
    double read(pin_t digital_io_id)
    {
        double res = 0.0;
#ifndef __aarch64__
        res = digital_continuous.read(digital_io_id);
#endif
        return res;
    }
    bool write(pin_t digital_io_id, Frequency_hz<double> rate)
    {
        bool res = false;
#ifndef __aarch64__
        res = digital_continuous.write(digital_io_id, rate);
#endif
        return res;
    }
    bool stop(pin_t digital_io_id)
    {
        bool res = false;
#ifndef __aarch64__
        res = digital_continuous.stop(digital_io_id);
#endif
        return res;
    }
    void close()
    {
#ifndef __aarch64__
        digital_continuous.close();
#endif
    }
    DAQ_CO_CREATE_STATUS get_status()
    {
        DAQ_CO_CREATE_STATUS res = DAQ_CO_CREATE_STATUS::INPUTS_NOT_VALID;
#ifndef __aarch64__
        res = digital_continuous.get_status();
#endif
        return res;
    }

    IO_TYPE get_type()
    {
        IO_TYPE res = IO_TYPE::unknown;
#ifndef __aarch64__
        res = digital_continuous.get_type();
#endif
        return res;
    }

    ~CreateDigitalContinuous()
    {
#ifndef __aarch64__
#endif
    }
private:
#ifndef __aarch64__
    daqmxCreateDigitalContinuous digital_continuous;
#endif
};