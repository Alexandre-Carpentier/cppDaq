/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     26/12/2025
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <concepts>
// Goal: Make strong types
// 
// It is best to use strong types to avoid misusage of units
// in the API functions.
//

// TYPES:

// Period_us
// Frequency_hz

// Period s type
template<typename P> requires std::same_as<P, size_t> // Require a size_t type only
class Period_us
{
public:
    Period_us() = default;
    explicit constexpr Period_us(P micro_seconds) : m_period_us(micro_seconds) {}
    constexpr Period_us& operator=(P micro_seconds){ /* sanitize */ m_period_us = micro_seconds; return *this; }
    [[nodiscard]] constexpr P get() const { /* sanitize */ return m_period_us; }
private:
    P m_period_us;
};

// Frequency hertz type
template<typename F> requires std::same_as<F, double> // Require a double type only
class Frequency_hz
{
public:
    Frequency_hz() = default;
    explicit constexpr Frequency_hz(F hertz) : m_frequency_hz(hertz) { /* sanitize */}
    constexpr Frequency_hz& operator=(F hertz){ /* sanitize */m_frequency_hz = hertz; return *this; }
    [[nodiscard]] constexpr F get() const { /* sanitize */ return m_frequency_hz; }
private:
    F m_frequency_hz;
};

// Voltage volts type
template<typename V> requires std::same_as<V, double> // Require a double type only
class Voltage_v 
{
public:
    Voltage_v() = default;
    explicit constexpr Voltage_v(V volts) : m_voltage_v(volts) { /* sanitize */ }
    constexpr Voltage_v& operator=(V volts) { /* sanitize */ m_voltage_v = volts; return *this; }
    [[nodiscard]] constexpr V get() const { /* sanitize */ return m_voltage_v; }
private:
    double m_voltage_v; 
};