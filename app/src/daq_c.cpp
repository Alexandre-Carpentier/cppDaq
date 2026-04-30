/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     06/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////

// You must call this header function with NI Labview if you use it. (and manage handle destruction yourself...)
// 
// This is used to provide a universal stable C ABI
// Prefer to link with c++ wrapper directly to avoid this level of indirection.

#include "cppDaq.h"
#include "find_device.h"
#include "daq_c.h"
#include "assert.h"
#include <string>
#include <format>
#ifdef _WIN32
#include "Windows.h"
#endif
#ifdef __APPLE__
#include <string.h>
#define MAX_PATH 260
// Windows safe function not available on Apple
static errno_t strncpy_s(
   char *strDest,
   size_t numberOfElements,
   const char *strSource,
   size_t count
)
{
	// Unsafe
	char* result = strncpy(strDest, strSource, numberOfElements);
	return 0;
}
#endif

// This opaque data structure hide implementation detail and forward calls to the c++ library.
struct DAQ
{
public:
	std::unique_ptr<cppDaq> m_daq;
	~DAQ()
	{
	}
};

//
int get_status_(HDAQ handle)// Return current status.
{
	if (handle == nullptr)
	{
		printf("[!] get_status() fail handle is 0/nullptr.\n");
		return -1;
	}
	DAQ* impl = (DAQ*)handle;

	return (int)impl->m_daq->GetStatus(); // Forward
} 

double get_analog_value_(HDAQ handle, int io_id) // Return a volt value
{
	if (handle == nullptr)
	{
		printf("[!] get_analog_value() fail handle is 0/nullptr.\n");
		return NAN;
	}
	DAQ* impl = (DAQ*)handle;

	analog_pin pin(io_id);
	return impl->m_daq->ReadAnalogPin(pin); // Forward
}

//
bool set_digital_value_(HDAQ handle, int io_id, int state) // Set pin to HIGH or LOW logic
{
	if (handle == nullptr)
	{
		printf("[!] set_digital_value() fail handle is 0/nullptr.\n");
		return false;
	}
	DAQ* impl = (DAQ*)handle;

	digital_state state_safe = digital_state::LOW;
	switch (state)
	{
	case 0:
		state_safe = digital_state::LOW;
		break;
	case 1:
		state_safe = digital_state::HIGH;
		break;
	default:
		state_safe = digital_state::LOW;
	}

	digital_pin pin(io_id);
	return impl->m_daq->WriteDigitalPin(pin, state_safe); // Forward
}

bool set_analog_value_(HDAQ handle, int pin, double value)
{
	if (handle == nullptr)
	{
		printf("[!] set_analog_value() fail handle is 0/nullptr.\n");
		return false;
	}
#ifdef _WIN32
	MessageBox(0, "Not implemented yet", "Fail", MB_ICONEXCLAMATION | S_OK);
#endif
	return false;
}

//

HDAQ create_cppDaq(const char* device, 
	const int analog_count, int analog_io_id[32],
	const int digital_count, int digital_io_id[32],
	const int analog_co_count, int analog_co_io_id[32],
	const int digital_co_count, int digital_co_io_id[32])
{
	printf("create_cppDaq in progress.\n");

		// Sanitize

	if (!device)
	{
		return NULL;
	}

	if (analog_count > 0)
	{
		if (analog_io_id == NULL)
		{
			printf("[!] Error: analog_io_id is NULL\n"); 
			return NULL;
		}
	}

	if (digital_count > 0)
	{
		if (digital_io_id == NULL)
		{
			printf("[!] Error: digital_io_id is NULL\n");
			return NULL;
		}
	}

	if (digital_co_count > 0)
	{
		if (digital_co_io_id == NULL)
		{
			printf("[!] Error: digital_co_io_id is NULL\n");
			return NULL;
		}
	}

	if (analog_co_count > 0)
	{
		if (analog_co_io_id == NULL)
		{
			printf("[!] Error: analog_co_io_id is NULL\n");
			return NULL;
		}
	}

	// Analog out off bounds
	if(analog_count <0)
	{
		return NULL;
	}
	if (analog_count > MAX_ANALOG_INPUT_CHANNELS)
	{
		return NULL;
	}

	// Digital out off bounds
	if (digital_count < 0) 
	{
		return NULL;
	}
	if (digital_count > MAX_DIGITAL_CHANNELS)
	{
		return NULL;
	}

	// Analog continuous out off bounds
	if (analog_co_count < 0)
	{
		return NULL;
	}
	if (analog_co_count > MAX_ANALOG_OUTPUT_CHANNELS)
	{
		return NULL;

	}

	// Digital continuous out off bounds
	if (digital_co_count < 0) 
	{
		return NULL;
	}	
	if (digital_co_count > MAX_DIGITAL_CHANNELS)
	{
		return NULL;
	}


	// Build c object

	DAQ* impl =  new DAQ();

	// Transfert C style to analog vector 
	std::vector<size_t> analog_io;
	for (size_t i = 0; i < analog_count; i++)
	{
		analog_io.emplace_back(analog_io_id[i]);
	}

	// Transfert C style to digital vector 
	std::vector<size_t> digital_io;
	for (size_t i = 0; i < digital_count; i++)
	{
		digital_io.emplace_back(digital_io_id[i]);
	}

	// Transfert C style to analog co vector 
	std::vector<size_t> analog_co_io;
	for (size_t i = 0; i < analog_co_count; i++)
	{
		analog_co_io.emplace_back(analog_co_io_id[i]);
	}

	// Transfert C style to digital co vector 
	std::vector<size_t> digital_co_io;
	for (size_t i = 0; i < digital_co_count; i++)
	{
		digital_co_io.emplace_back(digital_co_io_id[i]);
	}

	DaqConfig config;

	config.setDevice("Dev1")
		.setAnalogPins(analog_pins(analog_io))
		.setDigitalPins(digital_pins(digital_io))
		.setAnalogContinuous(analog_pins_continuous(analog_co_io))
		.setDigitalContinuous(digital_pins_continuous(digital_co_io));

	// Forward
	impl->m_daq =  std::make_unique<cppDaq>(config);

 	if(impl->m_daq->GetStatus() != DAQ_STATUS::NO_ERR)
 	{
 		printf("[!] create_cppDaq fail during cppDaq creation.\n");
 		delete impl;
 		impl = 0;
 		return NULL;
 	}

	printf("create_cppDaq done.\n");
	return (void*)impl;
}

bool start_digital_pulse_(HDAQ handle, int io_id, int freq_hz)
{
	if (handle == NULL)
	{
		printf("[!] start_digital_pulse_() fail handle is 0/nullptr.\n");
		return NULL;
	}

	DAQ* impl = (DAQ*)handle;
	Frequency_hz rate = Frequency_hz((double)freq_hz);
	digital_pin_continuous pin(io_id);
	return impl->m_daq->StartDigitalCounterPWM(pin, rate); // Forward
}

bool stop_digital_pulse_(HDAQ handle)
{
	DAQ* impl = (DAQ*)handle;
	if (handle == NULL)
	{
		printf("[!] stop_digital_pulse() fail handle is 0/nullptr.\n");
		return false;
	}
	return impl->m_daq->StopDigitalCounterPWM(); // Forward

}

bool start_digital_software_pulse_(HDAQ handle, int io_id, int freq_hz)
{
	if (handle == NULL)
	{
		printf("[!] start_digital_pulse_() fail handle is 0/nullptr.\n");
		return false;
	}

	DAQ* impl = (DAQ*)handle;
	Frequency_hz rate = Frequency_hz((double)freq_hz);
	digital_pin_continuous pin(io_id);
	return impl->m_daq->StartDigitalSoftwarePWM(pin, rate); // Forward
}

bool stop_digital_software_pulse_(HDAQ handle)
{
	DAQ* impl = (DAQ*)handle;
	if (handle == NULL)
	{
		printf("[!] stop_digital_pulse() fail handle is 0/nullptr.\n");
		return false;
	}
	return impl->m_daq->StopDigitalSoftwarePWM(); // Forward

}

bool update_digital_software_pulse_(HDAQ handle, int freq_hz)
{
	if (handle == NULL)
	{
		printf("[!] update_digital_pulse() fail handle is 0/nullptr.\n");
		return false;
	}
	DAQ* impl = (DAQ*)handle;
	Frequency_hz rate = Frequency_hz((double)freq_hz);
	return impl->m_daq->UpdateDigitalSoftwarePWM(rate); // Forward
}

bool start_analog_pulse_(HDAQ handle, int io_id, int freq_hz)
{
	if (handle == NULL)
	{
		printf("[!] set_analog_pulse() fail handle is 0/nullptr.\n");
		return false;
	}
	DAQ* impl = (DAQ*)handle;

	Frequency_hz rate = Frequency_hz((double)freq_hz);
	analog_pin_continuous pin(io_id);
	return impl->m_daq->StartAnalogPWM(pin, rate); // Forward
}

bool stop_analog_pulse_(HDAQ handle, int io_id)
{
	if (handle == NULL)
	{
		printf("[!] set_analog_pulse() fail handle is 0/nullptr.\n");
		return false;
	}
	DAQ* impl = (DAQ*)handle;
	analog_pin_continuous pin(io_id);
	return impl->m_daq->StopAnalogPWM(pin); // Forward
}

bool destroy_cppDaq_(HDAQ handle)
{
	if (handle == NULL)
	{
		printf("[!] destroy_cppDaq() fail handle is 0/nullptr.\n");
		return false;
	}
	DAQ* impl = (DAQ*)handle;
	delete impl;
	printf("destroy_cppDaq success.\n");
	return true;
}

static char* get_ni_device_list(int* read)
{
	static char buffer[MAX_PATH] = "";
	cppDaqFindDevice find;
	std::vector<std::string> vec = find.get_list();

	// sizing and copy
	int buffer_length = 0;
	for (auto& device_name : vec)
	{
		if (buffer_length + device_name.size() + 2 > MAX_PATH) 
		{
			break;
		}
		if (strncpy_s(buffer, device_name.size(), device_name.c_str(), (MAX_PATH - buffer_length)) < 0)
		{
			std::printf("[!] Critical error when string copy.\n");
			assert(false);
		}
		if (strncpy_s(buffer, 1, ",", (MAX_PATH - buffer_length)) < 0)
		{
			std::printf("[!] Critical error when string copy.\n");
			assert(false);
		}

		buffer_length += device_name.size() + 1; // + 1 for ,
	}

	buffer_length = buffer_length + 1; // + 1 for \0
	buffer[buffer_length] = '\0';

	*read = buffer_length;
	return buffer;
}