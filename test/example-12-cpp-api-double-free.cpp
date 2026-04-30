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

	std::print(CYN"[ EXAMPLE 12] Testing double free for crash...\n"); std::print(NC);

	analog_pin an0{ 0 };
	DaqConfig config;
	config.setDevice("Dev1").setAnalogPins(analog_pins(an0));
	cppDaq* usb = new cppDaq(config);

	double mesure_volt = usb->ReadAnalogPin(an0);
	std::print("{}\n", mesure_volt);
	if (usb)
	{
		delete usb;
		usb = 0;
	}
	if (usb)
	{
		delete usb;
	}


	return 0;
}