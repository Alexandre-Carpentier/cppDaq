/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     20/02/26
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
	std::print(CYN"[ EXAMPLE 9] Testing C++ OPB941 IR detector...\n"); std::print(NC);

	// Retrieve connected device list
	std::string dev = GetDeviceList().at(0);

	// Describe pin
	analog_pin an0(0);

	// set sine freq
	// Max 39kHz with R208
	Frequency_hz<double> frequency(200.0);

	// Create config with a named parameter list
	auto config = DaqConfig{}
		.setDevice(dev)
		.setAnalogPins(analog_pins{ an0 }
		);

	// instantiate
	auto home_detect = cppDaq(config);

	// Test pwm start stop in loop
	size_t loop_count = 10000;
	size_t sleep_duration = 10;
	for (size_t n = 0; n < loop_count; ++n)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
		double read = home_detect.ReadAnalogPin(an0);
		if (read < 1.2)
		{
			std::print(GRN"Read value: {:.1f}\r", read);
			std::print(NC);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		else
		{
			std::print(CYN"Read value: {:.1f}\r", read);
			std::print(NC);
		}
		
	}
	return 0;
}