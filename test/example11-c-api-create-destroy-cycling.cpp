/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     24/01/26
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "daq_c.h"
#include "term_colors.h"
#include <print>

int main()
{

	std::print(CYN"[ EXAMPLE 10] Testing opening and closing object in loop for crash...\n"); std::print(NC);

	analog_pin an0{ 0 };
	for (size_t i = 0; i < 2; ++i)
	{
		char dev[] = "Dev1";
		int an[1] = { 0 };

		HDAQ usb = create_cppDaq(dev,
			1, an,
			0, nullptr,
			0, nullptr,
			0, nullptr);

		if (!usb)
		{
			std::print(RED"[!] create_cppDaq_() failed.\n"); std::print(NC);
			return EXIT_FAILURE;
		}
		double mesure_volt = get_analog_value_(usb, an0.get());
		std::print("{}\n", mesure_volt);
		destroy_cppDaq_(usb);
		std::print(GRN"[*] cycle done.\n"); std::print(NC);
	}
	return 0;
}