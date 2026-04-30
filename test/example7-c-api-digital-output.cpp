/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     02/12/25
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
	std::print(CYN"[ EXAMPLE 7] Testing analog PWM on AO0 and Digital output on port1/line0...\n");
	std::print(NC);

	char dev[] = "Dev2";

	// Pin affected
	int di[] = {0};

	// Create a daq object with 1 digital output
	HDAQ daq = create_cppDaq(dev, 0, NULL, 0, di, 1, NULL, 0, NULL);
	if (!daq)
	{
		return EXIT_FAILURE;
	}

	// Set a digital 1 logic on pin 0.
	set_digital_value_(daq, 0, 1); 

	// Do other things
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// Clean up daq object
	destroy_cppDaq_(daq);

	std::println(NC);
	return EXIT_SUCCESS;
}