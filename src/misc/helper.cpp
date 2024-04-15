#include "helper.h"

int helper::roundUp(int input, int unit)
{
    //int output = std::pow(2, std::ceil(std::log2(input)));
    //while (output < unit) output *= 2;
    //return output;
    return std::ceil(static_cast<float>(input) / unit) * unit;
}
