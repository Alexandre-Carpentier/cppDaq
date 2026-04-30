/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     21/12/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <algorithm>
#include <concepts>
#include <vector>

// Goal: Make strong types
// 
// It is best to use strong types to avoid misusage of units
// in the API functions.
//

// TYPES:

// pin_t
// 	analog_pin
// 	digital_pin
// 	analog_pin_continuous
// 	digital_pin_continuous

// pin_list
// 	analog_pins
// 	digital_pins
// 	analog_pins_continuous
// 	digital_pins_continuous


// Generic pin type
class pin_t
{
public:
    constexpr pin_t() = default;
    explicit constexpr pin_t(size_t pin) : m_pin(pin) { /* sanitize */ }
    constexpr pin_t& operator=(size_t pin) { /* sanitize */m_pin = pin; return *this; }
    [[nodiscard]] constexpr size_t get() const { /* sanitize */ return m_pin; }
private:
    size_t m_pin;
};

// Analog pin_t type
struct analog_pin : public pin_t
{
    explicit constexpr analog_pin(size_t pin) : pin_t(pin) {}
};

// Digital pin type
struct digital_pin : public pin_t
{
    explicit constexpr digital_pin(size_t pin) : pin_t(pin) {}
};

// Analog pin continuous type	
struct analog_pin_continuous : public pin_t
{
    explicit constexpr analog_pin_continuous(size_t pin) : pin_t(pin) {}
};
// Digital pin continuous type
struct digital_pin_continuous : public pin_t
{

    explicit constexpr digital_pin_continuous(size_t pin) : pin_t(pin) {}
};

//  the generic pins type, declined in different types bellow

// Generic pins type
// Generic pin_t pin_list
// To protect arround a vector of size_t elems
class pin_list
{
public:
    // construct from {size_t}
    explicit pin_list(std::initializer_list<size_t> pins)
        : vec(pins)
    {
    }

    // construct from vector<size_t>
    explicit pin_list(std::vector<size_t> elements)
        : vec(std::move(elements))
    {
    }

    // construct from pin
    template<typename... Pins>
    explicit pin_list(Pins&&... pins)
        : vec{ std::forward<Pins>(pins).get()... }
    {
        static_assert((std::is_base_of_v<pin_t, std::decay_t<Pins>> && ...),"All args must herit from pin");
    }

    // Types d’itérateurs
    using iterator = std::vector<size_t>::iterator;
    using const_iterator = std::vector<size_t>::const_iterator;

    // Taille
    [[nodiscard]] size_t size() const noexcept
    {
        return vec.size();
    }

    // Accès élémentaire
    [[nodiscard]] size_t operator[](size_t index) const
    {
        return vec.at(index);
    }
    
    // Ajout sécurisé
    void push_back(const pin_t& pin)
    {
        vec.push_back(pin.get());
    }

    // Nettoyage
    void clear() noexcept
    {
        vec.clear();
    }

    // Accès au vecteur brut (lecture seule)
    [[nodiscard]] const std::vector<size_t>& data() const noexcept
    {
        return vec;
    }

    // Itérateurs
    iterator begin() noexcept { return vec.begin(); }
    iterator end() noexcept { return vec.end(); }
    const_iterator begin() const noexcept { return vec.begin(); }
    const_iterator end() const noexcept { return vec.end(); }
    const_iterator cbegin() const noexcept { return vec.cbegin(); }
    const_iterator cend() const noexcept { return vec.cend(); }

private:
    std::vector<size_t> vec;
};

struct analog_pins : public pin_list
{
    template<typename... Pins>
    explicit analog_pins(Pins&&... pins)
        : pin_list(std::forward<Pins>(pins)...)
    {
    }
    explicit analog_pins(std::initializer_list<size_t> pins)
        : pin_list(pins)
    {
    }
    explicit analog_pins(std::vector<size_t> pins)
        : pin_list(std::move(pins))
    {
    }
};

struct digital_pins : public pin_list
{
    template<typename... Pins>
    explicit digital_pins(Pins&&... pins)
        : pin_list(std::forward<Pins>(pins)...)
    {
    }
    explicit digital_pins(std::initializer_list<size_t> pins)
        : pin_list(pins)
    {
    }
    explicit digital_pins(std::vector<size_t> pins)
        : pin_list(std::move(pins))
    {
    }
};

// Analog pin_t type to use in cppDaq ctor
struct analog_pins_continuous : public pin_list
{
	template<typename... Ts>
	explicit analog_pins_continuous(Ts&&... elements) : pin_list(std::forward<Ts>(elements)...) {}

    explicit analog_pins_continuous(std::initializer_list<size_t> pins)
        : pin_list(pins)
    {
    }
    explicit analog_pins_continuous(std::vector<size_t> pins)
        : pin_list(std::move(pins))
    {
    }
};

// Digital pin_t type to use in cppDaq ctor
struct digital_pins_continuous : public pin_list
{
	template<typename... Ts>
	explicit digital_pins_continuous(Ts&&... elements) : pin_list(std::forward<Ts>(elements)...) {}

    explicit digital_pins_continuous(std::initializer_list<size_t> pins)
        : pin_list(pins)
    {
    }
    explicit digital_pins_continuous(std::vector<size_t> pins)
        : pin_list(std::move(pins))
    {
    }
};

enum class IO_TYPE
{
    ana = 0,
    dig,
    dig_co,
    ana_co,
    unknown
};

