/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     06/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "cppDaq.h"
#include "term_colors.h"
#include "find_device.h"
#include "check.h"
#include "create.h"
#include "dbgprint.h"
#include "pwm.h"
#include <print>
#include <string>
#include <array>
#include <variant>
#include <assert.h>
#ifdef _WIN32
#include "Windows.h"
#endif

#include <mutex>
std::mutex task_mutex; // Lock read write when PWM is used

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// To use first call cppDaq ctor. cppDaq responsability is to instanciate cppDaqimpl who hide implementation details (make api clean and stable ABI).
// The cppDaqimpl is responsible to get a device ref and make the ios (analog/digital). 
// It uses static polymorphism with variant and visit. The goal is to switch easily the types.
// 
// cppDaqimpl read or write to the analog or digital side.
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Use create_ios to create a vec of io. It is usefull to define the analog or the digital in the same vector a get
// all the measurements once.
// 
// Use create_analog_io or create_digital_io if only one type will be used
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Add more types here and defined it in daq_create_io.h to extend types 
using io = std::variant<CreateAnalog, CreateDigital>;
using io_co = std::variant<CreateAnalogContinuous, CreateDigitalContinuous>;

// All added types in the vectors will be called by the visitor
std::tuple<std::vector<io>, std::vector<io_co>> create_ios(
    const std::string device_name, 
    const analog_pins analog_io_id, 
    const digital_pins  digital_io_id, 
    const analog_pins_continuous analog_io_co_id,
    const digital_pins_continuous digital_io_co_id
)
{
    std::vector<io> ios;

    CreateAnalog ana = CreateAnalog(device_name, analog_io_id);
    if (ana.get_status() == DAQ_AN_CREATE_STATUS::NO_ERR)
    {
        ios.push_back(ana);
    }

    CreateDigital dig = CreateDigital(device_name, digital_io_id);
    if (dig.get_status() == DAQ_DI_CREATE_STATUS::NO_ERR)
    {
        ios.push_back(dig);
    }

    // TODO pack io_continuous with io and return a single vec
    std::vector<io_co> ios_co;
    CreateAnalogContinuous ana_co = CreateAnalogContinuous(device_name, analog_io_co_id);
    if(ana_co.get_status() == DAQ_CO_CREATE_STATUS::NO_ERR)
    {
        ios_co.emplace_back(ana_co);
    }
    CreateDigitalContinuous dig_co = CreateDigitalContinuous(device_name, digital_io_co_id);
    if(dig_co.get_status() == DAQ_CO_CREATE_STATUS::NO_ERR)
    {
        ios_co.emplace_back(dig_co);
    }
    return std::make_tuple(ios, ios_co);
}

// return analog type called by the visitor
std::vector<io> create_analog_io(const std::string device_name, const  analog_pins  analog_io_id)
{
    std::vector<io> ios;

    CreateAnalog ana = CreateAnalog(device_name, analog_io_id);
    if (ana.get_status() == DAQ_AN_CREATE_STATUS::NO_ERR)
    {
        ios.push_back(ana);
    }

    return ios;
}

// return digital type called by the visitor
std::vector<io> create_digital_io(const std::string device_name, const digital_pins digital_io_id)
{
    std::vector<io> ios;

    CreateDigital dig = CreateDigital(device_name, digital_io_id);
    if (dig.get_status() == DAQ_DI_CREATE_STATUS::NO_ERR)
    {
        ios.push_back(dig);
    }

    return ios;
}

// return analog type called by the visitor
std::vector<io_co> create_analog_co_io(const std::string device_name, const analog_pins_continuous analog_io_id)
{
    std::vector<io_co> ios;

    CreateAnalogContinuous ana_co = CreateAnalogContinuous(device_name, analog_io_id);
    if (ana_co.get_status() == DAQ_CO_CREATE_STATUS::NO_ERR)
    {
        ios.push_back(ana_co);
    }

    return ios;
}

// return digital type called by the visitor
std::vector<io_co> create_digital_co_io(const std::string device_name, const digital_pins_continuous digital_io_id)
{
    std::vector<io_co> ios;

    CreateDigitalContinuous dig_co = CreateDigitalContinuous(device_name, digital_io_id);
    if (dig_co.get_status() == DAQ_CO_CREATE_STATUS::NO_ERR)
    {
        ios.push_back(dig_co);
    }

    return ios;
}

// Use to convert IO_TYPE to string for debug messages
std::string io_type_to_string(IO_TYPE t)
{
    if (t == IO_TYPE::ana)
    {
        return "Analog";
    }
    if (t == IO_TYPE::dig)
    {
        return "Digital";
    }
    if (t == IO_TYPE::dig_co)
    {
        return "DigitalContinuous";
    }
    if (t == IO_TYPE::ana_co)
    {
        return "AnalogContinuous";
    }
    return "Unknown";
}

// Test if io exist
bool is_io_available(std::vector<io> ios, IO_TYPE t)
{
    for (auto& io : ios)
    {
        task_mutex.lock();
        IO_TYPE obj_type = std::visit([](auto&& arg) {return arg.get_type(); }, io);
        task_mutex.unlock();

        if (obj_type == t)
        {
            return true;
        }
    }
    std::print(RED"[!] Type not available\n");
    std::print(NC);
#ifdef _WIN32
    std::string msg = std::format("is_io_available Failed, {} io not available\n", io_type_to_string(t));
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    return false;
}

// Test if io exist
bool is_io_available(std::vector<io_co> ios, IO_TYPE t)
{
    for (auto& io : ios)
    {
        task_mutex.lock();
        IO_TYPE obj_type = std::visit([](auto&& arg) {return arg.get_type(); }, io);
        task_mutex.unlock();

        if (obj_type == t)
        {
            return true;
        }
    }
    std::print(RED"[!] Type not available\n");
    std::print(NC);
#ifdef _WIN32
    std::string msg = std::format("is_io_available Failed, {} io not available\n", io_type_to_string(t));
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    return false;
}

#pragma warning( push )
#pragma warning(disable:4715) // To disable the warning no case return switch
// Extract an io from ios vec
io get_io_from_ios(std::vector<io> ios, IO_TYPE t)
{
    for (auto& io : ios)
    {
        task_mutex.lock();
        IO_TYPE obj_type = std::visit([](auto&& arg) {return arg.get_type(); }, io);
        task_mutex.unlock();

        if (obj_type == t)
        {
            return io;
        }
    }
#ifdef _WIN32
    std::string msg = std::format("get_io_from_ios Failed, type {} is returned\n", io_type_to_string(t));
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    // no type is return.
    assert(false);
}
#pragma warning( pop )

#pragma warning( push )
#pragma warning(disable:4715) // To disable the warning no case return switch
// Extract an io from ios vec
io_co get_io_from_ios(std::vector<io_co> ios_co, IO_TYPE t)
{
    for (auto& io_co : ios_co)
    {
        task_mutex.lock();
        IO_TYPE obj_type = std::visit([](auto&& arg) {return arg.get_type(); }, io_co);
        task_mutex.unlock();

        if (obj_type == t)
        {
            return io_co;
        }
    }
#ifdef _WIN32
    std::string msg = std::format("get_io_from_ios Failed, type {} is returned\n", io_type_to_string(t));
    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
    // no type is return.
    assert(false);
}
#pragma warning( pop )

enum class SANITIZE_STATUS
{
    DAQVOID = 200,
    ASSIGN_TWICE,
    MAX_OVERRUN,
    NO_ERR
};

// Sanitize the main input entries here
SANITIZE_STATUS sanitize_inputs_(pin_list pins, size_t MAX_SIZE)
{
    if (pins.size() == 0)
    {
        std::print("[!] No Pin selected.\n");
        return SANITIZE_STATUS::DAQVOID;
    }

    // Must not be the same pin id
    size_t pos1 = 0;
    size_t pos2 = 0;
    for (auto& item1 : pins)
    {
        for (auto& item2 : pins)
        {
            if (item1 == item2)
            {
                if (pos1 != pos2)
                {
#ifdef _WIN32
                    std::string msg = std::format("sanitize_inputs() Failed, Pin number assigned twice\n");
                    MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
                    std::print("[!] Pin number assigned twice.\n");
                    return SANITIZE_STATUS::ASSIGN_TWICE;
                }
            }
            pos2++;
        }
        pos1++;
        pos2 = 0;
    }

    // Must be between 0 and MAX
    if ((pins.size() > MAX_SIZE) || (pins.size() < 0))
    {
        std::print(RED"[!] MAX_CHANNELS reached or < 0.\n");
        std::print(NC);
        return SANITIZE_STATUS::MAX_OVERRUN;
    }

    for (auto& id : pins)
    {
        if ((id > MAX_SIZE) || (id < 0))
        {
            std::print(RED"[!] id not valid.\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("sanitize_inputs() Failed, id not valide\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return SANITIZE_STATUS::MAX_OVERRUN;
        }
    }
    return SANITIZE_STATUS::NO_ERR;
}

bool is_io_registered(pin_t pin, pin_list pins)
{
    for (auto& elem : pins)
    {
        if (elem == pin.get())
        {
            return true;
        }
    }
    return false;
}

SANITIZE_STATUS do_sanitize(pin_list pins, size_t MAX_CHANNELS)
{
    SANITIZE_STATUS validity = sanitize_inputs_(pins, MAX_CHANNELS);
    if (validity != SANITIZE_STATUS::DAQVOID)
    {
        if (validity != SANITIZE_STATUS::NO_ERR)
        {
            std::print(RED"[!] DAQ inputs not valid\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("Sanitizing inputs fails.\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return validity;
        }
    }
    return validity;
}

// Opaque: to make a clean header
class cppDaq::cppDaqimpl
{
public:
    cppDaqimpl(cppDaq* self, const std::string device, const analog_pins ans, const digital_pins dis, const analog_pins_continuous ans_co, const digital_pins_continuous dis_co) :
        m_analog_io_id(ans),
        m_digital_io_id(dis),
        m_analog_co_io_id(ans_co),
        m_digital_co_io_id(dis_co),

        use_analog(false),
        use_digital(false),
        use_digital_co(false),
        use_analog_co(false),
        m_status(DAQ_STATUS::NO_ERR)
    {
        dbg::print("[*] cppDaqimpl ctor.\n");

        // Sanitize for each type of io

        SANITIZE_STATUS analog_validity = do_sanitize(ans, MAX_ANALOG_INPUT_CHANNELS);
        SANITIZE_STATUS digital_validity = do_sanitize(dis, MAX_DIGITAL_CHANNELS);
        SANITIZE_STATUS analog_co_validity = do_sanitize(ans_co, MAX_ANALOG_OUTPUT_CHANNELS);
        SANITIZE_STATUS digital_co_validity = do_sanitize(dis_co, MAX_DIGITAL_CHANNELS);

        // Check if a port is in another state than NO_ERR or DAQVOID = DAQ_STATUS::INPUTS_NOT_VALID
        auto is_all_port_valid = [analog_validity, digital_validity, analog_co_validity, digital_co_validity]() {

            auto is_port_valid = [](SANITIZE_STATUS status) {
                bool state = false;
                if (status == SANITIZE_STATUS::NO_ERR)
                {
                    state = true;
                }
                if (status == SANITIZE_STATUS::DAQVOID)
                {
                    state = true;
                }
                return state;
                };

            if (!is_port_valid(analog_validity) || !is_port_valid(digital_validity) || !is_port_valid(analog_co_validity) || !is_port_valid(digital_co_validity))
            {
                return DAQ_STATUS::INPUTS_NOT_VALID;
            }
            return DAQ_STATUS::NO_ERR;
            };
        
        m_status = is_all_port_valid(); // DAQ_STATUS::INPUTS_NOT_VALID; ?
 
        if (m_status == DAQ_STATUS::INPUTS_NOT_VALID)
        {
            return;
        }

            // Get device names

        cppDaqDevice m_daq_dev(device);
        if (m_daq_dev.get_status() != DAQ_DEVICE_STATUS::NO_ERR)
        {
            m_status = DAQ_STATUS::DAQ_RETRIEVE_NAME_FAIL;
            std::print(RED"[!] DAQ fail to retrieve daq dev name\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("DAQ fail to retrieve daq dev name\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return;
        }
        std::string m_device = m_daq_dev.get_name();

        // Check everythings is ok

        cppDaqCheck check(m_device);
        if (check.get_status() != DAQ_CHECK_STATUS::NO_ERR)
        {
            m_status = DAQ_STATUS::DAQ_TEST_DEVICE_FAIL;
            std::print(RED"[!] DAQ test fails\n");
            std::print(NC);
#ifdef _WIN32
            std::string msg = std::format("DAQ test fails\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return;
        }

        // Setup analog and digital ios

        if (analog_validity == SANITIZE_STATUS::NO_ERR)
        {
            use_analog = true;
            std::vector<io> analogs = create_analog_io(m_device, ans);
            for (auto& an : analogs)
            {
                ios.push_back(an);
            }
        }

        if (digital_validity == SANITIZE_STATUS::NO_ERR)
        {
            use_digital = true;
            std::vector<io> digitals = create_digital_io(m_device, m_digital_io_id);
            for (auto& di : digitals)
            {
                ios.push_back(di);
            }
        }

        if (analog_co_validity == SANITIZE_STATUS::NO_ERR)
        {
            use_analog_co = true;
            std::vector<io_co> analogs_co = create_analog_co_io(m_device, m_analog_co_io_id);
            for (auto& an_co : analogs_co)
            {
                ios_co.push_back(an_co);
            }
        }

        if (digital_co_validity == SANITIZE_STATUS::NO_ERR)
        {
            use_digital_co = true;
            std::vector<io_co> digitals_co = create_digital_co_io(m_device, m_digital_co_io_id);
            for (auto& di_co : digitals_co)
            {
                ios_co.push_back(di_co);
            }
        }

        // Use all analog and digital, and digital continuous

        //if ((use_analog) && (use_digital) && (use_digital_co) && (use_analog_co))
        //{
            //all_ios = create_ios(m_device, m_analog_io_id, m_digital_io_id, m_analog_co_io_id, m_digital_co_io_id);
            //ios = std::get<0>(all_ios);
            //ios_co = std::get<1>(all_ios);
        //}

        // All empties

        if ((!use_analog) && (!use_digital) && (!use_digital_co) && (!use_analog_co))
        {
            m_status = DAQ_STATUS::INPUTS_NOT_VALID;
        }

        // Error on IOs creation

        if ((ios.size() == 0) && (ios_co.size() == 0))
        {
            m_status = DAQ_STATUS::INPUTS_NOT_VALID;
        }

        // PWM module

    //ppwm = std::make_unique<cPwm>(parent);

    }

    DAQ_STATUS get_status()
    {
        return m_status;
    }

    // Get actual value (analog or digital)
    double get_value(IO_TYPE type, pin_t pin)
    {
        // Sanitize

        if (!is_io_available(ios, type))
        {
#ifdef _WIN32
            std::string msg = std::format("get_value() fails\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return NAN;
        }
        if (type == IO_TYPE::ana)
        {
            if (!use_analog)
            {
#ifdef _WIN32
                std::string msg = std::format("get_value() fails, analog io not available\n");
                MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
                return NAN;
            }
            if (!is_io_registered(pin, m_analog_io_id))
            {
#ifdef _WIN32
                std::string msg = std::format("get_value() fails, analog io not registered\n");
                MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
                return NAN;
            }

        }
        if (type == IO_TYPE::dig)
        {
            if (!use_digital)
            {
#ifdef _WIN32
                std::string msg = std::format("get_value() fails, digital io not available\n");
                MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
                return NAN;
            }
            if (!is_io_registered(pin, m_digital_io_id))
            {
#ifdef _WIN32
                std::string msg = std::format("get_value() fails, digital io not registered\n");
                MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
                return NAN;
            }

        }

        io io_ = get_io_from_ios(ios, type);

        size_t pos = pin_to_vec_pos(pin, m_analog_io_id);// Find pin pos in vec
        task_mutex.lock();
        auto result = std::visit([pos](auto&& arg) -> double
            {
                return arg.read(pin_t(pos));
            }, io_);
        task_mutex.unlock();

        return result;
    }

    //

    bool set_digital_value(digital_pin pin, digital_state state)
    {
        if (!use_digital)
        {
#ifdef _WIN32
            std::string msg = std::format("set_digital_value() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        if (!is_io_available(ios, IO_TYPE::dig))
        {
#ifdef _WIN32
            std::string msg = std::format("set_digital_value() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        if (!is_io_registered(pin, m_digital_io_id))
        {
            std::print("[!] IO PIN is not valid\n");
#ifdef _WIN32
            std::string msg = std::format("set_digital_value() fails, digital io not registered\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        io io_ = get_io_from_ios(ios, IO_TYPE::dig);
        size_t pos = pin_to_vec_pos(pin, m_digital_io_id);// Find pin pos in vec

        task_mutex.lock();
        auto result = std::visit([&pos, &state](auto&& arg) -> bool
            {
                return arg.write(pin_t(pos), state);
            }, io_);
        task_mutex.unlock();

        return result;
    }

    bool start_digital_pulse(digital_pin_continuous digital_io_id_co, Frequency_hz<double> rate)
    {
        if (!use_digital_co)
        {
#ifdef _WIN32
            std::string msg = std::format("start_digital_pulse() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        return set_digital_value_co(digital_io_id_co, rate);
        //return ppwm->start_pulse(io_id, period_us);
    }

    bool stop_digital_pulse()
    {
        if (!use_digital_co)
        {
#ifdef _WIN32
            std::string msg = std::format("start_digital_pulse() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        return false;
        //return ppwm->stop_pulse(); // manual reset
    }

    bool update_digital_pulse(Frequency_hz<double> rate)
    {
        if (!use_digital_co)
        {
#ifdef _WIN32
            std::string msg = std::format("start_digital_pulse() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        return false;
        //return ppwm->update_pulse(period_us);
    }

    bool set_analog_value(analog_pin pin, double value)
    {
        if (!use_analog)
        {
#ifdef _WIN32
            std::string msg = std::format("set_analog_value() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        std::print("[!] Not yet impl.\n");
        return true;
    }

    bool start_analog_pulse(analog_pin_continuous pin, Frequency_hz<double> rate)
    {
        if (!use_analog_co)
        {
#ifdef _WIN32
            std::string msg = std::format("start_analog_pulse() fails, analog io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        return start_analog_value_co(pin, rate);
        //return ppwm->start_pulse(io_id, period_us);
    }

    bool stop_analog_value_co(analog_pin_continuous pin)
    {
        
        if (!use_analog_co)
        {
#ifdef _WIN32
            std::string msg = std::format("stop_analog_value_co() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        if (!is_io_available(ios_co, IO_TYPE::ana_co))
        {
#ifdef _WIN32
            std::string msg = std::format("stop_analog_value_co() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        if (!is_io_registered(pin, m_analog_co_io_id))
        {
            std::print("[!] IO PIN is not valid\n");
#ifdef _WIN32
            std::string msg = std::format("stop_analog_value_co() fails, digital io not registered\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        io_co io_co_ = get_io_from_ios(ios_co, IO_TYPE::ana_co);

        task_mutex.lock();
        auto result = std::visit([&pin](auto&& arg) -> bool
            {
                return arg.stop(pin);
            }, io_co_);
        task_mutex.unlock();

        return result;
    }

    bool stop_analog_pulse(analog_pin_continuous pin)
    {
        if (!use_analog_co)
        {
#ifdef _WIN32
            std::string msg = std::format("start_analog_pulse() fails, analog io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        return stop_analog_value_co(pin);
        //return ppwm->start_pulse(io_id, period_us);
    }

    ~cppDaqimpl()
    {
        // RAII
        // 
        // As variant are suceptible to delete when copy move (design choice), 
        // raii is not possible inside all move semantics, copy objects
        // 
        // cleanup responsabilities occur here manually
        for (auto& io : ios)
        {
            // Free all tasks inside close impl
            task_mutex.lock();
            std::visit([](auto&& arg)
                {
                    arg.close();
                }, io);
            task_mutex.unlock();
        }

        for (auto& io_co : ios_co)
        {
            dbg::print("[*] ~cppDaqimpl() cleaning io_co\n");
            task_mutex.lock();
            std::visit([](auto&& arg)
                {
                    arg.close();
                }, io_co);
            task_mutex.unlock();
        }

        dbg::print("[*] cppDaqimpl dtor.\n");
    };

private:

    bool start_analog_value_co(analog_pin_continuous pin, Frequency_hz<double> rate)
    {
        if (!use_analog_co)
        {
#ifdef _WIN32
            std::string msg = std::format("set_analog_value_co() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        if (!is_io_available(ios_co, IO_TYPE::ana_co))
        {
#ifdef _WIN32
            std::string msg = std::format("set_analog_value_co() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        if (!is_io_registered(pin, m_analog_co_io_id))
        {
            std::print("[!] IO PIN is not valid\n");
#ifdef _WIN32
            std::string msg = std::format("set_analog_value_co() fails, digital io not registered\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        io_co io_co_ = get_io_from_ios(ios_co, IO_TYPE::ana_co);

        pin_t task_index(pin_to_vec_pos(pin, m_analog_co_io_id));
        task_mutex.lock();
        auto result = std::visit([&task_index, &rate](auto&& arg) -> bool
            {
                return arg.write(task_index, rate);
            }, io_co_);
        task_mutex.unlock();

        return result;
    }

    bool set_digital_value_co(digital_pin_continuous digital_io_id_co, Frequency_hz<double> rate)
    {
        if (!use_digital_co)
        {
#ifdef _WIN32
            std::string msg = std::format("set_digital_value_co() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }
        if (!is_io_available(ios_co, IO_TYPE::dig_co))
        {
#ifdef _WIN32
            std::string msg = std::format("set_digital_value_co() fails, digital io not available\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        if (!is_io_registered(digital_io_id_co, m_digital_co_io_id))
        {
            std::print("[!] IO PIN is not valid\n");
#ifdef _WIN32
            std::string msg = std::format("set_digital_value_co() fails, digital io not registered\n");
            MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
            return false;
        }

        io_co io_co_ = get_io_from_ios(ios_co, IO_TYPE::dig_co);

        pin_t task_index(pin_to_vec_pos(digital_io_id_co, m_digital_co_io_id));

        task_mutex.lock();
        auto result = std::visit([&task_index, &rate](auto&& arg) -> bool
            {
                return arg.write(task_index, rate);
            }, io_co_);
        task_mutex.unlock();

        return result;
    }

    // Use to find the position of the pin number: return vec pos
    size_t pin_to_vec_pos(pin_t pin, pin_list pins) {
        size_t pos = 0;
        for (auto& elem : pins)
        {
            // Pin value found, return vec pos
            if (pin.get() == elem)
            {
                return pos;
            }
            pos++;
        }
#ifdef _WIN32
        std::string msg = std::format("pin_to_vec_pos() fails\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        assert(false);
        return 0;
    };

    DAQ_STATUS m_status;
    std::string m_device;

    std::vector<io> ios;
    std::vector<io_co> ios_co;
	std::tuple<std::vector<io>, std::vector<io_co>> all_ios;

    analog_pins m_analog_io_id;
    digital_pins m_digital_io_id;
    analog_pins_continuous m_analog_co_io_id;
    digital_pins_continuous m_digital_co_io_id;

    bool use_analog;
    bool use_digital;
    bool use_analog_co;
    bool use_digital_co;

    std::unique_ptr<cPwm> ppwm;
};

cppDaq::cppDaq(const DaqConfig& config)
    : pdaq(std::make_unique<cppDaqimpl>(this, config.device, config.analog, config.digital, config.analog_continuous, config.digital_continuous)),
    m_config(config),
   status(DAQ_STATUS::NO_ERR)
{
    assert(pdaq);
    status = pdaq->get_status();
    if (status != DAQ_STATUS::NO_ERR)
    {
        std::print(RED"[!] cppDaq ctor failed to setup.\n");
        std::print(NC);
        pdaq.reset();
        return;
    }

    dbg::print("[*] cppDaq ctor.\n");
}

bool cppDaq::resetDaq()
{
	pdaq->~cppDaqimpl();
    std::print(CYN"[!] resetDaq() called.\n");
    std::print(NC);
	pdaq = std::make_unique<cppDaqimpl>(this, m_config.device, m_config.analog, m_config.digital, m_config.analog_continuous, m_config.digital_continuous);
    return true;
}

// Forwarder
cppDaq::cppDaq(const std::string device, const analog_pins ans, const digital_pins dis, const analog_pins_continuous ans_co, const digital_pins_continuous dis_co) :
    pdaq(std::make_unique<cppDaqimpl>(this, device, ans, dis, ans_co, dis_co)),
    status(DAQ_STATUS::DAQ_NOT_INITIALIZED)
{ 
    assert(pdaq);
    status = pdaq->get_status();
    if (status != DAQ_STATUS::NO_ERR)
    {
        std::print(RED"[!] cppDaq ctor failed to setup.\n");
        std::print(NC);
        pdaq.reset();
        return;
    }

    dbg::print("[*] cppDaq ctor.\n");
};

cppDaq::cppDaq(cppDaq&&) noexcept = default;
cppDaq& cppDaq::operator=(cppDaq&&) noexcept = default;

//

// Forwarder
DAQ_STATUS cppDaq::GetStatus()
{
    if (pdaq)
    {
        status= pdaq->get_status();
    }
    return status;
}

double cppDaq::ReadAnalogPin(analog_pin pin)
{
    if (!pdaq)
    {
        return NAN;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return NAN;
    }

    return pdaq->get_value(IO_TYPE::ana, pin);
}

std::array<double, 1000> cppDaq::ReadMultipleAnalogPin(analog_pin pin)
{
    std::array<double, 1000> arr = { 0 };

    if (!pdaq)
    {
        return arr;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return arr;
    }

    return arr;
}
//

// Forwarder
bool cppDaq::WriteDigitalPin(digital_pin pin, digital_state state)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->set_digital_value(pin, state);
}

// Forwarder
bool cppDaq::WriteAnalogPin(analog_pin pin, double value)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->set_analog_value(pin, value);
}
//

// Forwarder
bool cppDaq::StartDigitalCounterPWM(digital_pin_continuous pin, Frequency_hz<double> rate)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->start_digital_pulse(pin, rate);
}

// Forwarder
bool cppDaq::StopDigitalCounterPWM()
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->stop_digital_pulse();
}

// Forwarder
bool cppDaq::StartDigitalSoftwarePWM(digital_pin_continuous pin, Frequency_hz<double> rate)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->start_digital_pulse(pin, rate);
}

// Forwarder
bool cppDaq::StopDigitalSoftwarePWM()
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->stop_digital_pulse();
}

// Forwarder
bool cppDaq::UpdateDigitalSoftwarePWM(Frequency_hz<double> rate)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->update_digital_pulse(rate);
}

// Forwarder
bool cppDaq::StartAnalogPWM(analog_pin_continuous pin, Frequency_hz<double> rate)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->start_analog_pulse(pin, rate);
}

// Forwarder
bool cppDaq::StopAnalogPWM(analog_pin_continuous pin)
{
    if (!pdaq)
    {
        return false;
    }

    if (pdaq->get_status() != DAQ_STATUS::NO_ERR)
    {
        return false;
    }
    return pdaq->stop_analog_pulse(pin);
}

// Forwarder
cppDaq::~cppDaq() {
    dbg::print("[*] cppDaq dtor.\n");
};

std::vector<std::string> GetDeviceList()
{
    cppDaqFindDevice find;
    return find.get_list();
}
