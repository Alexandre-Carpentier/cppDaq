/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     24/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <thread>
#include "cppDaq.h"

class [[deprecated]]cPwm
{
public:
	[[deprecated]] cPwm(cppDaq* daq);
	~cPwm();

	[[deprecated]] bool  start_pulse(size_t io_id, size_t period_us);
	[[deprecated]] bool  update_pulse(size_t period_us);
	[[deprecated]] bool stop_pulse();
private:
	size_t m_pin;
	size_t m_period_us;
	std::jthread thread;
	cppDaq *m_daq;
};