/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     11/05/2026
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cassert>
#include <memory>
#include <thread>
#include <chrono>
#include <print>
#include <concepts>

#include "term_colors.h"
#include "../app/src/cppDaq.h"
#include "../app/src/physic_types.h"

// To test with Techniwave TWSF0438EG117S2201

digital_pin A1_PIN{ 2 };				// P0.2 → Techniwave DB9 Pin 6 : Motor enable (HIGH = active)
digital_pin A2_PIN{ 3 };				// P0.3 → Techniwave DB9 Pin 7 : Motor enable (HIGH = active)
digital_pin A3_PIN{ 4 };				// P0.4 → Techniwave DB9 Pin 8 : Motor enable (HIGH = active)
digital_pin A4_PIN{ 5 };				// P0.5 → Techniwave DB9 Pin 9 : Motor enable (HIGH = active)

int main()
{
	std::print("[*] Connecting to DAQ\n");

	DaqConfig config;

	config.setDevice("Dev1").setDigitalPins(digital_pins{ A1_PIN, A2_PIN, A3_PIN, A4_PIN }); // 3.3V sent to level shifter output at 5V

	std::unique_ptr<cppDaq> m_daq = std::make_unique<cppDaq>(config);
	assert(m_daq); // Ensure DAQ is initialized correctly

	for (int i = 0; i < 1000; ++i)
	{
		// All pin to HIGH (5.0V) 
		m_daq->WriteDigitalPin(A1_PIN, 1.0); // 5.0V
		m_daq->WriteDigitalPin(A2_PIN, 1.0); // 5.0V
		m_daq->WriteDigitalPin(A3_PIN, 1.0); // 5.0V
		m_daq->WriteDigitalPin(A4_PIN, 1.0); // 5.0V

		std::print(RED"[*] OFF\n"); std::print(NC);

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		// Enable A1
		m_daq->WriteDigitalPin(A1_PIN, 0.0);
		m_daq->WriteDigitalPin(A2_PIN, 1.0);
		m_daq->WriteDigitalPin(A3_PIN, 1.0);	
		m_daq->WriteDigitalPin(A4_PIN, 1.0);

		std::print(CYN"[*] ON\n"); std::print(NC);

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
	return 0;
}