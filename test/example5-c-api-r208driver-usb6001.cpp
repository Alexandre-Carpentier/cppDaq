/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     01/12/25
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
	std::print(CYN"[ EXAMPLE 5] Testing with R208 and NI USB6001...\n");
	std::print(NC);
	char dev[] = "Dev1";
	int di[2] = { 0,1 };
	int an_co[1] = { 0 };

	// With the C api you must specify the number of pin for each type and the list of pin to use. 
	// If you don't want to use a type of pin, set count to 0 and list to nullptr
	HDAQ daq = create_cppDaq(dev, 0, nullptr, 2, di, 1, an_co, 0, nullptr);
	if (NULL == daq)		
	{
		std::print("[!] Error create_cppDaq_()\n");
		return EXIT_FAILURE;
	}

	// This gen a square wave at 1kHz at the Analog output
	if (false == start_analog_pulse_(daq, 0, 1000)) // Set a PWM clock on selected pin
	{
		std::print("[!] Error start_digital_pulse()\n");
		destroy_cppDaq_(daq);
		return EXIT_FAILURE;
	}

	// Set digital pin 1 to HIGH to enable the R208 driver. 
	// The R208 driver will then use the 1kHz signal from the analog output as a clock to generate the stepper motor pulses.
	set_digital_value_(daq, 1, 1); // Set HIGH on pin digital 1

	// Do other things
	for (int i = 0; i < 5; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// Stop the pulse properly
	if (!stop_analog_pulse_(daq, 0))
	{
		std::print("[!] Error stop_digital_pulse()\n");
	}

	// Clean up daq object
	destroy_cppDaq_(daq);

	std::println(NC);
	return EXIT_SUCCESS;
}