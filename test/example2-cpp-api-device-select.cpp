/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     19/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include <print>
#include <assert.h>
#include <vector>
#include <string>
#include "cppDaq.h"
#include "term_colors.h"

int main()
{
	std::print(CYN"[ EXAMPLE 2] Specifiying a card...\n");
	std::print(NC);

		// Retrieve connected device list
	std::string dev = "Dev1";
	std::vector<std::string> list = GetDeviceList();

		// Setup pins

	analog_pin an2(2);
	digital_pin di0(0);

	if (list.size() <2)
	{
		std::print(RED"[!] At least two DAQ devices are required for this example.\n");
		std::println(NC);
		return EXIT_FAILURE;
	}
	// Create obj for each usb daq

	cppDaq daq6001(list[0], analog_pins(an2), digital_pins(di0), analog_pins_continuous{}, digital_pins_continuous{});

	cppDaq daq6501(list[1], analog_pins(), digital_pins(di0), analog_pins_continuous{}, digital_pins_continuous{});

	// Digital pin High

	bool success = daq6501.WriteDigitalPin(di0, digital_state::HIGH);
	if (!success)
	{
		std::print(RED"[!] Failed to set digital pin\n");
		std::println(NC);
	}

	// Get some meas

	std::println(GRN"[*] Measurement: {} Volt", daq6001.ReadAnalogPin(an2));
	std::println(NC);

	return EXIT_SUCCESS;
}