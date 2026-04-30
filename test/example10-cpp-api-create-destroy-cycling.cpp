/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     25/02/26
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "cppDaq.h"
#include "term_colors.h"
#include <print>

int main()
{

	std::print(CYN"[ EXAMPLE 10] Testing opening and closing object in loop for crash...\n"); std::print(NC);

	analog_pin an0{ 0 };
	for (size_t i = 0; i < 2; ++i)
	{
		DaqConfig config;
		config.setDevice("Dev1").setAnalogPins(analog_pins(an0));
		cppDaq usb(config);

		double mesure_volt = usb.ReadAnalogPin(an0);
		std::print("{}\n", mesure_volt);
	}
	return 0;
}