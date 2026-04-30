= cppDaq HowTo (C++ and C)
:toc:
:toclevels: 3
:sectnums:

This document explains how to use:

- the C++ API via `cppDaq` (recommended interface),
- the C API via `daq_c.h` (ABI compatible, but less safe).

== Overview

The project exposes a NI DAQ abstraction with:

- analog reading,
- analog and digital writing,
- digital PWM generation (hardware and software),
- analog PWM generation,
- status retrieval and device list.

== Build

- The project is built as a shared library (`cppDaq.dll` on Windows, `libcppDaq.so` on Linux).
- The C++ API is the primary interface, while the C API is provided for compatibility and interoperability with C code or other languages that can call C functions.
- Use CMAKE:
- `mkdir build && cd build && cmake ..` to configure cmake.
- `make` to build the library.



The C++ API uses *strong types* (`analog_pin`, `digital_pin`, `Frequency_hz<double>`, etc.) to limit manipulation errors.

== C++ API (`cppDaq`)

=== Prerequisites

- Include `cppDaq`.
- Link the `cppDaq` library and NI-DAQmx dependencies.
- Verify that the NI device is present (via `GetDeviceList()`).

=== Minimal C++ Example

[source,cpp]
----
#include "cppDaq"
#include <iostream>

int main()
{
	auto devices = GetDeviceList();
	if (devices.empty()) {
		std::cerr << "No device detected\n";
		return 1;
	}

	DaqConfig config;
	config.withDevice(devices[0])
		  .withAnalogPins({0})
		  .withDigitalPins({0})
		  .withAnalogContinuous({})
		  .withDigitalContinuous({0});

	cppDaq daq(config);

	if (daq.GetStatus() != DAQ_STATUS::NO_ERR) {
		std::cerr << "DAQ initialization error\n";
		return 1;
	}

	const double v = daq.ReadAnalogPin(analog_pin(0));
	std::cout << "AI0 = " << v << " V\n";

	daq.WriteDigitalPin(digital_pin(0), digital_state::HIGH);
	return 0;
}
----

=== Important Types (C++)

- `analog_pin`, `digital_pin`, `analog_pin_continuous`, `digital_pin_continuous`: typed pin identifiers.
- `analog_pins`, `digital_pins`, `analog_pins_continuous`, `digital_pins_continuous`: pin lists for configuration.
- `Frequency_hz<double>`: frequency in hertz for PWM functions.
- `digital_state::{LOW,HIGH}`: logic state for digital output.

=== Complete C++ Function Reference

==== `std::vector<std::string> GetDeviceList()`

Retrieves the list of detected DAQ device names.

- Input: none.
- Output: list of names (e.g., `Dev1`).
- Typical usage: choose the device to pass to `DaqConfig::withDevice()`.

==== `cppDaq::cppDaq(const DaqConfig& config)`

Constructs a DAQ instance with named configuration (recommended API).

- Input: `DaqConfig` (device + analog/digital pins + continuous pins).
- Output: `cppDaq` object.
- Verification: call `GetStatus()` right after construction.

==== `cppDaq::cppDaq(const std::string, const analog_pins, const digital_pins, const analog_pins_continuous, const digital_pins_continuous)`

Old constructor (deprecated), kept for compatibility.

- Functional equivalent: use `DaqConfig`.
- Recommendation: do not use in new code.

==== `DAQ_STATUS cppDaq::GetStatus()`

Returns the current state of the DAQ object.

Possible values:

- `NO_ERR`
- `INPUTS_NOT_VALID`
- `DAQ_RETRIEVE_NAME_FAIL`
- `DAQ_TEST_DEVICE_FAIL`
- `DAQ_NAME_EMPTY`
- `ERR_CREATE_IO_FAIL`
- `DAQ_NOT_INITIALIZED`

==== `double cppDaq::ReadAnalogPin(analog_pin pin)`

Reads the voltage of an analog input.

- Input: analog pin (`analog_pin`).
- Output: voltage in volts (`double`).
- Precondition: the pin must be part of the analog configuration.

==== `std::array<double, 1000> cppDaq::ReadMultipleAnalogPin(analog_pin pin)`

Reads a buffer of 1000 samples on an analog input.

- Input: analog pin.
- Output: fixed array of 1000 values (`double`).
- Usage: short multi-sample acquisition.

==== `bool cppDaq::WriteAnalogPin(analog_pin pin, double value)`

Writes a voltage to an analog output.

- Inputs: analog pin + value in volts.
- Output: `true` if successful, otherwise `false`.

==== `bool cppDaq::WriteDigitalPin(digital_pin pin, digital_state state)`

Writes a logic state to a digital output.

- Inputs: digital pin + state (`LOW`/`HIGH`).
- Output: `true` if successful, otherwise `false`.

==== `bool cppDaq::StartDigitalCounterPWM(digital_pin_continuous pin, Frequency_hz<double> rate)`

Starts a digital PWM based on hardware counter.

- Inputs: continuous digital pin + frequency.
- Output: `true` if start successful.
- Advantage: better time precision than software PWM.

==== `bool cppDaq::StopDigitalCounterPWM()`

Stops the previously started hardware digital PWM.

- Input: none.
- Output: `true` if stop successful.

==== `bool cppDaq::StartDigitalSoftwarePWM(digital_pin_continuous pin, Frequency_hz<double> rate)`

Starts a software-emulated digital PWM.

- Inputs: continuous digital pin + frequency.
- Output: `true` if start successful.
- Limitation: lower precision than hardware PWM.

==== `bool cppDaq::StopDigitalSoftwarePWM()`

Stops the software digital PWM.

- Input: none.
- Output: `true` if stop successful.

==== `bool cppDaq::UpdateDigitalSoftwarePWM(Frequency_hz<double> rate)`

Updates the frequency of the already active software digital PWM.

- Input: new frequency.
- Output: `true` if update successful.

==== `bool cppDaq::StartAnalogPWM(analog_pin_continuous pin, Frequency_hz<double> rate)`

Starts a periodic analog output (analog PWM mode).

- Inputs: continuous analog pin + frequency.
- Output: `true` if successful.

==== `bool cppDaq::StopAnalogPWM(analog_pin_continuous pin)`

Stops the periodic analog generation on the given pin.

- Input: continuous analog pin.
- Output: `true` if stop successful.

==== `cppDaq::~cppDaq()`

Destructor: releases internal resources (tasks, handle, memory, etc.).

== C API (`daq_c.h`)

=== Important Warning

The C interface is marked as less safe in the header:

- no strong types,
- more limited input validation,
- recommended usage mainly for ABI/C compatibility.

=== Minimal C Example

[source,c]
----
#include "daq_c.h"
#include <stdio.h>

int main(void)
{
	char dev[] = "Dev1";
	int digital_pins[] = {0};

	HDAQ h = create_cppDaq_(
		dev,
		0, NULL,
		1, digital_pins,
		0, NULL,
		0, NULL
	);

	if (h == NULL) {
		printf("Error create_cppDaq_\n");
		return 1;
	}

	if (!set_digital_value_(h, 0, 1)) {
		printf("Error set_digital_value_\n");
	}

	destroy_cppDaq_(h);
	return 0;
}
----

=== Complete C Function Reference

The C functions manipulate an opaque handle `HDAQ` (`void*`).

==== `HDAQ create_cppDaq_(char* device, int analog_count, int* analog_io_id, int digital_count, int* digital_io_id, int analog_co_count, int* analog_co_io_id, int digital_co_count, int* digital_co_io_id)`

Creates a DAQ instance and returns a handle.

- `device`: device name (e.g., `Dev1`).
- `*_count`: number of elements in each array.
- `*_io_id`: pin index arrays (can be `NULL` if count = 0).
- Return: valid handle or `NULL` if failed.

==== `int get_status_(HDAQ handle)`

Returns the current status of the instance.

- Return: integer code corresponding to `DAQ_STATUS`.

Code mapping:

- `0`: `NO_ERR`
- `1`: `INPUTS_NOT_VALID`
- `2`: `DAQ_RETRIEVE_NAME_FAIL`
- `3`: `DAQ_TEST_DEVICE_FAIL`
- `4`: `DAQ_NAME_EMPTY`
- `5`: `ERR_CREATE_IO_FAIL`
- `6`: `DAQ_NOT_INITIALIZED`

==== `double get_analog_value_(HDAQ daq, int pin)`

Reads the analog value of a pin.

- Inputs: handle + pin index.
- Output: voltage (`double`).

==== `bool set_digital_value_(HDAQ handle, int pin, int state)`

Writes a digital value.

- `state`: `0` = LOW, `1` = HIGH.
- Return: `true` if successful.

==== `bool set_analog_value_(HDAQ handle, int pin, double value)`

Writes an analog voltage.

- Inputs: handle + pin + voltage.
- Return: `true` if successful.

==== `bool start_digital_pulse_(HDAQ handle, int pin, int frequency)`

Starts a hardware digital PWM (counter).

- `frequency`: frequency in Hz.
- Return: `true` if successful.

==== `bool stop_digital_pulse_(HDAQ handle)`

Stops the hardware digital PWM.

- Return: `true` if successful.

==== `bool start_digital_software_pulse_(HDAQ handle, int pin, int frequency)`

Starts a software-emulated digital PWM.

- Return: `true` if successful.

==== `bool stop_digital_software_pulse_(HDAQ handle)`

Stops the software digital PWM.

- Return: `true` if successful.

==== `bool update_digital_software_pulse_(HDAQ handle, int frequency)`

Updates the frequency of the software digital PWM.

- Return: `true` if successful.

==== `bool start_analog_pulse_(HDAQ handle, int pin, int frequency)`

Starts a periodic analog signal on the indicated pin.

- Return: `true` if successful.

==== `bool stop_analog_pulse_(HDAQ handle, int pin)`

Stops the periodic analog signal on the indicated pin.

- Return: `true` if successful.

==== `bool destroy_cppDaq_(HDAQ handle)`

Destroys the instance associated with the handle and releases resources.

- Always call this function at the end of usage.

== Best Practices

- Prefer the C++ API (`cppDaq`) for type safety.
- Systematically verify the status (`GetStatus()` or `get_status_()`) after creation.
- Verify the `bool` return values of all write/PWM operations.
- Explicitly stop PWMs before destroying the handle/object.
