/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     21/11/25
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
	std::print(CYN"[ EXAMPLE 4] Testing the C API...\n");
	std::print(NC);
	char dev[] = "Dev1";
	int an[2] = { 0,1 };
	int di[2] = { 0,1 };
	double result = 0.0;

	// Create a daq object with 2 digital input.
	HDAQ daq = create_cppDaq(dev, 0, nullptr, 2, di, 0, nullptr, 0, nullptr);
	if(NULL == daq)
	{
		std::print("[!] Error create_cppDaq()\n");
		return EXIT_FAILURE;
	}

	// Start a digital pulse on pin 0 at 1kHz. 
	// This will gen a square wave at 1kHz on the selected digital pin (AOi).
	if (false == start_digital_pulse_(daq, 0, 1000)) // Set a PWM clock on selected pin
	{
		std::print("[!] Error start_digital_pulse()\n");
		destroy_cppDaq_(daq);
		return EXIT_FAILURE;
	}

	// Reading some analog value on pin 0 while the pulse is running.
	for (int i = 0; i < 10; i++)
	{
		std::print(GRN"\nAnalog:{:.2} V", get_analog_value_(daq, 0)); std::print(NC);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// Stop the pulse properly
	if (!stop_digital_pulse_(daq))
	{
		std::print("[!] Error stop_digital_pulse()\n");
	}

	// Clean up daq object
	destroy_cppDaq_(daq);

	std::println(NC);
	return EXIT_SUCCESS;
}