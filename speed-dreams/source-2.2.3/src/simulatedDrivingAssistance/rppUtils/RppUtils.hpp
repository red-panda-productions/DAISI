#pragma once
#include <string>

float stringToFloat(std::string s)
{
    try { return std::stof(s); }
    catch (std::exception& e) { return NAN; }
}