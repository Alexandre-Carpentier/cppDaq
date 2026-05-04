/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     06/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once

#define DAQ_API_VERSION "0.8.0"
#ifdef WIN32
#define DAQAPI __declspec(dllexport)		
#else
#define DAQAPI
#endif

#include <memory>
#include <vector>
#include <string>
#include <array>
#include <type_traits>
#include <utility>

// Strong types for pin identification
#include "physic_types.h"
#include "io_types.h"

// Define signals boundaries
// Adapt according to your DAQ device capabilities
const static short MAX_ANALOG_INPUT_CHANNELS = 8;
const static short MAX_ANALOG_OUTPUT_CHANNELS = 2;
const static short MAX_DIGITAL_CHANNELS = 8;

// Define logic pin states
enum class digital_state { LOW = 0, HIGH };

// Define error code. call get_status() to retrieve current status of a cppDaq object.
enum class DAQ_STATUS
{
	NO_ERR = 0,
	INPUTS_NOT_VALID, // Occurs when supplied index out of hardware bounds, ...
	DAQ_RETRIEVE_NAME_FAIL, // The device name could not be retrieved.
	DAQ_TEST_DEVICE_FAIL, // The device could not be accessed.
	DAQ_NAME_EMPTY, // The device name is empty and can't be retrieved automatically.
	ERR_CREATE_IO_FAIL, // Failed to create I/O channels (Task,...).
	DAQ_NOT_INITIALIZED // The DAQ device has not been initialized properly and do not exist in memory.
};

// Named parameters ctor
// 
// Usage example:
// 
// DaqConfig config; 
// config.setDevice("MyDevice")
//    .setAnalogPins({0, 1, 2})
//    .setDigitalPins({0, 1})
//    .setAnalogContinuous({0})
//    .setDigitalContinuous({0});
// 
// cppDaq daq(config);
// Thanks cpp >20. This is better than the old builder pattern (more readable)

struct DaqConfig
{
    std::string device = "";
    analog_pins analog ;
    digital_pins digital;
    analog_pins_continuous analog_continuous;
    digital_pins_continuous digital_continuous;

    // Constructeur par d�faut n�cessaire
    DaqConfig()
        : analog(analog_pins{})
        , digital(digital_pins{})
        , analog_continuous(analog_pins_continuous{})
        , digital_continuous(digital_pins_continuous{})
    {
    }

    DaqConfig& setDevice(std::string dev) {
        device = std::move(dev);
        return *this;
    }

    DaqConfig& setAnalogPins(analog_pins pins) {
        analog = std::move(pins);
        return *this;
    }

    DaqConfig& setDigitalPins(digital_pins pins) {
        digital = std::move(pins);
        return *this;
    }

    DaqConfig& setAnalogContinuous(analog_pins_continuous pins) {
        analog_continuous = std::move(pins);
        return *this;
    }

    DaqConfig& setDigitalContinuous(digital_pins_continuous pins) {
        digital_continuous = std::move(pins);
        return *this;
    }
};

// Interface API
class DAQAPI cppDaq
{
public:
    // with named parameters
    explicit cppDaq(const DaqConfig& config);

	// Old constructor (deprecated)
    [[deprecated]]
    explicit cppDaq(const std::string device,
        const analog_pins ans,
        const digital_pins dis,
        const analog_pins_continuous ans_co,
        const digital_pins_continuous dis_co);

    cppDaq(const cppDaq&) = delete;
    cppDaq& operator=(const cppDaq&) = delete;
    cppDaq(cppDaq&&) noexcept;
    cppDaq& operator=(cppDaq&&) noexcept;

    // Status & diagnostics
    [[nodiscard]] DAQ_STATUS GetStatus(); 

    // Analog I/O operations
    [[nodiscard]] double ReadAnalogPin(analog_pin pin); 
    [[nodiscard]] std::array<double, 1000> ReadMultipleAnalogPin(analog_pin pin); 
    bool WriteAnalogPin(analog_pin pin, double value); 

    // Digital I/O operations
    bool WriteDigitalPin(digital_pin pin, digital_state state); 

    // Digital PWM control
    [[nodiscard]] bool StartDigitalCounterPWM(digital_pin_continuous pin, Frequency_hz<double> rate); 
    bool StopDigitalCounterPWM(); 

    [[nodiscard]] bool StartDigitalSoftwarePWM(digital_pin_continuous pin, Frequency_hz<double> rate); 
    bool StopDigitalSoftwarePWM(); 
    bool UpdateDigitalSoftwarePWM(Frequency_hz<double> rate); 

    // Analog PWM control
    [[nodiscard]] bool StartAnalogPWM(analog_pin_continuous pin, Frequency_hz<double> rate); 
    bool StopAnalogPWM(analog_pin_continuous pin); 

    bool resetDaq();
    ~cppDaq();

private:
    //Opaque ptr
    class cppDaqimpl; 
    std::unique_ptr<cppDaqimpl> pdaq;

    DaqConfig m_config;
    DAQ_STATUS status;
};

// Helper functions
// To retrieve list of connected devices
std::vector<std::string> GetDeviceList();