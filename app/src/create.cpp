/////////////////////////////////////////////////////////////////////////////
// Author:      Alexandre CARPENTIER
// Modified by:
// Created:     15/11/25
// Copyright:   (c) Alexandre CARPENTIER
// Licence:     LGPL-2.1-or-later
/////////////////////////////////////////////////////////////////////////////
#include "create.h"
#include "io_types.h"
#include "term_colors.h"
#include <print>
#include <format>
#include <vector>
#include <assert.h>
#ifdef WIN32
#include <Windows.h>
#endif

/// <Sanitize>
/// 
/// <To control user inputs>
/// <returns></returns>
/// 
bool sanitize_inputs(pin_list pins, const size_t max_input_count)
{
    if (pins.size() > max_input_count)
    {
#ifdef _WIN32
        std::string msg = std::format("[!] Error CreateIo::sanitize_inputs()\n");
        MessageBox(0, msg.c_str(), "Info", MB_ICONINFORMATION | S_OK);
#endif
        return false;
    }
    return true;
}



