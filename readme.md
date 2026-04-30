# cppDaq - HowTo Guide

**A modern C++ and C API for NI DAQ hardware abstraction**

[![C++23](https://img.shields.io/badge/C++-23-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C.svg)](https://cmake.org/)

---

## 📚 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Build Instructions](#build-instructions)
- [C++ API (Recommended)](#c-api-cppdaq)
- [C API (Compatibility)](#c-api-daq_ch)
- [Best Practices](#best-practices)

---

## 🎯 Overview

This document explains how to use the **cppDaq** library with two distinct interfaces:

| Interface | Description | Use Case |
|-----------|-------------|----------|
| **C++ API** (`cppDaq`) | ✅ Recommended interface with strong type safety | Modern C++ projects |
| **C API** (`daq_c.h`) | ⚠️ ABI-compatible but less safe | C code, FFI, or legacy integration |

---

## ✨ Features

The **cppDaq** library provides a comprehensive NI DAQ abstraction with:

- 📊 **Analog I/O**: Read and write analog signals
- 🔌 **Digital I/O**: Control digital pins with HIGH/LOW states
- 📡 **PWM Generation**:
  - Hardware-based digital PWM (high precision)
  - Software-emulated digital PWM (flexible)
  - Analog PWM generation
- 🔍 **Device Management**: List and select available DAQ devices
- ⚡ **Status Monitoring**: Real-time error checking and validation

---

## 🛠️ Build Instructions

### Prerequisites

- **CMake** ≥ 3.20
- **C++23** compatible compiler
- **NI-DAQmx** drivers installed

### Build Steps

```bash
# 1. Create build directory
mkdir build && cd build

# 2. Configure CMake
cmake ..

# 3. Build the library
make
```

### Output

- **Windows**: `cppDaq.dll`
- **Linux**: `libcppDaq.so`

> 💡 **Note**: The C++ API uses **strong types** (`analog_pin`, `digital_pin`, `Frequency_hz<double>`, etc.) to prevent manipulation errors at compile-time.

---

## 🚀 C++ API (`cppDaq`)

### Prerequisites

Before using the C++ API, ensure you have:

1. ✅ Included the `cppDaq` header
2. ✅ Linked the `cppDaq` library and NI-DAQmx dependencies
3. ✅ Verified device availability using `GetDeviceList()`

---

### 📝 Quick Start Example

```cpp
#include "cppDaq"
#include <iostream>

int main()
{
    // 1. Detect available devices
    auto devices = GetDeviceList();
    if (devices.empty()) {
        std::cerr << "❌ No device detected\n";
        return 1;
    }

    // 2. Configure the DAQ with fluent API
    DaqConfig config;
    config.withDevice(devices[0])
          .withAnalogPins({0})
          .withDigitalPins({0})
          .withAnalogContinuous({})
          .withDigitalContinuous({0});

    // 3. Initialize the DAQ
    cppDaq daq(config);

    // 4. Verify initialization
    if (daq.GetStatus() != DAQ_STATUS::NO_ERR) {
        std::cerr << "❌ DAQ initialization error\n";
        return 1;
    }

    // 5. Read analog input
    const double voltage = daq.ReadAnalogPin(analog_pin(0));
    std::cout << "📊 AI0 = " << voltage << " V\n";

    // 6. Write digital output
    daq.WriteDigitalPin(digital_pin(0), digital_state::HIGH);
    std::cout << "✅ Digital pin 0 set to HIGH\n";

    return 0;
}
```

---

### 🔑 Important Types

| Type | Description |
|------|-------------|
| `analog_pin` | Strong type for analog pin identifiers |
| `digital_pin` | Strong type for digital pin identifiers |
| `analog_pin_continuous` | Analog pin for continuous/streaming operations |
| `digital_pin_continuous` | Digital pin for continuous/streaming operations |
| `analog_pins` | Collection of analog pins for configuration |
| `digital_pins` | Collection of digital pins for configuration |
| `Frequency_hz<double>` | Type-safe frequency representation in Hertz |
| `digital_state::{LOW, HIGH}` | Enumeration for digital logic states |

---

### 📖 Complete Function Reference

#### 🔍 Device Discovery

##### `GetDeviceList()`

```cpp
std::vector<std::string> GetDeviceList()
```

Retrieves all detected DAQ device names.

| | |
|---------|----------|
| **Input** | None |
| **Output** | List of device names (e.g., `"Dev1"`, `"Dev2"`) |
| **Usage** | Choose the device to pass to `DaqConfig::withDevice()` |

**Example:**
```cpp
auto devices = GetDeviceList();
for (const auto& dev : devices) {
    std::cout << "Found device: " << dev << "\n";
}
```

---

#### 🏗️ Construction

##### `cppDaq(const DaqConfig& config)` ⭐ Recommended

```cpp
cppDaq::cppDaq(const DaqConfig& config)
```

Constructs a DAQ instance using the modern configuration API.

| | |
|---------|----------|
| **Input** | `DaqConfig` object (device + pin configurations) |
| **Output** | `cppDaq` instance |
| **Note** | ⚠️ Always call `GetStatus()` after construction |

**Example:**
```cpp
DaqConfig config;
config.withDevice("Dev1")
      .withAnalogPins({0, 1})
      .withDigitalPins({0, 1, 2});

cppDaq daq(config);
if (daq.GetStatus() != DAQ_STATUS::NO_ERR) {
    // Handle error
}
```

---

##### `cppDaq(...)` ⚠️ Deprecated

```cpp
cppDaq::cppDaq(const std::string, const analog_pins, 
               const digital_pins, const analog_pins_continuous, 
               const digital_pins_continuous)
```

| | |
|---------|----------|
| **Status** | ⚠️ **Deprecated** - Kept for backward compatibility |
| **Recommendation** | Use `DaqConfig` constructor instead |

---

#### ⚡ Status Checking

##### `GetStatus()`

```cpp
DAQ_STATUS cppDaq::GetStatus()
```

Returns the current operational state of the DAQ.

**Possible Return Values:**

| Status | Meaning |
|--------|---------|
| `NO_ERR` | ✅ Everything operational |
| `INPUTS_NOT_VALID` | ❌ Invalid input parameters |
| `DAQ_RETRIEVE_NAME_FAIL` | ❌ Failed to retrieve device name |
| `DAQ_TEST_DEVICE_FAIL` | ❌ Device test failed |
| `DAQ_NAME_EMPTY` | ❌ Device name is empty |
| `ERR_CREATE_IO_FAIL` | ❌ I/O creation failed |
| `DAQ_NOT_INITIALIZED` | ❌ DAQ not properly initialized |

---

#### 📊 Analog Input

##### `ReadAnalogPin()`

```cpp
double cppDaq::ReadAnalogPin(analog_pin pin)
```

Reads the voltage from a single analog input.

| | |
|---------|----------|
| **Input** | `analog_pin` - Pin identifier |
| **Output** | Voltage in volts (`double`) |
| **Precondition** | Pin must be configured in analog pins |

**Example:**
```cpp
double voltage = daq.ReadAnalogPin(analog_pin(0));
```

---

##### `ReadMultipleAnalogPin()`

```cpp
std::array<double, 1000> cppDaq::ReadMultipleAnalogPin(analog_pin pin)
```

Acquires 1000 samples from an analog input.

| | |
|---------|----------|
| **Input** | `analog_pin` - Pin identifier |
| **Output** | Array of 1000 voltage samples |
| **Usage** | Burst acquisition or waveform capture |

**Example:**
```cpp
auto samples = daq.ReadMultipleAnalogPin(analog_pin(0));
double average = std::accumulate(samples.begin(), samples.end(), 0.0) / samples.size();
```

---

#### 📤 Analog Output

##### `WriteAnalogPin()`

```cpp
bool cppDaq::WriteAnalogPin(analog_pin pin, double value)
```

Writes a voltage to an analog output.

| | |
|---------|----------|
| **Input** | `analog_pin` - Pin identifier<br>`double` - Voltage value |
| **Output** | `true` if successful, `false` otherwise |

**Example:**
```cpp
if (daq.WriteAnalogPin(analog_pin(0), 3.3)) {
    std::cout << "✅ Voltage set to 3.3V\n";
}
```

---

#### 🔌 Digital Output

##### `WriteDigitalPin()`

```cpp
bool cppDaq::WriteDigitalPin(digital_pin pin, digital_state state)
```

Sets a digital pin to HIGH or LOW.

| | |
|---------|----------|
| **Input** | `digital_pin` - Pin identifier<br>`digital_state` - `LOW` or `HIGH` |
| **Output** | `true` if successful, `false` otherwise |

**Example:**
```cpp
daq.WriteDigitalPin(digital_pin(0), digital_state::HIGH);
daq.WriteDigitalPin(digital_pin(1), digital_state::LOW);
```

---

#### 📡 Hardware PWM (Digital)

##### `StartDigitalCounterPWM()`

```cpp
bool cppDaq::StartDigitalCounterPWM(digital_pin_continuous pin, 
                                     Frequency_hz<double> rate)
```

Starts hardware-based digital PWM using a counter.

| | |
|---------|----------|
| **Input** | `digital_pin_continuous` - Pin identifier<br>`Frequency_hz<double>` - Frequency |
| **Output** | `true` if started successfully |
| **Advantage** | ⚡ High precision, hardware-timed |

**Example:**
```cpp
daq.StartDigitalCounterPWM(digital_pin_continuous(0), Frequency_hz<double>(1000.0));
```

---

##### `StopDigitalCounterPWM()`

```cpp
bool cppDaq::StopDigitalCounterPWM()
```

Stops the active hardware PWM.

| | |
|---------|----------|
| **Input** | None |
| **Output** | `true` if stopped successfully |

---

#### 💻 Software PWM (Digital)

##### `StartDigitalSoftwarePWM()`

```cpp
bool cppDaq::StartDigitalSoftwarePWM(digital_pin_continuous pin, 
                                      Frequency_hz<double> rate)
```

Starts software-emulated digital PWM.

| | |
|---------|----------|
| **Input** | `digital_pin_continuous` - Pin identifier<br>`Frequency_hz<double>` - Frequency |
| **Output** | `true` if started successfully |
| **Note** | ⚠️ Lower precision than hardware PWM |

---

##### `StopDigitalSoftwarePWM()`

```cpp
bool cppDaq::StopDigitalSoftwarePWM()
```

Stops the active software PWM.

---

##### `UpdateDigitalSoftwarePWM()`

```cpp
bool cppDaq::UpdateDigitalSoftwarePWM(Frequency_hz<double> rate)
```

Updates the frequency of a running software PWM.

| | |
|---------|----------|
| **Input** | New frequency |
| **Output** | `true` if updated successfully |
| **Note** | Must be called while PWM is active |

---

#### 🌊 Analog PWM

##### `StartAnalogPWM()`

```cpp
bool cppDaq::StartAnalogPWM(analog_pin_continuous pin, 
                             Frequency_hz<double> rate)
```

Starts periodic analog signal generation.

| | |
|---------|----------|
| **Input** | `analog_pin_continuous` - Pin identifier<br>`Frequency_hz<double>` - Frequency |
| **Output** | `true` if started successfully |

---

##### `StopAnalogPWM()`

```cpp
bool cppDaq::StopAnalogPWM(analog_pin_continuous pin)
```

Stops the periodic analog generation on the specified pin.

| | |
|---------|----------|
| **Input** | `analog_pin_continuous` - Pin identifier |
| **Output** | `true` if stopped successfully |

---

#### 🧹 Cleanup

##### `~cppDaq()`

```cpp
cppDaq::~cppDaq()
```

Destructor that automatically releases all resources.

| | |
|---------|----------|
| **Resources** | Tasks, handles, memory allocations |
| **Note** | Automatically called when object goes out of scope |

---

## ⚙️ C API (`daq_c.h`)

### ⚠️ Important Warning

The C interface provides ABI compatibility but has limitations:

| Limitation | Impact |
|------------|--------|
| ❌ No strong types | Less compile-time safety |
| ❌ Limited validation | Runtime errors more likely |
| ⚠️ Manual memory management | Risk of leaks if not careful |

**Recommended for**: C projects, FFI (Foreign Function Interface), or legacy system integration.

---

### 📝 Quick Start Example

```c
#include "daq_c.h"
#include <stdio.h>

int main(void)
{
    // 1. Configure pins
    char dev[] = "Dev1";
    int digital_pins[] = {0};

    // 2. Create DAQ instance
    HDAQ h = create_cppDaq_(
        dev,
        0, NULL,           // No analog pins
        1, digital_pins,   // 1 digital pin (pin 0)
        0, NULL,           // No continuous analog
        0, NULL            // No continuous digital
    );

    // 3. Check creation
    if (h == NULL) {
        printf("❌ Error: create_cppDaq_ failed\n");
        return 1;
    }

    // 4. Set digital pin HIGH
    if (!set_digital_value_(h, 0, 1)) {
        printf("❌ Error: set_digital_value_ failed\n");
    } else {
        printf("✅ Digital pin 0 set to HIGH\n");
    }

    // 5. Cleanup
    destroy_cppDaq_(h);
    return 0;
}
```

---

### 📖 Complete Function Reference

> **Note**: All C functions use an opaque handle `HDAQ` (which is a `void*` pointer).

---

#### 🏗️ Lifecycle Management

##### `create_cppDaq_()`

```c
HDAQ create_cppDaq_(
    char* device,
    int analog_count,
    int* analog_io_id,
    int digital_count,
    int* digital_io_id,
    int analog_co_count,
    int* analog_co_io_id,
    int digital_co_count,
    int* digital_co_io_id
)
```

Creates a DAQ instance and returns an opaque handle.

| Parameter | Description |
|-----------|-------------|
| `device` | Device name (e.g., `"Dev1"`) |
| `analog_count` | Number of analog pins |
| `analog_io_id` | Array of analog pin IDs (or `NULL` if count = 0) |
| `digital_count` | Number of digital pins |
| `digital_io_id` | Array of digital pin IDs (or `NULL` if count = 0) |
| `analog_co_count` | Number of continuous analog pins |
| `analog_co_io_id` | Array of continuous analog pin IDs |
| `digital_co_count` | Number of continuous digital pins |
| `digital_co_io_id` | Array of continuous digital pin IDs |
| **Returns** | Valid `HDAQ` handle or `NULL` on failure |

**Example:**
```c
int analog_pins[] = {0, 1};
int digital_pins[] = {0};

HDAQ h = create_cppDaq_(
    "Dev1",
    2, analog_pins,    // 2 analog pins
    1, digital_pins,   // 1 digital pin
    0, NULL,           // No continuous analog
    0, NULL            // No continuous digital
);
```

---

##### `destroy_cppDaq_()`

```c
bool destroy_cppDaq_(HDAQ handle)
```

Destroys the DAQ instance and releases all resources.

| | |
|---------|----------|
| **Input** | `HDAQ` handle |
| **Returns** | `true` if successful |
| **Note** | ⚠️ **Always call** at the end of usage to prevent memory leaks |

---

#### ⚡ Status Checking

##### `get_status_()`

```c
int get_status_(HDAQ handle)
```

Returns the current status code of the DAQ instance.

**Status Code Mapping:**

| Code | Status Constant | Meaning |
|------|-----------------|---------|
| `0` | `NO_ERR` | ✅ Normal operation |
| `1` | `INPUTS_NOT_VALID` | ❌ Invalid input parameters |
| `2` | `DAQ_RETRIEVE_NAME_FAIL` | ❌ Failed to retrieve device name |
| `3` | `DAQ_TEST_DEVICE_FAIL` | ❌ Device test failed |
| `4` | `DAQ_NAME_EMPTY` | ❌ Empty device name |
| `5` | `ERR_CREATE_IO_FAIL` | ❌ I/O creation failed |
| `6` | `DAQ_NOT_INITIALIZED` | ❌ DAQ not initialized |

**Example:**
```c
int status = get_status_(h);
if (status != 0) {
    printf("DAQ error: %d\n", status);
}
```

---

#### 📊 Analog Operations

##### `get_analog_value_()`

```c
double get_analog_value_(HDAQ daq, int pin)
```

Reads the voltage from an analog input pin.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, pin index |
| **Returns** | Voltage in volts (`double`) |

**Example:**
```c
double voltage = get_analog_value_(h, 0);
printf("Analog pin 0: %.2f V\n", voltage);
```

---

##### `set_analog_value_()`

```c
bool set_analog_value_(HDAQ handle, int pin, double value)
```

Writes a voltage to an analog output pin.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, pin index, voltage value |
| **Returns** | `true` if successful |

**Example:**
```c
if (set_analog_value_(h, 0, 3.3)) {
    printf("✅ Set analog pin 0 to 3.3V\n");
}
```

---

#### 🔌 Digital Operations

##### `set_digital_value_()`

```c
bool set_digital_value_(HDAQ handle, int pin, int state)
```

Writes a digital state to a pin.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, pin index, state (`0` = LOW, `1` = HIGH) |
| **Returns** | `true` if successful |

**Example:**
```c
set_digital_value_(h, 0, 1);  // Set HIGH
set_digital_value_(h, 1, 0);  // Set LOW
```

---

#### 📡 Hardware PWM (Digital)

##### `start_digital_pulse_()`

```c
bool start_digital_pulse_(HDAQ handle, int pin, int frequency)
```

Starts hardware-based digital PWM.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, pin index, frequency in Hz |
| **Returns** | `true` if started successfully |

**Example:**
```c
start_digital_pulse_(h, 0, 1000);  // 1 kHz PWM
```

---

##### `stop_digital_pulse_()`

```c
bool stop_digital_pulse_(HDAQ handle)
```

Stops the hardware digital PWM.

---

#### 💻 Software PWM (Digital)

##### `start_digital_software_pulse_()`

```c
bool start_digital_software_pulse_(HDAQ handle, int pin, int frequency)
```

Starts software-emulated digital PWM.

| | |
|---------|----------|
| **Note** | ⚠️ Lower precision than hardware PWM |

---

##### `stop_digital_software_pulse_()`

```c
bool stop_digital_software_pulse_(HDAQ handle)
```

Stops the software digital PWM.

---

##### `update_digital_software_pulse_()`

```c
bool update_digital_software_pulse_(HDAQ handle, int frequency)
```

Updates the frequency of an active software PWM.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, new frequency in Hz |
| **Returns** | `true` if successful |

---

#### 🌊 Analog PWM

##### `start_analog_pulse_()`

```c
bool start_analog_pulse_(HDAQ handle, int pin, int frequency)
```

Starts periodic analog signal generation.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, pin index, frequency in Hz |
| **Returns** | `true` if successful |

---

##### `stop_analog_pulse_()`

```c
bool stop_analog_pulse_(HDAQ handle, int pin)
```

Stops the periodic analog signal on the specified pin.

| | |
|---------|----------|
| **Input** | `HDAQ` handle, pin index |
| **Returns** | `true` if successful |

---

## ✅ Best Practices

### General Recommendations

| Practice | Reason |
|----------|--------|
| ✅ **Prefer C++ API** | Type safety prevents common errors at compile-time |
| ✅ **Check status after creation** | Use `GetStatus()` or `get_status_()` immediately |
| ✅ **Verify return values** | All write/PWM operations return `bool` - check them! |
| ✅ **Stop PWMs before cleanup** | Explicitly stop all PWM signals before destroying DAQ |
| ✅ **Use `DaqConfig`** | Modern fluent API is clearer than old constructor |

---

### Error Handling Pattern (C++)

```cpp
// ✅ Good: Always check status
cppDaq daq(config);
if (daq.GetStatus() != DAQ_STATUS::NO_ERR) {
    std::cerr << "Initialization failed\n";
    return;
}

// ✅ Good: Verify write operations
if (!daq.WriteDigitalPin(digital_pin(0), digital_state::HIGH)) {
    std::cerr << "Failed to write digital pin\n";
}
```

---

### Resource Management Pattern (C)

```c
// ✅ Good: Always cleanup
HDAQ h = create_cppDaq_(...);
if (h == NULL) {
    return 1;
}

// ... use DAQ ...

// Stop PWM before destroying
stop_digital_pulse_(h);

// Always destroy
destroy_cppDaq_(h);
```

---

### PWM Management

```cpp
// ✅ Good: Stop before switching modes
daq.StopDigitalCounterPWM();
daq.StartDigitalSoftwarePWM(digital_pin_continuous(0), Frequency_hz<double>(500.0));

// ✅ Good: Stop before destruction
daq.StopDigitalSoftwarePWM();
// Destructor will run automatically
```

---

## 📞 Support

For issues, questions, or contributions, please refer to the project repository.

---

**Made with ❤️ for NI DAQ automation**
