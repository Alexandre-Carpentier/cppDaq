/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     06/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef _WIN32
#define DAQAPI __declspec(dllexport)		
#else
#define DAQAPI
#endif
#include "stdlib.h"
#include "stdio.h"
#include "cppDaq.h"

#ifdef __cplusplus
extern "C" {
#endif

// This is the C interface
// This interface IS unsafe. No check on the inputs is done. Use at your own risk.
// For strong type implementations and safety use the cpp interface.
// This header only provide ABI compatibility and is considered deprecated for new dev.

// Here values can be swaped, missused, etc.

// Opaque

typedef void* HDAQ;

	// Helper functions
	// __declspec(dllexport)char* get_ni_device_list(int *itemRead);
 
	// To instanciate: pass vec with desired pin. Ex: 1,3,7 to enable pin 1, 3 and 7. Must not be > to MAX_ANALOG_CHANNELS or MAX_DIGITAL_CHANNELS
	DAQAPI HDAQ create_cppDaq(const char* device,
		const int analog_count, int analog_io_id[32],
		const int digital_count, int digital_io_id[32],
		const int analog_co_count, int analog_co_io_id[32],
		const int digital_co_count, int digital_co_io_id[32]);
	//
	// 
	// Return current status.
	DAQAPI int get_status_(HDAQ);

	// Return a volt value
	DAQAPI double get_analog_value_(HDAQ daq, int pin);
	//
	// Set pin to HIGH or LOW logic
	DAQAPI bool set_digital_value_(HDAQ handle, int pin, int state);

	// Set volt value
	DAQAPI bool set_analog_value_(HDAQ handle, int pin, double value);
	//

	// PWM from counter hardware
	DAQAPI bool start_digital_pulse_(HDAQ handle, int pin, int frequency);
	DAQAPI bool stop_digital_pulse_(HDAQ handle);

	// PWM from software emulation (not precise)
	DAQAPI bool start_digital_software_pulse_(HDAQ handle, int pin, int frequency);
	DAQAPI bool stop_digital_software_pulse_(HDAQ handle);
	DAQAPI bool update_digital_software_pulse_(HDAQ handle, int frequency);

	// Set a PWM clock on selected pin
	DAQAPI bool start_analog_pulse_(HDAQ handle, int pin, int frequency);

	// Set a PWM clock on selected pin
	DAQAPI bool stop_analog_pulse_(HDAQ handle, int pin);

	//
	DAQAPI bool destroy_cppDaq_(HDAQ handle);

#ifdef __cplusplus
}
#endif