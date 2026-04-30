/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     13/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef WIN32
#include <NIDAQmx.h>
#endif
#include "cppDaq.h" 
#include <vector>
#include <variant>

// Factory types
enum class writer_type
{
    sim = 0,
    digital_real,
	digital_real_co,
    analog_real_co
};

// Sim mock
struct cDigitalWriteSim final {
    #ifndef __aarch64__
    bool write(TaskHandle m_digital_task_handle, digital_state state);
    #endif
};

// Real impl
struct cDigitalWrite final {
    #ifndef __aarch64__
    bool write(TaskHandle m_digital_task_handle, digital_state state);
    #endif
};

// Real impl
struct cDigitalDaqWriteCO final {
    #ifndef __aarch64__
    bool write(TaskHandle m_digital_task_handle, Frequency_hz<double> rate);
    #endif
};

// Real impl
struct cAnalogDaqWriteCO final {
    #ifndef __aarch64__
    bool write(TaskHandle m_digital_task_handle, Frequency_hz<double> rate);
    #endif
};

// Static polymorphic types here
// must be pass to std::visit
using writer_single = std::variant< cDigitalWriteSim, cDigitalWrite>; // Sim, digital, (analog not implemented)

using writer_continuous = std::variant<cDigitalDaqWriteCO, cAnalogDaqWriteCO>; // digital continuous, analog continuous

// vector factory
std::vector<writer_single> factory_writers(writer_type choice);

// elem factory
writer_single factory_writer_single(writer_type choice);

// elem factory
writer_continuous factory_writer_co(writer_type choice);
