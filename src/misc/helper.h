#pragma once

#include <vector>

namespace helper
{
    /// @brief Round up the input to the nearest multiples of unit
    /// @param input the input
    /// @param unit the unit
    /// @return the rounded-up number
    inline int roundUp(int input, int unit) { return (input / unit + 1) * unit; }
}