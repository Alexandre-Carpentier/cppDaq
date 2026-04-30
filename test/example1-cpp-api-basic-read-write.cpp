/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     12/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "example.h"
#include <print>
#include "cppDaq.h"
#include "term_colors.h"

int main()
{
	// Normal use

	std::print(CYN"[ EXAMPLE 1] Running...\n");
	std::print(NC);

	// Setup device

// Empty device name = select first available
	std::string device_name;

	// Setup pins

// Basic pin creation
	analog_pin an0(0);
	analog_pin an3(3);
	analog_pin an7(7);
	digital_pin di0(0);

	// Enable pin 0, 3 and 7 as analog input
	const analog_pins analogs{ an0, an3, an7 }; // On Daq usb6001 0 to 7 are available with RSE (not differential)

	// Enable pin 0 as output
	const digital_pins digitals{di0}; // On Daq usb6001 P0.<0..7> + P1.<0..3> + P2.0>are available

	// Create obj

	auto daq = cppDaq(DaqConfig{}
		.setDevice("Dev1")
		.setAnalogPins(analogs)
		.setDigitalPins(digitals)
	);


	// Digital pin High

	bool success = daq.WriteDigitalPin(di0, digital_state::HIGH);
	if (!success)
	{
		std::print(RED"[!] Failed to set digital pin\n");
		std::println(NC);
	}

	// Get some meas

	std::println(GRN"[*] Measurement on An0: {} V", daq.ReadAnalogPin(an0)); std::println(NC);
	std::println(GRN"[*] Measurement on An3: {} V", daq.ReadAnalogPin(an3)); std::println(NC);
	std::println(GRN"[*] Measurement on An7: {} V", daq.ReadAnalogPin(an7)); std::println(NC);

	return EXIT_SUCCESS;
}