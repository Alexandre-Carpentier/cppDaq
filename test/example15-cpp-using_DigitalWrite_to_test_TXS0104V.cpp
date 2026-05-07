/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     06/05/2026
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

#include "../app/src/cppDaq.h"
#include "../app/src/physic_types.h"

// To test with TSX0104V

digital_pin A1_PIN{ 2 };				// P0.0 → R208 DB9 Pin 4 : Motor enable (HIGH = active)


int main()
{
	std::print("[*] Connecting to DAQ\n");

	DaqConfig config;

	config.setDevice("Dev1") 
		.setDigitalPins(digital_pins{ A1_PIN }); // Level sent to the driver at 3.3V

	std::unique_ptr<cppDaq> m_daq = std::make_unique<cppDaq>(config);
	assert(m_daq); // Ensure DAQ is initialized correctly

	m_daq->WriteDigitalPin(A1_PIN, digital_state::HIGH); // 3.3V

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	m_daq->WriteDigitalPin(A1_PIN, digital_state::LOW); // 0.0V

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	return 0;
}