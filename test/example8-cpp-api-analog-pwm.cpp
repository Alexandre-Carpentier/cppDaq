/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     24/01/26
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include <print>
#include "daq_c.h"
#include "term_colors.h"
#include <stdio.h>
#include <chrono>
#include <thread>

int main()
{
	std::print(CYN"[ EXAMPLE 8] Testing C++ PWM api...\n"); std::print(NC);
	
	// Retrieve connected device list
	std::string dev = GetDeviceList().at(0);
	
	// Describe pin
	analog_pin_continuous an0(0);

	// set sine freq
	// Max 39kHz with R208
	Frequency_hz<double> frequency(200.0);

	// Create config with a named parameter list
	auto config = DaqConfig{}
		.setDevice(dev)
		.setAnalogContinuous(analog_pins_continuous{ an0 }
		);

	// instantiate
	auto gbf = cppDaq(config);

	// Test pwm start stop in loop
	size_t loop_count = 100;
	size_t sleep_duration = 1;
	for (size_t n = 0; n < loop_count; ++n)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));

		//Activate PWM on AO0 at 1kHz
		if (!gbf.StartAnalogPWM(an0, frequency))
		{
			std::print(RED"[!] Error start_analog_pwm()\n");
			return EXIT_FAILURE;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));

		// Stop PWM on AO0
		gbf.StopAnalogPWM(an0);
		
	}
	return 0;
}