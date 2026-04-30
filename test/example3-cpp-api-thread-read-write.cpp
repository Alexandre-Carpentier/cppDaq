/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     21/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include <print>
#include <vector>
#include <assert.h>
#include <string>
#include <chrono>
#include <thread>
#include "cppDaq.h"
#include "term_colors.h"

std::jthread pulse_thread;

void pulse(cppDaq *daq6001)
{
	auto st = pulse_thread.get_stop_token();

	while (!st.stop_requested())
	{
		daq6001->WriteDigitalPin(digital_pin(1), digital_state::HIGH);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		daq6001->WriteDigitalPin(digital_pin(1), digital_state::LOW);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int main()
{
	std::print(CYN"[ EXAMPLE 3] Testing push pull output to VN10 FET transistor...\n");
	std::print(NC);
	
	// Retrieve connected device list

	std::vector<std::string> list = GetDeviceList();

	// Create obj for each usb daq
	analog_pin an0(0);
	analog_pin an1(1);
	analog_pin an2(2);
	analog_pins analogs(an0, an1, an2);

	digital_pin di0(0);
	digital_pin di1(1);
	digital_pins digitals(di0, di1);

	cppDaq daq6001(list[0], analogs, digitals, analog_pins_continuous{ std::vector<size_t>() }, digital_pins_continuous{ std::vector<size_t>() });

	// Digital pin High

	bool success = daq6001.WriteDigitalPin(di0, digital_state::HIGH);
	if (!success)
	{
		std::print(RED"[!] Failed to set digital pin\n");
		std::println(NC);
	}
	success = daq6001.WriteDigitalPin(di1, digital_state::HIGH);
	if (!success)
	{
		std::print(RED"[!] Failed to set digital pin\n");
		std::println(NC);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::print(GRN"[*] DO_0 = {:.3} V Vds = {:.3} V+ = {:.3}", daq6001.ReadAnalogPin(an0), daq6001.ReadAnalogPin(an1), daq6001.ReadAnalogPin(an2));
	double i = (daq6001.ReadAnalogPin(an2) - daq6001.ReadAnalogPin(an1)) / 22000;
	std::print(" I=U/R={:.3}A\n", i);
	std::println(NC);
	
	pulse_thread = std::jthread([&daq6001]() {pulse(&daq6001); });


	for (size_t i = 0; i < 10000; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::print(GRN"[*] {:.1} V\n", daq6001.ReadAnalogPin(an1));
	}
	pulse_thread.request_stop();
	pulse_thread.join();

	std::println(NC);
	
	return EXIT_SUCCESS;
}