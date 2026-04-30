#pragma once
/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     12/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include <print>
#include <chrono>
#include <thread>
#include "daq_c.h"
#include "cppDaq.h"
#include "check.h"
#include "create.h"
#include "term_colors.h"

// Cancel calls to MessageBoxA on Windows because it is anoying to click S_OK btn.
// This is totally optionnal hacky and boiler plate feel free to remove this or use Detours?
// Only work with MBCS builds, to use enable ==> #define HOOKENABLE
#define HOOKENABLE
#ifdef HOOKENABLE
#ifdef WIN32
#include "Windows.h"
typedef int (*MessageBoxA_)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxA_ mb = 0;
int mb_hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) { return 0; }// Call canceled
#endif
void cancelMessageBox()
{
	#ifdef WIN32
	// Disabling MessageBoxA because it is anoying to click when running tests
	mb = (MessageBoxA_)GetProcAddress(LoadLibrary("User32.dll"), "MessageBoxA");
	if (mb)
	{
		DWORD oldProtect;
		if (VirtualProtect(mb, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			// 0x48B8Long jump (abs) on 64 bit arch almost mandatory as addr can be very far away
			// 0xFF25 is another option (shorter) but why not
#ifdef _M_X64 
			// mov rax, imm64 ; jmp rax
			uint8_t patch[12];
			patch[0] = 0x48;
			patch[1] = 0xB8;
			uint64_t hookAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(mb_hook));
			std::memcpy(&patch[2], &hookAddr, sizeof(hookAddr)); // 8 bytes
			patch[10] = 0xFF;
			patch[11] = 0xE0;
			std::memcpy(mb, patch, sizeof(patch));
#else
			// relative JMP (E9) with 32-bit offset
			uint8_t patch[5];
			patch[0] = 0xE9;
			int32_t rel = static_cast<int32_t>(reinterpret_cast<intptr_t>(mb_hook) - (reinterpret_cast<intptr_t>(mb) + 5));
			std::memcpy(&patch[1], &rel, sizeof(rel));
			std::memcpy(mb, patch, sizeof(patch));
#endif
			VirtualProtect(mb, sizeof(void*), oldProtect, &oldProtect);
		}
	}
	#endif
}
#endif


// Helper function
[[nodiscard]] static std::string get_name()
{
	std::string res = "Unknown";
	#ifndef __aarch64__
	size_t len = DAQmxGetSysDevNames(NULL, 0);
	std::string device(len, 0);
	DAQmxGetSysDevNames(device.data(), len);
	if(device.size()>0)
	{	
		res = device;
	}
	#endif
	return res;
}

// Test c++ front end API
#include "gtest/gtest.h"
TEST(test_api, test1_valid_inputs)
{
	std::print("[*] test daq with valid inputs\n");

	// Enable pin 0, 3 and 7 as analog input

	analog_pin an0(0);
	analog_pin an1(1);
	const analog_pins analogs{ an0, an1}; // On Daq usb6001 0 to 7 are available with RSE (not differential)

	digital_pin di0(0);
	digital_pin di1(1);

	const digital_pins digitals{ di0, di1};

	analog_pins_continuous analogs_co{ };
	digital_pins_continuous digitals_co{ };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
		.setDigitalPins(digitals)
		.setAnalogContinuous(analogs_co)
		.setDigitalContinuous(digitals_co)
	);

	DAQ_STATUS stat = daq.GetStatus();
	std::print(GRN"DAQ status: {}\n", static_cast<int>(stat));
	std::print(NC);
	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::DAQ_RETRIEVE_NAME_FAIL);
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_EQ(stat, DAQ_STATUS::NO_ERR);
	#endif
}

TEST(test_api, test2)
{
	std::print("[*] test daq with signals id > MAX_CHAN\n");

	analog_pin an0{ 0 };
	analog_pin an9{ 9 };
	digital_pin di4{ 4 };
	digital_pin di1{ 1 };
	analog_pins analogs{ std::vector<size_t>{an0.get(), an9.get()} };
	digital_pins digitals{ std::vector<size_t>{di4.get(), di1.get() }};
	analog_pins_continuous analogs_co{ std::vector<size_t>{} };
	digital_pins_continuous digitals_co{ std::vector<size_t>{} };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
		.setDigitalPins(digitals)
		.setAnalogContinuous(analogs_co)
		.setDigitalContinuous(digitals_co)
	);

 
	DAQ_STATUS stat = daq.GetStatus();

	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::INPUTS_NOT_VALID);
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_EQ(daq.GetStatus(), DAQ_STATUS::INPUTS_NOT_VALID);
	#endif
}

TEST(test_api, test3)
{
	std::print("[*] test daq sig with same chan_num\n");
	analog_pin an2{ 2 };
	analog_pin an2_{ 2 };
	digital_pin di7{ 7 };
	digital_pin di7_{ 7 };
	analog_pins analogs(std::vector<size_t>(an2.get(), an2_.get()));
	digital_pins digitals{ std::vector<size_t>(di7.get(), di7_.get()) };
	analog_pins_continuous analogs_co{ std::vector<size_t>{} };
	digital_pins_continuous digitals_co{ std::vector<size_t>{} };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
		.setDigitalPins(digitals)
		.setAnalogContinuous(analogs_co)
		.setDigitalContinuous(digitals_co)
	);
 
	DAQ_STATUS stat = daq.GetStatus();

	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::INPUTS_NOT_VALID);
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_EQ(stat, DAQ_STATUS::INPUTS_NOT_VALID);
	#endif
}

TEST(test_api, test4)
{
	std::print("[*] test daq channel with empty name and void vectors\n");

	// Safe by design (won't compile)
	//analog_pin an0{};
	//analog_pin an1{};
	//digital_pin di2{};
	//digital_pin di3{};

	// Testing without pin registered
	analog_pins analogs(std::vector<size_t>({}));
	digital_pins digitals{ std::vector<size_t>({}) };
	analog_pins_continuous analogs_co{ std::vector<size_t>{} };
	digital_pins_continuous digitals_co{ std::vector<size_t>{} };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
		.setDigitalPins(digitals)
		.setAnalogContinuous(analogs_co)
		.setDigitalContinuous(digitals_co)
	);

 
	DAQ_STATUS stat = daq.GetStatus();

	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::DAQ_RETRIEVE_NAME_FAIL);
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_EQ(daq.GetStatus(), DAQ_STATUS::INPUTS_NOT_VALID);
	#endif
}

TEST(test_api, test5)
{
	std::print("[*] test daq_check with bad name\n");
	cppDaqCheck check("wrong_name");
	EXPECT_EQ(check.get_status(), DAQ_CHECK_STATUS::NO_SERIAL_FOUND);
}

TEST(test_api, test6)
{
	std::print("[*] test daq_check with no name\n");
	cppDaqCheck check("");
	EXPECT_EQ(check.get_status(), DAQ_CHECK_STATUS::DEV_EMPTY);
}

#ifndef __aarch64__
TEST(test_api, test7)
{
	std::print("[*] test analog_daq_io with bad name\n");
	analog_pins analogs(std::vector<size_t>(0, 1));
	daqmxCreateAnalog an("wrong_name", analogs);
	EXPECT_EQ(an.get_status(), DAQ_AN_CREATE_STATUS::DEVICE_NOT_FOUND);
}

TEST(test_api, test8)
{
	std::print("[*] test analog_daq_io with bad chan conf 1\n");
	const std::vector<size_t> an_channel_number{ 9, 1 };
	analog_pins analogs(std::vector<size_t>(9, 1));
	daqmxCreateAnalog an(get_name(),analogs);
	DAQ_AN_CREATE_STATUS stat = an.get_status();
	EXPECT_EQ(stat, DAQ_AN_CREATE_STATUS::INPUTS_NOT_VALID);
}

TEST(test_api, test9)
{
	std::print("[*] test digital_daq_io with bad name\n");
	digital_pin di0 = digital_pin(0);
	digital_pin di1 = digital_pin(1);
	digital_pins digitals(di0, di1);
	daqmxCreateDigital di("wrong_name", digitals);
	EXPECT_EQ(di.get_status(), DAQ_DI_CREATE_STATUS::ERR_CREATE_DO_LOGIC);
}

TEST(test_api, test10)
{
	std::print("[*] test digital_daq_io with bad chan conf 1\n");
	digital_pin di0 = digital_pin(0);
	digital_pin di1 = digital_pin(9);
	digital_pins digitals(di0, di1);
	daqmxCreateDigital di(get_name(), digitals);
	EXPECT_EQ(di.get_status(), DAQ_DI_CREATE_STATUS::ERR_CREATE_DO_LOGIC);
}
#endif

// Test c front end API
TEST(test_api, test11)
{
	std::print("[*] test C API standard calls\n");

	char dev[] = "Dev1";
	int an[2] = { 0,1 };
	int di_co[2] = { 0,1 };
	double result = 0.0;

	std::print(CYN"Only available on card with counter outputs(6001, 6510 not compatible\nTEST CAN FAIL HERE\n"); std::print(NC);
	HDAQ daq = create_cppDaq(dev, 2, an, 0, nullptr, 0, nullptr, 2, di_co);

	#ifdef __aarch64__
	EXPECT_EQ(daq, nullptr);
	return;
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_NE(daq,  nullptr);
	#endif

	
	bool res = start_digital_pulse_(daq, 0, 100); // Set a PWM sqare pulse at 100Hz
	EXPECT_EQ(res, true); 

	for (int i = 0; i < 2; i++)
	{
		EXPECT_NE( get_analog_value_(daq, 0), NAN);

		std::print(GRN"\nAnalog:{:.2} V", get_analog_value_(daq, 0)); std::print(NC);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	EXPECT_NE(stop_digital_pulse_(daq), false);

	destroy_cppDaq_(daq);
}

TEST(test_api, test12)
{
	std::print("[*] test C API with analog count = -1\n");

	char dev[] = "Dev2";// Dev2 only supportt digital I/O
	int an[2] = { 0,1 };
	int di[2] = { 0,1 };
	double result = 0.0;

	HDAQ daq = create_cppDaq(dev, -1, an, 0, nullptr, 0, nullptr, 0, nullptr);

	EXPECT_EQ(daq, nullptr);
	destroy_cppDaq_(daq);
}

TEST(test_api, test13)
{
	std::print("[*] test C API with digital pin > 8\n");

	char dev[] = "Dev1"; // Dev2 only supportt digital I/O
	int an[2] = { 0,1 };
	int di[2] = { 9,1 };
	double result = 0.0;

	HDAQ daq = create_cppDaq(dev, 2, an, 0, di, 0, nullptr, 0, nullptr);
	#ifdef __aarch64__
	EXPECT_EQ(daq, nullptr);
	return;
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_NE(daq, nullptr);
	#endif

	EXPECT_NE(get_analog_value_(daq, 0), NAN);

	destroy_cppDaq_(daq);
}

TEST(test_api, test14)
{
	std::print("[*] test C API accessing bad pin number\n");

	char dev[] = "Dev2"; // Dev2 only supportt digital I/O
	int an[2] = { 0,1 };
	int di[2] = { 9,1 };
	double result = 0.0;

	HDAQ daq = create_cppDaq(dev, 2, an, 0, di, 0, nullptr, 0, nullptr);

	EXPECT_EQ(set_digital_value_(daq, 1, 1), false);
	EXPECT_EQ(set_digital_value_(daq, 9, 1), false);
	destroy_cppDaq_(daq);
}

TEST(test_api, test15)
{
	std::print("[*] Test strong types\n");

	//cppDaq daq = cppDaq("Dev1", {0,1,2}, {0,1,2}, {0,1,2}, {0,1,2}); // nok
	analog_pin an0(0);//ok
	//analog_pin an1(0.0);//nok
	//analog_pin an0("txt");//nok
	
	analog_pins analogs = analog_pins{0,1,2};//nok
	//analog_pins analogs0 = analog_pins(0,1,2);//nok
	std::vector<size_t> list {0,1,2};
	//analog_pins analogs1 = analog_pins(0,1,2);//nok
	analog_pins analogs2= analog_pins(list);//ok
	//analog_pins analogs3 = analog_pins(0);//nok

	analog_pins analogs4 = analog_pins(an0); //ok

	std::string dev = "";

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
	);
	
	DAQ_STATUS stat = daq.GetStatus();

	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::DAQ_RETRIEVE_NAME_FAIL);
	return;
	#elif defined(WIN32) | defined (__LINUX__) 
	EXPECT_NE(stat, DAQ_STATUS::INPUTS_NOT_VALID);
	#endif
}

TEST(test_api, test16)
{
	std::print("[*] test GetDeviceList\n");
	auto list = GetDeviceList();
	std::print("Devices count: {}\n", list.size());
	EXPECT_GE(list.size(), 0);
}

TEST(test_api, test17)
{
	std::print("[*] test cppDaq move semantics\n");
	analog_pin an0(0);
	analog_pins analogs{ an0 };

	cppDaq daq1 = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
	);

	cppDaq daq2 = std::move(daq1);

	DAQ_STATUS stat = daq2.GetStatus();
	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::DAQ_RETRIEVE_NAME_FAIL);
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_EQ(stat, DAQ_STATUS::NO_ERR);
	#endif
}

TEST(test_api, test18)
{
	std::print("[*] test ReadAnalogPin\n");
	analog_pin an0(0);
	analog_pins analogs{ an0 };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		double val = daq.ReadAnalogPin(an0);
		EXPECT_NE(val, NAN);
	}
}

TEST(test_api, test19)
{
	std::print("[*] test WriteDigitalPin\n");
	digital_pin di0(0);
	digital_pins digitals{ di0 };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setDigitalPins(digitals)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		bool res = daq.WriteDigitalPin(di0, digital_state::HIGH);
		EXPECT_TRUE(res);
		res = daq.WriteDigitalPin(di0, digital_state::LOW);
		EXPECT_TRUE(res);
	}
}

TEST(test_api, test20)
{
	std::print("[*] test ReadMultipleAnalogPin\n");
	analog_pin an0(0);
	analog_pins analogs{ an0 };

	cppDaq daq = cppDaq(DaqConfig{}
		.setDevice("")
		.setAnalogPins(analogs)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		auto vals = daq.ReadMultipleAnalogPin(an0);
		EXPECT_EQ(vals.size(), 1000);
	}
}

TEST(test_api, test21)
{
	std::print("[*] test StartDigitalCounterPWM\n");
	digital_pin_continuous dic0(0);
	digital_pins_continuous digitals_co{ dic0 };

	cppDaq daq = cppDaq(DaqConfig()
		.setDevice("")
		.setDigitalContinuous(digitals_co)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		bool res = daq.StartDigitalCounterPWM(dic0, Frequency_hz<double>(100.0));
		EXPECT_TRUE(res);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		res = daq.StopDigitalCounterPWM();
		EXPECT_TRUE(res);
	}
}

TEST(test_api, test22_PWM_digital_start)
{
	std::print("[*] test StartDigitalSoftwarePWM\n");
	digital_pin_continuous dic0(0);
	digital_pins_continuous digitals_co{ dic0 };

	cppDaq daq = cppDaq(DaqConfig()
		.setDevice("")
		.setDigitalContinuous(digitals_co)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		bool res = daq.StartDigitalSoftwarePWM(dic0, Frequency_hz<double>(50.0));
		EXPECT_TRUE(res);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		res = daq.UpdateDigitalSoftwarePWM(Frequency_hz<double>(20.0));
		EXPECT_TRUE(res);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		res = daq.StopDigitalSoftwarePWM();
		EXPECT_TRUE(res);
	}
}

TEST(test_api, test23_PWM_analog_start)
{
	std::print("[*] test StartAnalogPWM\n");
	analog_pin_continuous anc0(0);
	analog_pins_continuous analogs_co{ anc0 };

	cppDaq daq = cppDaq(DaqConfig()
		.setDevice("")
		.setAnalogContinuous(analogs_co)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		bool res = daq.StartAnalogPWM(anc0, Frequency_hz<double>(100.0));
		if(res) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			daq.StopAnalogPWM(anc0);
		}
	}
}

TEST(test_api, test24_test_parameters_initializer)
{
	std::print("[*] test Builder multiple adds\n");
	analog_pin an0(0);
	analog_pin an1(1);
	
	cppDaq daq = cppDaq(DaqConfig()
		.setDevice("")
		.setAnalogPins(analog_pins{ an0 })
		.setAnalogPins(analog_pins{ an1 })
	);


	DAQ_STATUS stat = daq.GetStatus();
	#ifdef __aarch64__
	EXPECT_EQ(stat, DAQ_STATUS::DAQ_RETRIEVE_NAME_FAIL);
	#elif defined(WIN32) | defined (__LINUX__)
	EXPECT_EQ(stat, DAQ_STATUS::NO_ERR);
	#endif
}

TEST(test_api, test25_using_analog_write)
{
	std::print("[*] test WriteAnalogPin\n");
	analog_pin an0(0);
	analog_pins analogs{ an0 };

	cppDaq daq = cppDaq(DaqConfig()
		.setDevice("")
		.setAnalogPins(analogs)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		bool res = daq.WriteAnalogPin(an0, 2.5);
		// Just ensure call is valid
	}
}

TEST(test_api, test26_using_multiple_DIGITAL_inputs)
{
	std::print("[*] test multiple WriteDigitalPin\n");
	digital_pin di0(0);
	digital_pin di1(1);
	digital_pins digitals{ di0, di1 };

	cppDaq daq = cppDaq(DaqConfig()
		.setDevice("")
		.setDigitalPins(digitals)
	);

	if (daq.GetStatus() == DAQ_STATUS::NO_ERR)
	{
		bool res = daq.WriteDigitalPin(di0, digital_state::HIGH);
		res = daq.WriteDigitalPin(di1, digital_state::HIGH);
		std::print("[*] Check Digital pin 0 and 1 for logic level: 0=H 1=H. Then press any key to continue.");
		system("pause");
		res = daq.WriteDigitalPin(di0, digital_state::LOW);
		res = daq.WriteDigitalPin(di1, digital_state::LOW);
		std::print("[*] Check Digital pin 0 and 1 for logic level: 0=L 1=L. Then press any key to continue.");
		system("pause");
		res = daq.WriteDigitalPin(di0, digital_state::HIGH);
		res = daq.WriteDigitalPin(di1, digital_state::LOW);
		std::print("[*] Check Digital pin 0 and 1 for logic level: 0=H 1=L. Then press any key to continue.");
		system("pause");
		res = daq.WriteDigitalPin(di1, digital_state::LOW);
		res = daq.WriteDigitalPin(di0, digital_state::HIGH);
		std::print("[*] Check Digital pin 0 and 1 for logic level: 0=H 1=L. Then press any key to continue.");
		system("pause");
		res = daq.WriteDigitalPin(di1, digital_state::HIGH);
		res = daq.WriteDigitalPin(di0, digital_state::LOW);
		std::print("[*] Check Digital pin 0 and 1 for logic level: 0=L 1=H. Then press any key to continue.");
		system("pause");
	}		
}

int main()
{
	// Unit test first each bloc
	std::print(GRN"[*] Unit tests start\n");
	std::print(NC);

	cancelMessageBox(); // Cancel annoying message on Win32 when testing
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}