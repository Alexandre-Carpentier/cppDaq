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
	std::print(CYN"[ EXAMPLE 6] Testing C PWM api...\n");
	std::print(NC);
	char dev[] = "Dev1";

	// Pin affected
	int an_co[1] = { 0 };

	// Create a daq object with 1 analog continuous output (an_co)
	// Other pin must be set to 0 and list to nullptr if not used.
	HDAQ daq = create_cppDaq(dev, 0, NULL, 0, NULL, 1, an_co, 0, NULL);
	if (!daq)
	{
		return EXIT_FAILURE;
	}

	// Start a pulse on pin 0 at 500Hz.
	start_analog_pulse_(daq, 0, 500);

	// Do other things
	for (int i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// Stop the pulse properly
	stop_analog_pulse_(daq, 0);

	// Clean up daq object
	destroy_cppDaq_(daq);

	std::println(NC);
	return EXIT_SUCCESS;
}