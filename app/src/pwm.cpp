/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     25/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "pwm.h"
#include <print>
#include <thread>
#include <chrono>
#include <assert.h>
#include "cppDaq.h"
#ifdef _WIN32
#include "Windows.h"
#endif

static void pwm_loop(std::stop_token st, cppDaq* daq, size_t pin, size_t period_us)
{
	std::print("[*] PWM loop start\n");
	while (!st.stop_requested())
	{
		auto half_period_us = period_us / 2;
		daq->WriteDigitalPin(digital_pin(pin), digital_state::HIGH);
		std::this_thread::sleep_for(std::chrono::microseconds(half_period_us));
		daq->WriteDigitalPin(digital_pin(pin), digital_state::LOW);
		std::this_thread::sleep_for(std::chrono::microseconds(half_period_us));
	}
}

cPwm::cPwm(cppDaq* daq) :
	m_period_us(0),
	m_pin(0),
	m_daq(daq)
{
	std::print("[*] Pwm ctor\n");
	assert(m_daq);

};

cPwm::~cPwm()
{
	stop_pulse();
	std::print("[*] Pwm dtor\n");
}
bool cPwm::start_pulse(size_t io_id, size_t period_us)
{
	m_pin = io_id;
	m_period_us = period_us;

	assert(m_daq);
	if (!thread.joinable())
	{
		thread = std::jthread(pwm_loop, m_daq, m_pin, m_period_us);
	}
	if (!thread.joinable())
	{
#ifdef _WIN32
		std::string msg = std::format("start_pulse fails\n");
		MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
		return false;
	}
	return true;
}

bool cPwm::update_pulse(size_t period_us)
{
	m_period_us = period_us;
	return true;
}

bool cPwm::stop_pulse()
{
	if (thread.joinable())
	{
		std::print("[*] Request stop\n");
		thread.request_stop();
		std::print("[*] Stopped\n");
		std::print("[*] Joining\n");
		thread.join();
		std::print("[*] Joined\n");
	}
	return true;
};
