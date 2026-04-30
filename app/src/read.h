/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     12/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef WIN32
#include <NIDAQmx.h>
#endif
#include <vector>
#include <variant>

// Factory types
enum class reader_type
{
    sim = 0,
    real
};

// Sim mock
struct cAnalogReadSim final {
    #ifndef __aarch64__
    double read(TaskHandle m_analog_task_handle);
    #endif
};

// Real impl
struct cAnalogRead final {
    #ifndef __aarch64__
    double read(TaskHandle m_analog_task_handle);
    #endif
};

// Static polymorphic types here
// must be pass to std::visit
using reader = std::variant< cAnalogReadSim, cAnalogRead>;

// vector factory
std::vector<reader> factory_readers(reader_type choice);

// elem factory
reader factory_reader(reader_type choice);
