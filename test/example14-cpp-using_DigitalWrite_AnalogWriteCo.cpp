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

// To test with R208 driver

analog_input_pin HOME_PIN{ 0 };				// AI0  → R208 DB9 Pin 5 : Home position sensor
analog_pin_continuous CLOCK_PIN{ 0 };	// AO0  → R208 DB9 Pin 2 : PWM step signal (speed control)
digital_pin MOVE_PIN{ 0 };				// P0.0 → R208 DB9 Pin 4 : Motor enable (HIGH = active)
digital_pin DIR_PIN{ 1 };				// P0.1 → R208 DB9 Pin 3 : Direction (HIGH = CW, LOW = CCW)

int main()
{
	std::print("[*] Connecting to DAQ\n");

	DaqConfig config;

	config.setDevice("Dev1") // Auto connect to first device if empty. Default name is Dev1 if needed.
		.setAnalogInputPins(analog_input_pins{ HOME_PIN }) // To read home position
		.setAnalogContinuous(analog_pins_continuous{ CLOCK_PIN }) // To make a PWM signal
		.setDigitalPins(digital_pins{ MOVE_PIN, DIR_PIN }); // To control the direction and the movement

	std::unique_ptr<cppDaq> m_daq = std::make_unique<cppDaq>(config);
	assert(m_daq); // Ensure DAQ is initialized correctly


	m_daq->StartAnalogPWM(CLOCK_PIN, Frequency_hz<double>(1500.0));

	m_daq->WriteDigitalPin(DIR_PIN, 1.0);// turn clockwise
	m_daq->WriteDigitalPin(MOVE_PIN, 0.0); // Enable

	m_daq->WriteDigitalPin(DIR_PIN, 0.0);// turn clockwise
	m_daq->WriteDigitalPin(MOVE_PIN, 0.0); // Enable

	m_daq->WriteDigitalPin(DIR_PIN, 1.0);// turn clockwise
	m_daq->WriteDigitalPin(MOVE_PIN, 1.0); // Enable

	m_daq->WriteDigitalPin(DIR_PIN, 0.0);// turn clockwise
	m_daq->WriteDigitalPin(MOVE_PIN, 1.0); // Enable

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	return 0;
}